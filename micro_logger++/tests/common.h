/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once
#include "micro_logger_writer.hpp"
//
#include <vector>

class TestWriter : public micro_logger::BaseWriter {
public:
  using BaseWriter::BaseWriter;
  size_t write(const char *buf, size_t size) const final {
    std::string tmp{buf, size};
    line_bufer.emplace_back(tmp);
    return size;
  }
  inline static TestWriter &get_instatnce() {
    static TestWriter obj;
    return obj;
  }

public:
  mutable std::vector<std::string> line_bufer;
};
