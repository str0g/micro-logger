# Key Features
  - C++20 standard with high-performance design
  - Dual interface: C API (micro_logger.h) and C++ API (micro_logger.hpp)
  - Thread-safe multi-threaded logging
  - Customizable logging levels: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL
  - Configurable format with header patterns, timestamps, file/line/function info
  - Async writer support
  - Caching optimization for thread information
  - Benchmarked performance up to ~273 MB/s logging bandwidth

# Key difference C vs C++
 - C is lacking async writer support
 - C filenames are being resolved on runtime

# How to build
Preferable building method for testing, development, release

## List build profiles
```make list_presets```

## Release
```make release```

## Debug
```make debug```

## Package for your distribution
Currently only ArchLinux is supported

```make pkg```

# How to test
## all at once
```make test```

## unit tests
```cmake --build build/debug/ && ctest --test-dir build/debug/ --output-on-failure```

## Integration (testing with python)
```pytest tests/test_library.py```
```pytest tests/test_c_demo.py```

## Performance
C++ implementation
```build/<profile>/demos/benchmark all```

C wrapper over C++ implementation
```LD_PRELOAD=$(gcc -print-file-name=libasan.so) build/<profile>/micro_logger/demos/demo_c --benchmark```


# How to develop
## First thing to do
 - Before any changes execute regression tests.
## Choose scope of work
 - bug fix
 - improvement
 - feature
## Definition of done
 - No regression has been introduced
 - New functionality has been cover by unit/integration tests
 - Documentation has been updated


# How to use
## Demonstration
Follow demonstration implementation for C++
located in
```micro_logger++/demos/demo.cpp```
related binary
```LD_PRELOAD=$(gcc -print-file-name=libasan.so) build/<profile>/demos/demo help```

Follow demonstration implementation for C
located in
```micro_logger/demos/demo.c```
related binary
```LD_PRELOAD=$(gcc -print-file-name=libasan.so) build/<profile>/micro_logger/demos/demo_c --help```

## Integrate to project
### C++ API
- micro_logger.hpp - Contains logging functionality
- micro_logger_writer.hpp - Contains writer implementations
- micro_logger_tools.hpp - Utility functions
- micro_logger_custom_paramters.h - Custom parameters
### C++ library
- micro_logger++.so

### C API
- micro_logger.h - Contains logging functionality
- micro_logger_custom_paramters.h - Custom parameters
### C library
- micro_logger++.so (mandatory dependency for C implementation)
- micro_logger.so

## Troubleshooting
On some system compiling with MICRO_LOGGER_SANITIZER requires adding to environment variables ```LD_PRELOAD=$(gcc -print-file-name=libasan.so)```

# Performance changes:
Hardware: 9800X3D, 96GB 6400, Goodram IRPRO 4TB
Build info: debug profile

### Current performance:

|Function                     |test description |               |
|:----------------------------|-----------------|--------------:|
|bench_bytes_to_hex           |short data set   |18.29 MB/s     |
|bench_bytes_to_hex           |long data set    |35045.72 MB/s  |
|bench_hex_to_bytes           |short data set   |3.76 MB/s      |
|bench_hex_to_bytes           |long data set    |15.58 MB/s     |
|bench_bytes_to_integral      |int32            |230.77 MB/s    |
|bench_bytes_to_integral      |int64            |304.35 MB/s    |
|thread_info                  |cache creation   |998ms          |
|bench_logging_bandwidth      |write one thread |114.06 MB/s    |
|bench_logging_bandwidth      |write multithread|188.18 MB/s    |
|bench_logging_bandwidth_async|write one thread |68.04 MB/s     |
|bench_logging_bandwidth_async|write multithread|58.46 MB/s     |
|C bench logging              |single thread    |108.31 MB/s    |
|C bench logging              |multithread      |273.89 MB/s    |

### Historical data:

|Function               |test description | C++17         |C++20          |%    |
|:----------------------|-----------------|--------------:|--------------:|:---:|
|bench_bytes_to_hex     |short data set   |18.29 MB/s     |No perf gain   |     |
|bench_bytes_to_hex     |long data set    |35045.72 MB/s  |No perf gain   |     |
|bench_hex_to_bytes     |short data set   |2.32 MB/s      |3.76 MB/S      |61%  |
|bench_hex_to_bytes     |long data set    |8.39 MB/s      |15.58 MB/s     |54%  |
|bench_bytes_to_integral|int32            |N/A            |230.77 MB/s    |     |
|bench_bytes_to_integral|int64            |N/A            |304.35 MB/s    |     |
|thread_info            |cache creation   |808ms          |No perf gain   |     |
|bench_logging_bandwidth|write one thread |77.90 MB/s     |               |     |
|bench_logging_bandwidth|write multithread|94.84 MB/s     |               |     |
|C bench logging        |single thread    |71.67 MB/s     |72.67MB/s      |0.14%|
|C bench logging        |multithread      |99.77 MB/s     |102.87MB/s     |3%   |
