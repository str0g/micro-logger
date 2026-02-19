/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef MICRO_LOGGER_MICRO_LOGGER_H
#define MICRO_LOGGER_MICRO_LOGGER_H

#include "micro_logger_custom_parameters.h"

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

const char *micro_logger_basename(const char *const filename);

extern const char *MICRO_LOGGER_LVL_TRACE;
extern const char *MICRO_LOGGER_LVL_DEBUG;
extern const char *MICRO_LOGGER_LVL_INFO;
extern const char *MICRO_LOGGER_LVL_WARN;
extern const char *MICRO_LOGGER_LVL_ERROR;
extern const char *MICRO_LOGGER_LVL_CRITICAL;

#ifndef NODEBUG
#define MSG_DEBUG(fmt, ...)                                                    \
  micro_logger_logme(MICRO_LOGGER_LVL_DEBUG, micro_logger_basename(__FILE__), __FUNCTION__, \
                     __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ENTER()                                                            \
  micro_logger_logme(MICRO_LOGGER_LVL_TRACE, micro_logger_basename(__FILE__), __FUNCTION__, \
                     __LINE__, "%s", "--ENTER--")
#define MSG_EXIT()                                                             \
  micro_logger_logme(MICRO_LOGGER_LVL_TRACE, micro_logger_basename(__FILE__), __FUNCTION__, \
                     __LINE__, "%s", "--EXIT--")
#else
#define MSG_DEBUG(fmt, ...)
#define MSG_ENTER()
#define MSG_EXIT()
#endif

#define MSG_INFO(fmt, ...)                                                     \
  micro_logger_logme(MICRO_LOGGER_LVL_INFO, micro_logger_basename(__FILE__), __FUNCTION__,  \
                     __LINE__, fmt, ##__VA_ARGS__)
#define MSG_WARN(fmt, ...)                                                     \
  micro_logger_logme(MICRO_LOGGER_LVL_WARN, micro_logger_basename(__FILE__), __FUNCTION__,  \
                     __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ERROR(fmt, ...)                                                    \
  micro_logger_logme(MICRO_LOGGER_LVL_ERROR, micro_logger_basename(__FILE__), __FUNCTION__, \
                     __LINE__, fmt, ##__VA_ARGS__)
#define MSG_CRITICAL(fmt, ...)                                                 \
  micro_logger_logme(MICRO_LOGGER_LVL_CRITICAL, micro_logger_basename(__FILE__),            \
                     __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // MICRO_LOGGER_MICRO_LOGGER_H
