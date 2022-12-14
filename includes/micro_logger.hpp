/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_HPP
#define MICRO_LOGGER_MICRO_LOGGER_HPP

#include "micro_logger_writer.hpp"
#include "micro_logger_custom_paramters.h"

namespace micro_logger {
    /**
     * for performance reasons keep @header_size + message_size below page size.
     */
    constexpr micro_logger_CustomParameters default_parameters {
            128,
            1024,
            "",
            "03",
            "[%D %T]"
    };

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
#ifndef USE_C_VERSION

#ifndef NODEBUG
const char* LVL_TRACE       = "TRACE";
const char* LVL_DEBUG       = "DEBUG";
#endif
const char* LVL_INFO        = "INFO ";
const char* LVL_WARN        = "WARN ";
const char* LVL_ERROR       = "ERROR";
const char* LVL_CRITICAL    = "CRITI";

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
#endif // USE_C_VERSION

#endif //MICRO_LOGGER_MICRO_LOGGER_HPP
