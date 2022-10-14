//
// Created by lukasz on 13.10.2022.
//

#include "micro_logger.h"
#include "thread_info.h"
#include <iostream>

namespace micro_logger {
    thread_local ThreadInfo thead_info;
    const DefaultParameters* custom_parameters = nullptr;

    void set_custom_parameters(DefaultParameters*) {
        custom_parameters = &default_parameters;
    }

    std::string init_header_formatter() {
        if (!custom_parameters) {
            custom_parameters = &default_parameters;
        }

        char buf[128];
        std::snprintf(buf, sizeof(buf), "[%%s][%%%ss:%%%sd]%s",
                      custom_parameters->align_filename_length,
                      custom_parameters->align_lines_length,
                      thead_info.info.c_str());
        return buf;
    }

    void __logme(const char *level, const char *file, int line, const char *message) {
        std::string header_formatter {init_header_formatter()};
        std::cout << "header:" << header_formatter << "\n";
        char header[128];
        std::snprintf(header, sizeof(header), header_formatter.c_str(), level, file, line);
        std::cout << header << message << "\n";
    }
}