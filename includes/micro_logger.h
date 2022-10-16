/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_H
#define MICRO_LOGGER_MICRO_LOGGER_H

#include "micro_logger_writer.h"


namespace micro_logger {
    struct DefaultParameters {
        size_t header_size;
        size_t message_size;
        const char *align_filename_length;
        const char *align_lines_length;
        const char *time_format; /// https://en.cppreference.com/w/cpp/chrono/c/strftime
    };

    /**
     * for performance reasons keep @header_size + message_size below page size.
     */
    constexpr DefaultParameters default_parameters {
            128,
            1024,
            "",
            "03",
            "[%D %T]"
    };
    /**
     * Use if need custom options and make sure that @DefaultParameters instance will be available until program terminates
     */
    void set_custom_parameters(DefaultParameters*);

    /**
     * Before any logging operation is mandatory to set writer
     * this function called again is not going to do anything.
     */
    void set_writer(const BaseWriter&);
    /**
     *  Do not use directly
     * @param level
     * @param file
     * @param line
     * @param message
     */
    void __logme(const char *level, const char *file, const char*func, int line, const char* fmt, ...);

    /**
 * https://stackoverflow.com/a/19004720
 * @param path
 * @param index
 * @param slash_index
 * @return
 */
    constexpr int32_t basename_index (const char * const path, const int32_t index = 0, const int32_t slash_index = -1) {
        return path [index]
               ? ( path [index] == '/'
                   ? basename_index (path, index + 1, index)
                   : basename_index (path, index + 1, slash_index)
               )
               : (slash_index + 1)
                ;
    }
}

#ifndef NODEBUG
#define LVL_TRACE       "TRACE"
#define LVL_DEBUG       "DEBUG"
#endif
#define LVL_INFO        "INFO "
#define LVL_WARN        "WARN "
#define LVL_ERROR       "ERROR"
#define LVL_CRITICAL    "CRITI"

#define __FILE_ONLY__ ({ static const int32_t basename_idx = micro_logger::basename_index(__FILE__); \
                        static_assert (basename_idx >= 0, "compile-time basename");   \
                        __FILE__ + basename_idx;})

#ifndef NODEBUG
#define MSG_DEBUG(fmt, ...) \
    micro_logger::__logme(LVL_DEBUG, __FILE_ONLY__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ENTER() \
    micro_logger::__logme(LVL_TRACE, __FILE_ONLY__, __FUNCTION__, __LINE__, "%s", "--ENTER--")
#define MSG_EXIT() \
    micro_logger::__logme(LVL_TRACE, __FILE_ONLY__, __FUNCTION__, __LINE__, "%s", "--EXIT--")
#else
#define MSG_DEBUG(fmt, ...)
#define MSG_ENTER()
#define MSG_EXIT()
#endif

#define MSG_INFO(fmt, ...) \
    micro_logger::__logme(LVL_INFO, __FILE_ONLY__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MSG_WARN(fmt, ...) \
    micro_logger::__logme(LVL_WARN, __FILE_ONLY__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ERROR(fmt, ...) \
    micro_logger::__logme(LVL_ERROR, __FILE_ONLY__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define MSG_CRITICAL(fmt, ...) \
    micro_logger::__logme(LVL_CRITICAL, __FILE_ONLY__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif //MICRO_LOGGER_MICRO_LOGGER_H
