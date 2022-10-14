//
// Created by lukasz on 13.10.2022.
//

#ifndef MICRO_LOGGER_MICRO_LOGGER_TOOLS_H
#define MICRO_LOGGER_MICRO_LOGGER_TOOLS_H

#include <vector>
#include <string>

namespace micro_logger {
/**
 * common function to dump object as hex
 * @param size
 * @return
 */
    std::string bytes_to_hex(const uint8_t *, size_t size);

/**
 * reinterpret hex to something else in further step
 * @param hex
 * @return
 */
    std::vector<uint8_t> hex_to_bytes(const std::string &hex);
}

#endif //MICRO_LOGGER_MICRO_LOGGER_TOOLS_H
