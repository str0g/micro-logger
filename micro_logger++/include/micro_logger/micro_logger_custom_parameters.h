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

/// Custom parameters for configuring micro-logger formatting and output behavior.
///
/// All members except `message_size` should be initialized to their default values.
/// A future implementation will allow runtime customization.
struct micro_logger_CustomParameters {
  /// Total size of the header buffer (must be set to the default value; future
  /// implementation will allow customization).
  size_t header_size;

  /// Message buffer size. The minimal allowed value is 1; for performance
  /// reasons it should be kept below one memory page.
  size_t message_size;

  /// Printf-style header pattern (must be set to the default value; future
  /// implementation will allow customization). The pattern accepts three
  /// arguments: the formatted thread-info string, the desired aligned filename
  /// width, and the desired aligned line-number width.
  const char *header_pattern;

  /// Printf-style format string for the filename alignment width.
  const char *align_filename_length;

  /// Printf-style format string for the line-number alignment width.
  const char *align_lines_length;

  /// Format string for the time component, following the
  /// [strftime](https://en.cppreference.com/w/cpp/chrono/c/strftime) specification.
  const char *time_format;

  /// Optional format string for the millisecond component. Set to `nullptr` if
  /// millisecond formatting is not desired.
  const char *milliseconds_format;
};

#ifdef __cplusplus
}
#endif

#endif // MICRO_LOGGER_MICRO_LOGGER_CUSTOM_PARAMETERS_H
