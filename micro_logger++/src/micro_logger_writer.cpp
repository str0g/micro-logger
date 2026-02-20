/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "micro_logger/micro_logger_writer.hpp"
//
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <errno.h>
#include <format>
#include <iostream>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>

namespace micro_logger {
size_t StandardOutWriter::write(const char *buf, size_t size) const {
  std::cout.write(buf, size);
  return size;
}

size_t SilentWriter::write(const char *data, size_t size) const { return 0; }

FileWriter::FileWriter(const char *path) : outfile(path) {
  if (not outfile.is_open()) {
    std::cerr << "failed to open file: " << path << std::endl;
    throw std::domain_error("open file");
  }
}

size_t FileWriter::write(const char *data, size_t size) const {
  outfile.write(data, size);
  return size;
}

FileWriter::~FileWriter() { outfile.close(); }

NetworkWriter *instance = nullptr;
std::mutex instance_mutex;
NetworkWriter::NetworkWriter(const std::string &address, int port)
    : address(address), port(port), sock(socket(AF_INET, SOCK_STREAM, 0)) {
  instance = this;
  reconnect();

  struct sigaction action = {};
  action.sa_handler = [](int) { // simple not beautiful solution to reconnect
    std::lock_guard<std::mutex> lock(instance_mutex);
    if (instance) {
      try {
        instance->reset_socket();
        instance->reconnect();
      } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
      }
    }
  };

  if (sigaction(SIGPIPE, &action, nullptr) == -1) {
    throw std::domain_error(std::format("{}", strerrordesc_np(errno)));
  }
}

void NetworkWriter::reconnect() {
  if (sock < 0) {
    throw std::domain_error(
        std::format("{} {}:{}", strerrordesc_np(errno), address, port));
  }

  sockaddr_in serv_addr = {};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr) <= 0) {
    throw std::domain_error(
        std::format("{} {}:{}", strerrordesc_np(errno), address, port));
  }

  if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr,
                           sizeof(serv_addr))) < 0) {
    throw std::domain_error(
        std::format("{} {}:{}", strerrordesc_np(errno), address, port));
  }
}

void NetworkWriter::reset_socket() {
  close(client_fd);
  sock = socket(AF_INET, SOCK_STREAM, 0);
}

size_t NetworkWriter::write(const char *data, size_t size) const {
  return send(sock, data, size, 0);
}

NetworkWriter::~NetworkWriter() {
  std::lock_guard<std::mutex> lock(instance_mutex);
  close(client_fd);
  instance = nullptr;
}

AsyncWriter::AsyncWriter(std::unique_ptr<BaseWriter> &output)
    : output(std::move(output)),
      thread(std::thread(&AsyncWriter::worker, this)), run(true) {}

size_t AsyncWriter::write(const char *buf, size_t size) const {
  {
    std::scoped_lock lock(sync);
    queue.emplace(buf, size);
  }
  cv.notify_all();
  return size;
}

AsyncWriter::~AsyncWriter() { stop(); }
void AsyncWriter::stop() {
  {
    std::scoped_lock lock(sync);
    run = false;
  }
  cv.notify_all();
  if (thread.joinable())
    thread.join();
}

void AsyncWriter::worker() {
  while (true) {
    std::unique_lock lock(sync);
    cv.wait(lock, [&]() { return not queue.empty() or not run; });
    if (not run and queue.empty()) {
      return;
    }
    auto data = std::move(queue.front());
    queue.pop();
    lock.unlock();
    output->write(data.data(), data.size());
  }
}
} // namespace micro_logger
