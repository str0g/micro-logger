/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP
#define MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP

#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>

namespace micro_logger {

/**
 * @brief Abstract interface for a log message sink.
 *
 * Implementations decide where log lines go (stdout, file, network, etc.).
 * Writers are stateless for the `write` call so they can be safely copied
 * into the `AsyncWriter` ring buffer without data races.
 *
 *
 */
class BaseWriter {
public:
  BaseWriter() = default;
  virtual ~BaseWriter() = default;

  /**
   * @brief Write a single log line to the destination.
   *
   * @param buf     Buffer containing the formatted log line.
   * @param size    Length of the log line in bytes.
   * @return        Number of bytes actually written.
   */
  virtual size_t write(const char *buf, size_t size) const = 0;

  BaseWriter(const BaseWriter &) = delete;
  BaseWriter(BaseWriter &&) = delete;
  BaseWriter &operator=(const BaseWriter &) = delete;
  BaseWriter &operator=(BaseWriter &&) = delete;
};

/**
 * @brief A writer that discards all log messages.
 *
 * Useful for building applications with zero logging overhead.
 */
class SilentWriter : public BaseWriter {
public:
  size_t write(const char *buf, size_t size) const final;
};

/**
 * @brief A writer that sends log messages to standard output.
 */
class StandardOutWriter : public BaseWriter {
public:
  size_t write(const char *buf, size_t size) const final;
};

/**
 * @brief A writer that sends log messages to a file.
 *
 * The file is opened in append mode when the writer is constructed.
 */
class FileWriter : public BaseWriter {
public:
  /**
   * @brief Open (or append to) the file at the given path.
   * @param path  Absolute or relative filesystem path.
   */
  explicit FileWriter(const char *path);
  size_t write(const char *buf, size_t size) const final;
  ~FileWriter();

private:
  mutable std::ofstream outfile;
};

/**
 * @brief A writer that sends log messages over a TCP connection.
 *
 * Connects to the given address:port and writes each log line as a
 * newline-terminated message.  If the connection drops the writer
 * automatically attempts to reconnect.
 * Receiving server can be emulated by netcat -l -p \p {port}
 */
class NetworkWriter : public BaseWriter {
public:
  /**
   * @brief Connect to the given remote endpoint.
   * @param address  IP address or hostname (e.g. `"127.0.0.1"`).
   * @param port     TCP port number (e.g. `55514`).
   */
  explicit NetworkWriter(const std::string &address, int port);
  size_t write(const char *buf, size_t size) const final;
  ~NetworkWriter();

private:
  int sock;
  int client_fd;
  std::string address;
  int port;
  void reconnect();
  void reset_socket();
};

/**
 * @brief A lock-free (mutex-protected) ring-buffer writer.
 *
 * Producer threads call `write()` which copies the log line into a free
 * slot and signals a background worker thread.  The worker drains the
 * queue and forwards messages to the wrapped downstream writer.
 *
 * This class decouples the fast path (producer side) from the slow
 * I/O path, improving latency for log-heavy hot loops.
 */
class AsyncWriter : public BaseWriter {
public:
  /**
   * @brief Wrap the given downstream writer.
   * @param output  Ownership is transferred to AsyncWriter.
   */
  explicit AsyncWriter(std::unique_ptr<BaseWriter> &output);
  size_t write(const char *buf, size_t size) const final;
  ~AsyncWriter();

protected:
  /** @brief Stop the worker thread and flush remaining entries. */
  void stop();
  /** @brief Worker thread entry point — drains the queue. */
  void worker();
  /** @brief Find a free slot in the ring buffer, or -1 if full. */
  int64_t find_available_entry() const;

protected:
  /**
   * @brief One slot in the ring buffer.
   *
   * `size == 0` means the slot is free; `size > 0` means it holds a
   * pending log line waiting to be drained by the worker.
   */
  struct Entry {
    /** One ring-bucket: a line buffer plus its length (0 = free). */
    char line[128 + 1024 + 1];
    /** Payload length; zero means the slot is free. */
    size_t size;
    /** True when the slot holds a pending log line. */
    inline bool is_taken() { return size != 0; }
    /** Return the slot to the pool by zeroing its length. */
    inline void reclaim() { size = 0; }
    /** Copy @p in bytes from @p in into @p line and record the length. */
    inline void update(const char *in, size_t size) {
      std::memcpy(line, in, size);
      this->size = size;
    }
  };

protected:
  /** Number of slots in the ring buffer. */
  static constexpr int64_t max_entries{1000};
  /** Destination writer (forwarded to in the worker thread). */
  mutable std::unique_ptr<BaseWriter> output;
  /** Ring buffer of Entry slots shared between producer and worker threads. */
  mutable std::array<Entry, max_entries> queue;
  /** Guards @p output, @p queue, @p run, and @p current_entry_index. */
  mutable std::mutex sync;
  /** Signalled when a new entry is enqueued. */
  mutable std::condition_variable cv;
  /** Worker thread that drains the queue and forwards to @p output. */
  std::thread thread;
  /** Whether the worker loop should keep running. */
  bool run;
  /** Index of the next slot the producer will try to claim. */
  int64_t current_entry_index;
};

} // namespace micro_logger

#endif // MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP
