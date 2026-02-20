/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once
#include "micro_logger/micro_logger_writer.hpp"
//
#include <ostream>
#include <sstream>
#include <thread>
#include <vector>

class TestWriter : public micro_logger::BaseWriter {
public:
  using BaseWriter::BaseWriter;
  size_t write(const char *buf, size_t size) const final {
    std::string tmp{buf, size};
    line_buffer.emplace_back(tmp);
    return size;
  }
  inline static TestWriter &get_instance() {
    static TestWriter obj;
    return obj;
  }

public:
  mutable std::vector<std::string> line_buffer;
};

struct logged_data {
  const std::string &data;
  const std::string &time;
  const std::string &log_level;
  pid_t pid;
  size_t tid;
  const std::string &filename;
  const std::string &line_number;
  const std::string &function;
  const std::string &message;
  friend std::ostream &operator<<(std::ostream &os, const logged_data &obj) {
    os << "(ignored)data: " << obj.data << std::endl;
    os << "(ignored)time: " << obj.time << std::endl;
    os << "log level: " << obj.log_level << std::endl;
    os << "pid: " << obj.pid << std::endl;
    os << "tid: " << obj.tid << std::endl;
    os << "filename: " << obj.filename << std::endl;
    os << "line_number: " << obj.line_number << std::endl;
    os << "function: " << obj.function << std::endl;
    os << "message: " << obj.message << std::endl;
    return os;
  }
};

struct logged_data_exp {
  std::string data;
  std::string time;
  std::string log_level;
  pid_t pid;
  size_t tid;
  std::string filename;
  std::string line_number;
  std::string function;
  std::string message;
  friend std::ostream &operator<<(std::ostream &os,
                                  const logged_data_exp &obj) {
    os << "(ignored)data: " << obj.data << std::endl;
    os << "(ignored)time: " << obj.time << std::endl;
    os << "log level: " << obj.log_level << std::endl;
    os << "pid: " << obj.pid << std::endl;
    os << "tid: " << obj.tid << std::endl;
    os << "filename: " << obj.filename << std::endl;
    os << "line_number: " << obj.line_number << std::endl;
    os << "function: " << obj.function << std::endl;
    os << "message: " << obj.message << std::endl;
    return os;
  }
};

bool operator==(const logged_data &left, const logged_data_exp &right) {
  return ((left.log_level == right.log_level) and (left.pid == right.pid) and
          (left.tid == right.tid) and (left.filename == right.filename) and
          (left.line_number == right.line_number) and
          (left.function == right.function) and
          (left.message == right.message));
}

size_t get_tid() {
  size_t tid;
  std::stringstream os;
  os << std::this_thread::get_id();
  os >> tid;
  return tid;
}
