/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef MICRO_LOGGER_MICRO_LOGGER_HPP
#define MICRO_LOGGER_MICRO_LOGGER_HPP

#include "micro_logger_custom_parameters.h"
#include "micro_logger_writer.hpp"
//
#include <sstream>

namespace micro_logger {

/**
 * @brief Default logging configuration.
 *
 * Configures the header format, alignment widths, timestamp layout, and
 * buffer sizes used by every logger instance.
 *
 * @headergroup micro_logger
 *
 * @remark header_size + message_size is kept below one memory page for
 *         performance reasons.
 */
constexpr micro_logger_CustomParameters default_parameters{
    .header_size = 128,
    .message_size = 1024,
    .header_pattern = "[%%s]%s[%%%ss:%%%sd::%%s][%%s]\n",
    .align_filename_length = "",
    .align_lines_length = "03",
    .time_format = "[%D %T",
    .milliseconds_format = ".%03ld]",
};

/**
 * @brief Initialise the library.
 *
 * Must be called once, before any logging macro or function, typically at
 * program startup.  It sets up the internal data structures that every
 * subsequent log call depends on.
 *
 * @param[in] writer             Sink to which log lines are written.  Obtain
 *                               one of the standard writers via the writer
 *                               classes in `micro_logger_writer.hpp` (e.g.
 *                               `StandardOutWriter`).
 * @param[in] custom_parameters  Optional override of the default parameters.
 *                               Pass `nullptr` to keep the defaults.
 */
void initialize(
    const BaseWriter &,
    const micro_logger_CustomParameters *custom_parameters = nullptr);

/**
 * @brief Convert any streamable object to std::string.
 *
 * Helper template used internally by the library, but also available to
 * callers who need a quick string conversion via operator<<.
 *
 * @tparam T  Type with a `std::ostream& operator<<` overload.
 * @param[in] obj  Object to stringify.
 * @return    std::string containing the textual representation.
 */
template <class T> std::string to_string(const T &obj) {
  std::stringstream os;
  os << obj;
  return os.str();
}

/**
 * @brief Core logging function.
 *
 * Low-level function that every `MSG_*` macro expands to.  Users should not
 * call this function directly — use the macros instead.
 *
 * @internal
 *
 * @param level   Log level string (e.g. `"INFO "`).
 * @param file    Source file name (shortened to basename).
 * @param func    Function name where the call occurred.
 * @param line    Source line number.
 * @param fmt     Format string (printf-style).
 * @param ...     Format arguments corresponding to @p fmt.
 */
void __logme(const char *level, const char *file, const char *func, int line,
             const char *fmt, ...);

/**
 * @brief Return the basename component of a path.
 *
 * @param path  Full path (e.g. `"/usr/src/main.cpp"`).
 * @return      Pointer to the character inside @p path that starts the
 *              basename (e.g. `"main.cpp"`).  The result points into the
 *              original string — do not free it.
 */
constexpr const char *basename(const char *const path) {
  int slash_index = 0;
  int index = 0;
  do {
    if (path[index++] == '/') {
      slash_index = index;
    }
  } while (path[index]);
  return &path[slash_index];
}

/** Log level identifiers — `TRACE` variant. */
constexpr const char *LVL_TRACE = "TRACE";
/** Log level identifiers — `DEBUG` variant. */
constexpr const char *LVL_DEBUG = "DEBUG";
/** Log level identifiers — `INFO` variant. */
constexpr const char *LVL_INFO = "INFO ";
/** Log level identifiers — `WARN` variant. */
constexpr const char *LVL_WARN = "WARN ";
/** Log level identifiers — `ERROR` variant. */
constexpr const char *LVL_ERROR = "ERROR";
/** Log level identifiers — `CRITICAL` variant. */
constexpr const char *LVL_CRITICAL = "CRITI";

} // namespace micro_logger

#ifndef USE_C_VERSION

#ifndef NODEBUG
#define MSG_DEBUG(fmt, ...)                                                    \
  micro_logger::__logme(micro_logger::LVL_DEBUG,                               \
                        micro_logger::basename(__FILE__), __FUNCTION__,        \
                        __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ENTER()                                                            \
  micro_logger::__logme(micro_logger::LVL_TRACE,                               \
                        micro_logger::basename(__FILE__), __FUNCTION__,        \
                        __LINE__, "%s", "--ENTER--")
#define MSG_EXIT()                                                             \
  micro_logger::__logme(micro_logger::LVL_TRACE,                               \
                        micro_logger::basename(__FILE__), __FUNCTION__,        \
                        __LINE__, "%s", "--EXIT--")
#else
#define MSG_DEBUG(fmt, ...)
#define MSG_ENTER()
#define MSG_EXIT()
#endif

#define MSG_INFO(fmt, ...)                                                     \
  micro_logger::__logme(micro_logger::LVL_INFO,                                \
                        micro_logger::basename(__FILE__), __FUNCTION__,        \
                        __LINE__, fmt, ##__VA_ARGS__)
#define MSG_WARN(fmt, ...)                                                     \
  micro_logger::__logme(micro_logger::LVL_WARN,                                \
                        micro_logger::basename(__FILE__), __FUNCTION__,        \
                        __LINE__, fmt, ##__VA_ARGS__)
#define MSG_ERROR(fmt, ...)                                                    \
  micro_logger::__logme(micro_logger::LVL_ERROR,                               \
                        micro_logger::basename(__FILE__), __FUNCTION__,        \
                        __LINE__, fmt, ##__VA_ARGS__)
#define MSG_CRITICAL(fmt, ...)                                                 \
  micro_logger::__logme(micro_logger::LVL_CRITICAL,                            \
                        micro_logger::basename(__FILE__), __FUNCTION__,        \
                        __LINE__, fmt, ##__VA_ARGS__)

#endif // USE_C_VERSION

#endif // MICRO_LOGGER_MICRO_LOGGER_HPP
