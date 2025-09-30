# How to build
Preferable building method for testing and development

```cmake . -Bout -GNinja -DWITH_MICRO_LOGGER_TESTS=ON -DCMAKE_BUILD_TYPE=Debug```

# How to test

### Notes
On some system adding ```LD_PRELOAD=$(gcc -print-file-name=libasan.so)``` might be required

### Unit testing
```ninja -C out/ && ctest --test-dir out --output-on-failure```

### Testing for performance changes
```ninja -C out/ && out/benchmark all```
```LD_PRELOAD=$(gcc -print-file-name=libasan.so) out/backport/demo_c --benchmark```

### Integration testing with python 
Caching issues with formatting or library
```LD_PRELOAD=$(gcc -print-file-name=libasan.so) python test_library.py```

# How to develope
@TODO

# How to use
CPP demo
```LD_PRELOAD=$(gcc -print-file-name=libasan.so) out/demo help```
###
C demo
```LD_PRELOAD=$(gcc -print-file-name=libasan.so) out/backport/demo_c --help```

# Performance changes:
Hardware: 9800X3D, 96GB 6400, Goodram IRPRO 4TB
|Function               |test description | C++17         |C++20          |%    |
|:----------------------|-----------------|--------------:|--------------:|:---:|
|bench_bytes_to_hex     |short data set   |18.29 MB/s     |No perf gain   |     |
|bench_bytes_to_hex     |long data set    |35045.72 MB/s  |No perf gain   |     |
|bench_hex_to_bytes     |short data set   |2.32 MB/s      |3.76 MB/S      |61%  |
|bench_hex_to_bytes     |long data set    |8.39 MB/s      |15.58 MB/s     |54%  |
|bench_bytes_to_integral|int32            |N/A            |230.77 MB/s    |     |
|bench_bytes_to_integral|int64            |N/A            |304.35 MB/s    |     |
|thread_info            |cache creation   |580ms          |No perf gain   |     |
|bench_logging_bandwidth|write one thread |66.15 MB/s     |               |     |
|bench_logging_bandwidth|write multithread|94.84 MB/s     |               |     |
|C bench logging        |single thread    |58.82 MB/s     |               |     |
|C bench logging        |multithread      |99.19 MB/s     |               |     |
