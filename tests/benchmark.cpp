#include "micro_logger.hpp"
#include "micro_logger_tools.hpp"
#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std::literals;

size_t to_kilobytes(size_t bytes) { return bytes / (1024); }

size_t to_megabytes(size_t bytes) { return bytes / (1024 * 1024); }

double to_bandwidth_mb_s(size_t bytes, long time) {
  return static_cast<double>(to_megabytes(bytes)) /
         (static_cast<double>(time) / 1000);
}

template <typename T>
auto bench(T obj, std::string_view func, std::string_view desc,
           size_t data_set_size) {
  auto start = std::chrono::steady_clock::now();
  obj();
  auto stop = std::chrono::steady_clock::now();
  auto exec_time_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)
          .count();

  auto out = std::format("[{}] {} took {}ms, size: {} bandwidth: {:.2f} MB/s",
                         func, desc, exec_time_ms, data_set_size,
                         to_bandwidth_mb_s(data_set_size, exec_time_ms));
  std::cout << out << std::endl;
  return exec_time_ms;
}

void bench_bytes_to_hex() {

  {
    size_t data_set_size = 1000000;
    auto exec_time = bench(
        [&]() {
          for (int i = 0; i < data_set_size; ++i) {
            volatile auto ignore = micro_logger::bytes_to_hex(
                reinterpret_cast<const uint8_t *>(&i), sizeof(i));
          }
        },
        __func__, {"for short data set"}, sizeof(int) * data_set_size);
  }
  {
    size_t data_set_size = 0;
    std::vector<std::string> data_set_str;
    for (int i = 0; i < 2000; ++i) {
      std::string out;
      for (int multiplay = 0; multiplay <= i; ++multiplay) {
        out += std::format("lorem ipsum {} {}", i, multiplay);
        data_set_size += out.size();
      }
      data_set_str.emplace_back(out);
    }
    auto exec_time = bench(
        [&]() {
          for (auto &data : data_set_str) {
            volatile auto ignore = micro_logger::bytes_to_hex(
                reinterpret_cast<const uint8_t *>(data.data()), data.size());
          }
        },
        __func__, {"for long data set"}, data_set_size);
  }
}

void bench_hex_to_bytes() {
  {
    size_t data_set_size = 1000000;
    std::vector<std::string> data_set;
    data_set.resize(data_set_size);
    for (size_t i = 0; i < data_set_size; ++i) {
      // int reprezentation
      data_set.emplace_back(
          std::format("0{}7{}a{}b{}", i % 2, i % 3, i % 5, i % 7));
    }
    auto exec_time = bench(
        [&]() {
          for (auto &data : data_set) {
            volatile auto ignore = micro_logger::hex_to_bytes(data);
          }
        },
        __func__, {"for short data set"}, sizeof(int) * data_set_size);
  }
  {

    size_t data_set_size = 0;
    std::vector<std::string> data_set;
    data_set.resize(2000);
    for (size_t i = 0; i < 2000; ++i) {
      // int reprezentation
      std::string out;
      for (size_t multiplay = 0; multiplay <= i; ++multiplay) {
        out += std::format("0{}7{}a{}b{}", i % 2, i % 3, i % 5, i % 7,
                           multiplay % 11);
      }
      data_set_size += out.length();
      data_set.emplace_back(out);
    }
    auto exec_time = bench(
        [&]() {
          for (auto &data : data_set) {
            volatile auto ignore = micro_logger::hex_to_bytes(data);
          }
        },
        __func__, {"for long data set"}, data_set_size);
  }
}

void bench_bytes_to_integral() {
  {
    size_t data_set_size = 1000000;
    std::vector<std::vector<uint8_t>> data_set;
    data_set.resize(data_set_size);
    for (size_t i = 0; i < data_set_size; ++i) {
      // int reprezentation
      data_set[i].emplace_back(static_cast<uint8_t>(i % 2));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 3));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 5));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 7));
    }
    auto exec_time = bench(
        [&]() {
          for (auto &data : data_set) {
            volatile auto ignore = micro_logger::bytes_to_integral<int>(data);
          }
        },
        __func__, {"for int32 data set"}, sizeof(int) * data_set_size);
  }
  {
    size_t data_set_size = 1000000;
    std::vector<std::vector<uint8_t>> data_set;
    data_set.resize(data_set_size);
    for (size_t i = 0; i < data_set_size; ++i) {
      // long reprezentation
      data_set[i].emplace_back(static_cast<uint8_t>(i % 2));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 3));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 5));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 7));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 11));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 13));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 17));
      data_set[i].emplace_back(static_cast<uint8_t>(i % 19));
    }
    auto exec_time = bench(
        [&]() {
          for (auto &data : data_set) {
            volatile auto ignore = micro_logger::bytes_to_integral<long>(data);
          }
        },
        __func__, {"for int64 data set"}, sizeof(long) * data_set_size);
  }
}

void bench_thread_local_cache() {
  {
    std::shared_ptr<micro_logger::BaseWriter> writer =
        std::make_shared<micro_logger::SilentWriter>();
    set_writer(*writer);
    size_t data_set_size = 10000;

    auto exec_time = bench(
        [&]() {
          std::vector<std::thread> data_set;
          for (size_t i = 0; i < data_set_size; ++i)
            data_set.emplace_back([]() { MSG_INFO(""); });
          for (auto &th : data_set)
            th.join();
        },
        __func__, {"cache creation time"}, 1);
  }
}

void bench_logging_bandwidth() {
  // data to write is 907bytes per one attempt
  std::shared_ptr<micro_logger::BaseWriter> writer =
      std::make_shared<micro_logger::FileWriter>("/dev/null");
  set_writer(*writer);
  {
    size_t data_set_size = 50000;

    auto exec_time = bench(
        [&]() {
          for (size_t i = 0; i < data_set_size; ++i) {
            MSG_ENTER();
            MSG_DEBUG("this logging [%s] is specialy crafted to write numbers: "
                      "[%08ld] and [pi]:%08f",
                      "message", i, (3.14 * i));
            MSG_INFO("my super shot logging message");
            MSG_WARN("This is warninig message");
            MSG_ERROR("Sometimes errors happens");
            MSG_CRITICAL("Critiacal errors happens");
            MSG_EXIT();
          }
        },
        __func__, {"write one thread"}, 907 * data_set_size);
  }
  {
    size_t data_set_size = 100;

    auto exec_time = bench(
        [&]() {
          std::vector<std::thread> data_set;
          for (size_t i = 0; i < data_set_size; ++i)
            data_set.emplace_back([i, data_set_size]() {
              for (size_t i = 0; i < data_set_size*10; ++i) {
                MSG_ENTER();
                MSG_DEBUG(
                    "this logging [%s] is specialy crafted to write numbers: "
                    "[%ld] and [pi]:%f",
                    "message", i, (3.14 * i));
                MSG_INFO("my super shot logging message");
                MSG_WARN("This is warninig message");
                MSG_ERROR("Sometimes errors happens");
                MSG_CRITICAL("Critiacal errors happens");
                MSG_EXIT();
              }
            });
          for (auto &th : data_set)
            th.join();
        },
        __func__, {"write multithread"}, 907 * data_set_size * (data_set_size *10));
  }
}

int main(int argc, char **argv) {
  std::unordered_map<std::string, void (*)()> options{
      {"bytes_to_hex", bench_bytes_to_hex},
      {"hex_to_bytes", bench_hex_to_bytes},
      {"bytes_to_integral", bench_bytes_to_integral},
      {"thread_local_cache", bench_thread_local_cache},
      {"logging_bandwidth", bench_logging_bandwidth},
  };
  for (int i = 1; i < argc; i++) {
    try {
      if (argv[i] == "all"s) {
        for (auto &o : options) {
          o.second();
        }
        continue;
      }
      options.at(argv[i])();
    } catch (const std::out_of_range &e) {
    }
  }
  return 0;
}
