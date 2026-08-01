# myhttp

A minimal HTTP server written in C from scratch, built as part of
Low Level Academy [Build Your Own HTTP Server](https://lowlevel.academy/courses/http).

## Description

Learn the fundamental protocol that powers the web. Write a server in C
that parses HTTP requests, serves HTTP responses, and learn how to write
defensive code.

## Requirements

- CMake >= 3.27
- GCC
- `ctest` (bundled with CMake) for running tests
- `libfmt`

## Build

```sh
make build
```

This configures and builds the project into `build/` via CMake.

## Run

```sh
make run
```

Starts the server, which listens on port `8080`.

## Test

```sh
make test
```

## Clean

```sh
make clean
```

## Project layout

```sh
include/         Public headers (main.h, tcp.h)
src/             Implementation (main.c, tcp.c)
CMakeLists.txt   CMake build configuration
Makefile         Convenience wrapper around CMake (build/run/test/clean)
```
