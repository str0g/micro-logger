//
// Created by lukasz on 13.10.2022.
//

#ifndef MICRO_LOGGER_MICRO_LOGGER_H
#define MICRO_LOGGER_MICRO_LOGGER_H

#include <string>
#include <vector>
namespace micro_logger {
    struct DefaultParameters {
        const char *align_filename_length;
        const char *align_lines_length;
    };

    constexpr DefaultParameters default_parameters {
            "25",
            "03"
    };
    /**
     * Use if need custom options and make sure that @DefaultParameters instance will be available until program terminates
     */
    void set_custom_parameters(DefaultParameters*);
    /**
     *  Do not use directly
     * @param level
     * @param file
     * @param line
     * @param message
     */
    void __logme(const char *level, const char *file, const char*func, int line, const char *message);

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
#define LVL_CRITIACL    "CRITI"

#define __FILE_ONLY__ ({ static const int32_t basename_idx = micro_logger::basename_index(__FILE__); \
                        static_assert (basename_idx >= 0, "compile-time basename");   \
                        __FILE__ + basename_idx;})

#define MSG_DEBUG(msg) \
    micro_logger::__logme(LVL_DEBUG, __FILE_ONLY__, __FUNCTION__, __LINE__, msg)

#endif //MICRO_LOGGER_MICRO_LOGGER_H
