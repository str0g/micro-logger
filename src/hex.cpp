//
// Created by lukasz on 13.10.2022.
//
#include "micro_logger_tools.h"
#include <ostream>
#include <iomanip>

namespace micro_logger {
    std::string bytes_to_hex(const uint8_t *bytes, size_t size) {
        std::ostringstream os;
        for (size_t i = 0; i < size; i++) {
            os << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(bytes[i]);
        }
        return os.str();
    }

    std::vector<uint8_t> hex_to_bytes(const std::string &hex) {
        std::vector<uint8_t> out;
        out.reserve(hex.length() / 2);
        for (size_t i = 0; i < hex.length(); i += 2) {
            auto bytes_str = hex.substr(i, 2);
            out.emplace_back(std::stoul(bytes_str, nullptr, 16));
        }

        return out;
    }
}