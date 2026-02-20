/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_CUSTOM_PARAMETERS_H
#define MICRO_LOGGER_MICRO_LOGGER_CUSTOM_PARAMETERS_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct micro_logger_CustomParameters {
  size_t header_size;  // must be set to default value, future implementation
                       // will allow to customize
  size_t message_size; // minimal size is 1, for performance reason keep it bellow page size
  const char *header_pattern; // must be set to default value, future
                              // implementation will allow to customize
  const char *align_filename_length;
  const char *align_lines_length;
  /// https://en.cppreference.com/w/cpp/chrono/c/strftime
  const char *time_format;
  const char *milliseconds_format; /// optional set to nullptr if not used
};

#ifdef __cplusplus
}
#endif

#endif // MICRO_LOGGER_MICRO_LOGGER_CUSTOM_PARAMETERS_H
