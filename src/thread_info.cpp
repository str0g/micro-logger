//
// Created by lukasz on 13.10.2022.
//

#include "thread_info.h"
#include "micro_logger_tools.h"

namespace micro_logger {
    ThreadInfo::ThreadInfo() : tid(std::this_thread::get_id()), pid(getpid()), info(formatter()) {
    }

    std::string ThreadInfo::formatter() {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "[pid:%08s][tid:%016s]",
                      micro_logger::bytes_to_hex(reinterpret_cast<const uint8_t*>(&pid), sizeof(pid)).c_str(),
                      micro_logger::bytes_to_hex(reinterpret_cast<const uint8_t*>(&tid), sizeof(tid)).c_str());
        return buf;
    }
}