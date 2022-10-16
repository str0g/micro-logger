/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_WRITER_H
#define MICRO_LOGGER_MICRO_LOGGER_WRITER_H

#include <fstream>

namespace micro_logger {
    class BaseWriter {
        public:
            virtual size_t write(const char *buf, size_t) const = 0;
    };

    class StandardOutWriter : public BaseWriter {
        public:
            size_t write(const char *buf, size_t) const final;
    };

    class FileWriter : public BaseWriter {
        public:
            explicit FileWriter(const char* path);
            size_t write(const char *buf, size_t) const final;
            ~FileWriter();
        private:
            mutable std::ofstream outfile;
    };

    //read with netcat -l -p <port>
    class NetworkWriter : public BaseWriter {
        public:
            explicit NetworkWriter(const char* address, int port);
            size_t write(const char *buf, size_t) const final;
            NetworkWriter();
            ~NetworkWriter();
        private:
            int sock;
            int client_fd;
    };
}

#endif //MICRO_LOGGER_MICRO_LOGGER_WRITER_H
