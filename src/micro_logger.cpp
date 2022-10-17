/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "micro_logger.hpp"
#include "thread_info.h"

#include <stdarg.h>
#include <mutex>
#include <cstring>
#include <chrono>

namespace micro_logger {
    thread_local ThreadInfo thead_info;
    const micro_logger_CustomParameters* custom_parameters = nullptr;
    std::mutex sync_write;

    void set_custom_parameters(micro_logger_CustomParameters*) {
        custom_parameters = &default_parameters;
    }

    const BaseWriter* custom_writer = nullptr;
    void set_writer(const BaseWriter& writer) {
        if(not custom_writer)
            custom_writer = &writer;
    }

    std::string init_header_formatter() {
        if (!custom_parameters) {
            custom_parameters = &default_parameters;
        }

        char buf[128];
        std::snprintf(buf, sizeof(buf), "[%%s]%s[%%%ss:%%%sd::%%s][%%s]\n",
                      thead_info.info.c_str(),
                      custom_parameters->align_filename_length,
                      custom_parameters->align_lines_length
                      );
        return buf;
    }

    size_t get_time(char* output) {
        std::time_t t = std::time(nullptr);
        return std::strftime(output,
                             custom_parameters->header_size,
                             custom_parameters->time_format,
                             std::localtime(&t));
    }

    void __logme(const char *level, const char *file, const char*func, int line, const char* fmt, ...) {
        std::string header_formatter {init_header_formatter()};
        char message[custom_parameters->message_size];
        static size_t output_size {sizeof(message) + custom_parameters->header_size};
        // message
        va_list args;
        va_start(args, fmt);
        std::vsnprintf(message, sizeof(message), fmt, args);
        va_end(args);
        //
        char output[output_size];
        auto current_time_size = get_time(output);
        auto size = std::snprintf(output+current_time_size,
                                  output_size - current_time_size,
                                  header_formatter.c_str(),
                                  level,
                                  file,
                                  line,
                                  func,
                                  message);
        //
        const std::lock_guard<std::mutex> lock(sync_write);
        custom_writer->write(output, size + current_time_size);
    }
}