/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_CUSTOM_PARAMTERS_H
#define MICRO_LOGGER_MICRO_LOGGER_CUSTOM_PARAMTERS_H

#include <sys/types.h>
extern "C" {

struct micro_logger_CustomParameters {
    size_t header_size;
    size_t message_size;
    const char *align_filename_length;
    const char *align_lines_length;
    const char *time_format; /// https://en.cppreference.com/w/cpp/chrono/c/strftime
};

/**
 * Use if need custom options and make sure that @DefaultParameters instance will be available until program terminates
 */
void set_custom_parameters(micro_logger_CustomParameters*);
}

#endif //MICRO_LOGGER_MICRO_LOGGER_CUSTOM_PARAMTERS_H
