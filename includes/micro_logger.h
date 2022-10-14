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
    void __logme(const char *level, const char *file, int line, const char *message);
}

#ifndef NODEBUG
#define LVL_TRACE       "TRACE"
#define LVL_DEBUG       "DEBUG"
#endif
#define LVL_INFO        "INFO "
#define LVL_WARN        "WARN "
#define LVL_ERROR       "ERROR"
#define LVL_CRITIACL    "CRITI"

#define MSG_DEBUG(msg) \
    micro_logger::__logme(LVL_DEBUG, __FILE__, __LINE__, msg)

#endif //MICRO_LOGGER_MICRO_LOGGER_H
