/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "micro_logger.hpp"
#include "thread_info.h"
//
#include <chrono>
#include <cstring>
#include <memory>
#include <mutex>
#include <stdarg.h>
#include <unordered_map>

namespace micro_logger {
const micro_logger_CustomParameters *custom_parameters = nullptr;
std::mutex sync_write;
std::mutex sync_init_header_formatter;

void set_custom_parameters(micro_logger_CustomParameters *custom) {
  custom_parameters = custom;
}

const BaseWriter *custom_writer = nullptr;
void set_writer(const BaseWriter &writer) {
  if (not custom_writer)
    custom_writer = &writer;
}

const std::string &init_header_formatter() {
  std::lock_guard<std::mutex> lock(sync_init_header_formatter);
  static std::unordered_map<std::thread::id, std::unique_ptr<std::string>>
      cached_patterns;
  auto thread_id = std::this_thread::get_id();
  if (auto &obj = cached_patterns[thread_id]) {
    return *obj;
  } else {
    ThreadInfo thread_info;
    if (!custom_parameters) {
      custom_parameters = &default_parameters;
    }

    char buf[custom_parameters->header_size];
    std::snprintf(buf, sizeof(buf), "[%%s]%s[%%%ss:%%%sd::%%s][%%s]\n",
                  thread_info.info.c_str(),
                  custom_parameters->align_filename_length,
                  custom_parameters->align_lines_length);
    cached_patterns[thread_id] = std::make_unique<std::string>(&buf[0]);
    return *cached_patterns[thread_id];
  }
}

size_t get_time(char *output) {
  const auto current_time_point{std::chrono::system_clock::now()};
  const auto t{std::chrono::system_clock::to_time_t(current_time_point)};
  const auto current_time_since_epoch{current_time_point.time_since_epoch()};
  const auto current_milliseconds{
      std::chrono::duration_cast<std::chrono::milliseconds>(
          current_time_since_epoch)
          .count() %
      1000};
  std::tm tm{};
  auto time_info = ::localtime_r(&t, &tm);
  size_t size = 0;
  if (time_info) {
    size = std::strftime(output, custom_parameters->header_size,
                         custom_parameters->time_format, time_info);
  }
  if (size and custom_parameters->milliseconds_format) {
    size += std::snprintf(output + size, custom_parameters->header_size - size,
                          custom_parameters->milliseconds_format,
                          current_milliseconds);
  }
  return size;
}

void __logme(const char *level, const char *file, const char *func, int line,
             const char *fmt, ...) {
  const std::string &header_formatter{init_header_formatter()};
  char message[custom_parameters->message_size];
  static size_t output_size{sizeof(message) + custom_parameters->header_size};
  // message
  va_list args;
  va_start(args, fmt);
  std::vsnprintf(message, sizeof(message), fmt, args);
  va_end(args);
  //
  char output[output_size];
  auto current_time_size = get_time(output);
  auto size =
      std::snprintf(output + current_time_size, output_size - current_time_size,
                    header_formatter.c_str(), level, file, line, func, message);
  //
  const std::lock_guard<std::mutex> lock(sync_write);
  custom_writer->write(output, size + current_time_size);
}
} // namespace micro_logger
