//
// Created by lukasz on 13.10.2022.
//

#ifndef MICRO_LOGGER_THREAD_INFO_H
#define MICRO_LOGGER_THREAD_INFO_H

#include <string>
#include <thread>
#include <unistd.h>

namespace micro_logger {
    class ThreadInfo {
        public:
            ThreadInfo();

            pid_t pid;
            std::thread::id tid;
            const std::string info;

        private:
            std::string formatter();
    };
}

#endif //MICRO_LOGGER_THREAD_INFO_H
