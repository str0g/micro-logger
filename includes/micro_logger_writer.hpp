/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP
#define MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP

#include <fstream>

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
} // namespace micro_logger

#endif // MICRO_LOGGER_MICRO_LOGGER_WRITER_HPP
