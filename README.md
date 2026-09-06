# myhttp

A minimal HTTP server written in C from scratch, built as part of
Low Level Academy [Build Your Own HTTP Server](https://lowlevel.academy/courses/http).

## Description

Learn the fundamental protocol that powers the web. Write a server in C
that parses HTTP requests, serves HTTP responses, and learn how to write
defensive code.

## Requirements

- CMake >= 3.25
- Ninja
- A C23 compiler (GCC >= 14 or Clang >= 18)
- `ctest` (bundled with CMake) for running tests
- [`just`][just-link] (optional) for the task shortcuts &mdash; `pacman -S just`

## Build

The build is driven entirely by `CMakePresets.json`. Use CMake directly:

```sh
cmake --preset debug          # configure
cmake --build --preset debug  # build
ctest --preset debug          # test
cmake --workflow --preset debug   # all three in one step
```

Available presets: `debug`, `release`, `relwithdebinfo`, `asan`
(Debug + Address/UB sanitizers). Each configures into `build/<preset>/`.

### `just` shortcuts

A thin `justfile` forwards to the preset commands and adds a couple of
conveniences. It has no build logic of its own.

```sh
just                    # list recipes
just build              # configure (if needed) + build   [preset=debug]
just test               # build + run tests
just run                # build + start the server
just ci                 # full configure -> build -> test workflow
just format             # clang-format all sources
just clean / distclean  # drop artifacts / drop build/
just preset=release build
```

## Run

```sh
just run
```

Starts the server, which listens on port `8080`.

## Project layout

```
CMakeLists.txt        Top-level: project(), options, subdirectories
CMakePresets.json     The build interface (configure/build/test/workflow)
justfile              Thin task-runner shortcuts around the presets
cmake/                Reusable CMake modules (CompilerWarnings.cmake)
include/              Public headers (main.h, tcp.h)
src/                  Library + executable targets (tcp.c -> myhttp_lib, main.c -> myhttp)
tests/                CTest-registered unit tests
.clangd               Points clangd at build/debug/compile_commands.json
```

Personal, machine-specific build tweaks go in `CMakeUserPresets.json`
(gitignored), not in `CMakePresets.json`.

[just-link]:<https://github.com/casey/just>
