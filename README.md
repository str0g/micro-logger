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

### Integration testing with python 
Caching issues with formatting or library
```LD_PRELOAD=$(gcc -print-file-name=libasan.so) python test_library.py```

# How to develope
@TODO

# How to use
@TODO demo application section

# Performance changes:
|Function               |test description| C++17          |C++20           |
|:----------------------|----------------|---------------:|---------------:|
|bench_bytes_to_hex     | short data set |16.3043 MB/s    |N/A             |
|bench_bytes_to_hex     | long data set  |24133.7154 MB/s |N/A             |
|bench_hex_to_bytes     | short data set |2.3166 MB/s     |N/A             |
|bench_hex_to_bytes     | long data set  |8.3893 MB/s     |N/A             |
|bench_bytes_to_integral| int32          |N/A             |230.7692 MB/s   |
|bench_bytes_to_integral| int64          |N/A             |304.3478 MB/s   |
