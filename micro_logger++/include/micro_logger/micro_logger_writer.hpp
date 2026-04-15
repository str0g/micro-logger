/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP
#define MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP

#include <concepts>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>

namespace micro_logger {
class BaseWriter {
public:
  BaseWriter() = default;
  virtual ~BaseWriter() = default;
  virtual size_t write(const char *buf, size_t) const = 0;

  BaseWriter(const BaseWriter &) = delete;
  BaseWriter(BaseWriter &&) = delete;
  BaseWriter &operator=(const BaseWriter &) = delete;
  BaseWriter &operator=(BaseWriter &&) = delete;
};

class SilentWriter : public BaseWriter {
public:
  using BaseWriter::BaseWriter;
  size_t write(const char *buf, size_t) const final;
};

class StandardOutWriter : public BaseWriter {
public:
  size_t write(const char *buf, size_t) const final;
};

class FileWriter : public BaseWriter {
public:
  explicit FileWriter(const char *path);
  size_t write(const char *buf, size_t) const final;
  ~FileWriter();

private:
  mutable std::ofstream outfile;
};

// read with netcat -l -p <port>
class NetworkWriter : public BaseWriter {
public:
  explicit NetworkWriter(const std::string &address, int port);
  size_t write(const char *buf, size_t) const final;
  ~NetworkWriter();

private:
  int sock;
  int client_fd;
  std::string address;
  int port;
  void reconnect();
  void reset_socket();
};

class AsyncWriter : public BaseWriter {
public:
  explicit AsyncWriter(std::unique_ptr<BaseWriter> &output);
  size_t write(const char *buf, size_t size) const final;
  ~AsyncWriter();

protected:
  void stop();
  void worker();
  int64_t find_available_entry() const;

protected:
  struct Entry {
    char line[128 + 1024 + 1];
    size_t size;
    inline bool is_taken() { return size != 0; }
    inline void reclaim() { size = 0; }
    inline void update(const char *in, size_t size) {
      std::memcpy(line, in, size);
      this->size = size;
    }
  };

protected:
  static constexpr int64_t max_entries{1000};
  mutable std::unique_ptr<BaseWriter> output;
  mutable std::array<Entry, max_entries> queue;
  mutable std::mutex sync;
  mutable std::condition_variable cv;
  std::thread thread;
  bool run;
  int64_t current_entry_index;
};

} // namespace micro_logger

#endif // MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP
