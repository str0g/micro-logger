//
// Created by lukasz on 13.10.2022.
//

#include "micro_logger.h"
#include "thread_info.h"

#include <stdarg.h>
#include <mutex>
#include <cstring>

namespace micro_logger {
    thread_local ThreadInfo thead_info;
    const DefaultParameters* custom_parameters = nullptr;
    std::mutex sync_write;

    void set_custom_parameters(DefaultParameters*) {
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
        auto size = std::snprintf(output, output_size, header_formatter.c_str(), level, file, line, func, message);
        //
        const std::lock_guard<std::mutex> lock(sync_write);
        custom_writer->write(output, size);
    }
}