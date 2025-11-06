# How to build
Preferable building method for testing and development

### release build
```make debug```

### debug build
```make debug```

# How to test

### Notes
On some system adding ```LD_PRELOAD=$(gcc -print-file-name=libasan.so)``` might be required

### Unit testing
```make test```

### Testing for performance changes
```build/<profile>/demos/benchmark all```

```LD_PRELOAD=$(gcc -print-file-name=libasan.so) build/<profile>/micro_logger/demos/demo_c --benchmark```

### Integration testing with python 
Caching issues with formatting or library

```LD_PRELOAD=$(gcc -print-file-name=libasan.so) python test_library.py```

# Build package for your distribution
Currently only archlinux is supported

```make pkg```

# How to develope
@TODO

# How to use
CPP demo

```LD_PRELOAD=$(gcc -print-file-name=libasan.so) build/<profile>/demos/demo help```

C demo

```LD_PRELOAD=$(gcc -print-file-name=libasan.so) build/<profile>/micro_logger/demos/demo_c --help```

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
|thread_info            |cache creation   |808ms          |No perf gain   |     |
|bench_logging_bandwidth|write one thread |77.90 MB/s     |               |     |
|bench_logging_bandwidth|write multithread|94.84 MB/s     |               |     |
|C bench logging        |single thread    |71.67 MB/s     |72.67MB/s      |0.14%|
|C bench logging        |multithread      |99.77 MB/s     |102.87MB/s     |3%   |
