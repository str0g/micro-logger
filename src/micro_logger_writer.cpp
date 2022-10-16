//
// Created by lukasz on 14.10.2022.
//
#include "micro_logger_writer.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

namespace micro_logger {
    size_t StandardOutWriter::write(const char *buf, size_t size) const {
        std::cout.write(buf, size);
        return size;
    }

    FileWriter::FileWriter(const char* path) : outfile(path) {
        if (not outfile.is_open()) {
            std::cerr << "failed to open file: " << path << std::endl;
            throw std::domain_error("open file");
        }
    }

    size_t FileWriter::write(const char* data, size_t size) const {
        outfile.write(data, size);
        return size;
    }

    FileWriter::~FileWriter() {
        outfile.close();
    }

    NetworkWriter::NetworkWriter(const char* address, int port) : sock(socket(AF_INET, SOCK_STREAM, 0)) {
        char buf[128];
        if (sock < 0) {
            std::snprintf(buf, sizeof(buf), "%s %s:%d", strerrordesc_np(errno), address, port);
            throw std::domain_error(buf);
        }

        sockaddr_in serv_addr = {};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
            std::snprintf(buf, sizeof(buf), "%s %s:%d", strerrordesc_np(errno), address, port);
            throw std::domain_error(buf);
        }

        if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
            std::snprintf(buf, sizeof(buf), "%s %s:%d", strerrordesc_np(errno), address, port);
            throw std::domain_error(buf);
        }
    }
    size_t NetworkWriter::write(const char* data, size_t size) const {
        return send(sock, data, size, 0);
    }

    NetworkWriter::~NetworkWriter() {
        close(client_fd);
    }
}