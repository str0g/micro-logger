/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "micro_logger.h"

#include <stdarg.h>

#include "micro_logger.hpp"

const char* LVL_TRACE = micro_logger::LVL_TRACE;
const char* LVL_DEBUG = micro_logger::LVL_DEBUG;
const char* LVL_INFO = micro_logger::LVL_INFO;
const char* LVL_WARN = micro_logger::LVL_WARN;
const char* LVL_ERROR = micro_logger::LVL_ERROR;
const char* LVL_CRITICAL = micro_logger::LVL_CRITICAL;

void* micro_logger_get_silent_writer() {
    static micro_logger::SilentWriter instance;
    return &instance;
}

void* micro_logger_get_stdout_writer() {
    static micro_logger::StandardOutWriter instance;
    return &instance;
}

void* micro_logger_get_net_writer(const char* address, int port) {
    static micro_logger::NetworkWriter instance(address, port);
    return &instance;
}

void* micro_logger_get_file_writer(const char* path) {
    static micro_logger::FileWriter instance(path);
    return &instance;
}

void micro_logger_set_writer(void *in) {
    if(not in) {
        throw std::runtime_error("base writer can not be null");
    }
    auto writer = reinterpret_cast<micro_logger::BaseWriter*>(in);
    micro_logger::set_writer(*writer);
}

void micro_logger_logme(const char *level, const char *file, const char* func, int line, const char* fmt, ...) {
    char buf[micro_logger::default_parameters.message_size];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    micro_logger::__logme(level, file, func, line, buf);
}
