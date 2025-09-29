/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_HPP
#define MICRO_LOGGER_MICRO_LOGGER_HPP

#include "micro_logger_custom_paramters.h"
#include "micro_logger_writer.hpp"

namespace micro_logger {
/**
 * for performance reasons keep @header_size + message_size below page size.
 */
constexpr micro_logger_CustomParameters default_parameters{
    128, 1024, "", "03", "[%D %T", ".%03ld]"};

/**
 * Before any logging operation is mandatory to set writer
 * this function called again is not going to do anything.
 */
void set_writer(const BaseWriter &);
/**
 *  Do not use directly
 * @param level
 * @param file
 * @param line
 * @param message
 */
void __logme(const char *level, const char *file, const char *func, int line,
             const char *fmt, ...);

consteval const char *basename(const char *const path) {
  int slash_index = -1;
  int index = 0;
  do {
    if (path[index] == '/') {
      slash_index = index + 1;
    }
  } while (path[index++]);
  return &path[slash_index == -1 ? 0 : slash_index];
}

#ifndef NODEBUG
constexpr const char *LVL_TRACE = "TRACE";
constexpr const char *LVL_DEBUG = "DEBUG";
#endif
constexpr const char *LVL_INFO = "INFO ";
constexpr const char *LVL_WARN = "WARN ";
constexpr const char *LVL_ERROR = "ERROR";
constexpr const char *LVL_CRITICAL = "CRITI";

#define __FILE_ONLY__ micro_logger::basename(__FILE__)

} // namespace micro_logger

#ifndef USE_C_VERSION

#ifndef NODEBUG
#define MSG_DEBUG(fmt, ...)                                                    \
  micro_logger::__logme(micro_logger::LVL_DEBUG, __FILE_ONLY__, __FUNCTION__,  \
                        __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ENTER()                                                            \
  micro_logger::__logme(micro_logger::LVL_TRACE, __FILE_ONLY__, __FUNCTION__,  \
                        __LINE__, "%s", "--ENTER--")
#define MSG_EXIT()                                                             \
  micro_logger::__logme(micro_logger::LVL_TRACE, __FILE_ONLY__, __FUNCTION__,  \
                        __LINE__, "%s", "--EXIT--")
#else
#define MSG_DEBUG(fmt, ...)
#define MSG_ENTER()
#define MSG_EXIT()
#endif

#define MSG_INFO(fmt, ...)                                                     \
  micro_logger::__logme(micro_logger::LVL_INFO, __FILE_ONLY__, __FUNCTION__,   \
                        __LINE__, fmt, ##__VA_ARGS__)
#define MSG_WARN(fmt, ...)                                                     \
  micro_logger::__logme(micro_logger::LVL_WARN, __FILE_ONLY__, __FUNCTION__,   \
                        __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ERROR(fmt, ...)                                                    \
  micro_logger::__logme(micro_logger::LVL_ERROR, __FILE_ONLY__, __FUNCTION__,  \
                        __LINE__, fmt, ##__VA_ARGS__)
#define MSG_CRITICAL(fmt, ...)                                                 \
  micro_logger::__logme(micro_logger::LVL_CRITICAL, __FILE_ONLY__,             \
                        __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#endif // USE_C_VERSION

#endif // MICRO_LOGGER_MICRO_LOGGER_HPP
