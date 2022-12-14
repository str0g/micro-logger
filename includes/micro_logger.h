/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_H
#define MICRO_LOGGER_MICRO_LOGGER_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * C version has certain limitations
 * - definitely slower than c++ one.
 * - not configurable message size, check default one from @micro_logger.hpp and
 * keep default size while customizing.
 * - runtime file name shifting
 */
void *micro_logger_get_stdout_writer();

void *micro_logger_get_net_writer(const char *address, int port);

void *micro_logger_get_file_writer(const char *path);

void micro_logger_set_writer(void *);

void micro_logger_logme(const char *level, const char *file, const char *func,
                        int line, const char *fmt, ...);

#ifndef NODEBUG
const char *LVL_TRACE = "TRACE";
const char *LVL_DEBUG = "DEBUG";
#endif
const char *LVL_INFO = "INFO ";
const char *LVL_WARN = "WARN ";
const char *LVL_ERROR = "ERROR";
const char *LVL_CRITICAL = "CRITI";

#define CURRENT_FILE_NAME_ONLY_                                                \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__) // runtime

#ifndef NODEBUG
#define MSG_DEBUG(fmt, ...)                                                    \
  micro_logger_logme(LVL_DEBUG, CURRENT_FILE_NAME_ONLY_, __FUNCTION__,         \
                     __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ENTER()                                                            \
  micro_logger_logme(LVL_TRACE, CURRENT_FILE_NAME_ONLY_, __FUNCTION__,         \
                     __LINE__, "%s", "--ENTER--")
#define MSG_EXIT()                                                             \
  micro_logger_logme(LVL_TRACE, CURRENT_FILE_NAME_ONLY_, __FUNCTION__,         \
                     __LINE__, "%s", "--EXIT--")
#else
#define MSG_DEBUG(fmt, ...)
#define MSG_ENTER()
#define MSG_EXIT()
#endif

#define MSG_INFO(fmt, ...)                                                     \
  micro_logger_logme(LVL_INFO, CURRENT_FILE_NAME_ONLY_, __FUNCTION__,          \
                     __LINE__, fmt, ##__VA_ARGS__)
#define MSG_WARN(fmt, ...)                                                     \
  micro_logger_logme(LVL_WARN, CURRENT_FILE_NAME_ONLY_, __FUNCTION__,          \
                     __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ERROR(fmt, ...)                                                    \
  micro_logger_logme(LVL_ERROR, CURRENT_FILE_NAME_ONLY_, __FUNCTION__,         \
                     __LINE__, fmt, ##__VA_ARGS__)
#define MSG_CRITICAL(fmt, ...)                                                 \
  micro_logger_logme(LVL_CRITICAL, CURRENT_FILE_NAME_ONLY_, __FUNCTION__,      \
                     __LINE__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // MICRO_LOGGER_MICRO_LOGGER_H
