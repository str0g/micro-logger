/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef MICRO_LOGGER_MICRO_LOGGER_H
#define MICRO_LOGGER_MICRO_LOGGER_H

#include "micro_logger_custom_paramters.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * C version has certain limitations
 * - definitely slower than c++ one.
 * - runtime file name shifting
 */
void *micro_logger_get_silent_writer();

void *micro_logger_get_stdout_writer();

void *micro_logger_get_net_writer(const char *address, int port);

void *micro_logger_get_file_writer(const char *path);
/*
 * @param use one of @micro_logger_get_silent_writer
 * @micro_logger_get_stdout_writer @micro_logger_get_net_writer
 * @micro_logger_get_file_writer to get correct writer
 * @param parameter if not provided set to NULL
 */
void micro_logger_initialize(void *writer,
                              struct micro_logger_CustomParameters *parameter);

void micro_logger_logme(const char *level, const char *file, const char *func,
                        int line, const char *fmt, ...);

const char *current_file_name_only(const char *const filename);

#ifndef NODEBUG
extern const char *LVL_TRACE;
extern const char *LVL_DEBUG;
#endif
extern const char *LVL_INFO;
extern const char *LVL_WARN;
extern const char *LVL_ERROR;
extern const char *LVL_CRITICAL;

#define CURRENT_FILE_NAME_ONLY_ current_file_name_only(__FILE__)

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
