# Tensorizer 0.1

Tensorizer is a library that allows you to load tensors from backing cloud storage (s3) without an intermediate hop to any type of posix filesystem. This is useful because often cloud hosts have much faster access to S3 than they do to their file volumes such as EBS. As an example a p3dn.24xlarge instance can read at 100 Gbps when reading from S3, but only at 14 Gbps when reading from EBS. For comparison a Macbook Air, with a relatively speedy SSD gets about 10 Gbps for sequential reads.

Is this fast enough to load tensors while training a fancy Computer Vision or NLP model? I don't know, but I hope it is. The traditional way of loading large datasets is to store them in a compressed format in network storage, copy them to a local disk, decompress them, and then run a number of data transformations before they are eventually in-memory as tensors. It would simplify the process if you could do all of your required data transforms ahead of time, and store your tensors in S3 in a format that's ready to be used directly in training. Whether this is the case or not for you likely depends on your use-case, and the amount of pre-processing transformations you run.

## Features

* Tensor Preloading - Tensorizer lets users specify which tensors should be loaded in which order. Behind the scenes Tensorizer will preemptively download a fixed number of tensors, so that when your program is ready to use them they're instantly available.
* NDArray Handles - Tensorizer also uses NDArray handle references to pass data. That means that when you use the data, you're simply copying a reference. You don't do any serialization or deserialization work, and you don't even need to copy memory to the GPU.
* A High-Level Python API - Although Tensorizer makes heavy use of threads, and memory references, traditionally the domain of low-level CPU code, it will work just as well, and is in fact intended to be used from Python with simple sequential calls.
* A Stable Well-Supported Library - Just kidding, this is a side project that is highly likely to break if you try to use it.

## Roadmap

* Support for simple lossless compression of tensors (LZ4).
* RecordIO support
* Python Interface

## Benchmarks
TODO

## Q&A
* Why implement the backend in C++, why not Python?
- Python is a uniquely difficult language to write DataLoaders in. DataLoaders require a lot of parallel, non-blocking IO, which in Python often means you're using several different processes. This approach works in general, however you eventually need to use the Data you've loaded, and in Python this means you have to implicitly serialize the data in your DataLoader process, and then deserialize it in your main process, which is very expensive.
