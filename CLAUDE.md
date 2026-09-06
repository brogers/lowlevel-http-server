# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

`myhttp` is a from-scratch HTTP server in C, written alongside the Low Level Academy
"Build Your Own HTTP Server" course. It is a work in progress: the code implements
the early lessons (TCP bind/accept, request-line parsing) and does not yet write
responses or route.

## Commands

The build is driven entirely by `CMakePresets.json`. The `justfile` is a thin
wrapper with no build logic of its own; use it or call CMake directly.

```sh
just build              # configure (if needed) + build      [preset=debug]
just test               # build + run the full CTest suite
just run                # build + run the server binary
just ci                 # configure -> build -> test workflow
just format             # clang-format src/ include/ tests/ (skips vendor/)
just clean / distclean  # drop build artifacts / drop build/ entirely
just preset=release build   # override the preset for any recipe
```

Presets: `debug` (default), `release`, `relwithdebinfo`, `asan` (Debug + ASan/UBSan).
Each configures into `build/<preset>/`. Equivalent raw commands:
`cmake --preset debug`, `cmake --build --preset debug`, `ctest --preset debug`.

### Running a single test

Test executables are registered with CTest under the same name as their source
(`test_tcp`, `test_http`) and land in `build/<preset>/bin/`.

```sh
ctest --preset debug -R test_http          # one test binary, via CTest
ctest --preset debug -R test_http -V       # + full Unity per-assertion output
./build/debug/bin/test_http                # run the binary directly
```

Unity has no built-in single-case filter; narrow further by editing the
`RUN_TEST(...)` lines in the test's `main()`.

### Sanitizers

`just preset=asan test` (or `ctest --preset asan`) builds and runs everything
under AddressSanitizer + UBSan.

## Architecture

**Library / executable split.** `src/CMakeLists.txt` builds everything except
`main()` into a static lib, `myhttp_lib` (alias `myhttp::lib`), so tests can link
the real code. `main.c` becomes the `myhttp` executable. `route.c` / `include/route.h`
are empty stubs and are **not** in the build yet.

**Flat headers.** All public headers live directly in `include/` (added as a
`PUBLIC` include dir on the lib), so sources use `#include <http.h>`, not relative
paths.

**Request parsing (`src/http.c`).** `read_http_request(int fd, http_request *out)`
reads raw bytes straight off a file descriptor and parses the request line by
mutating the buffer in place (writing NULs at the spaces). `http_request` is
fixed-size char buffers (`method[8]`, `path[256]`, `protocol[12]`) in
`include/http.h`. Tests exercise it by priming a `pipe()` with a raw request
string and passing the read end — see `tests/test_http.c`.

**TCP layer (`src/tcp.c`).** `bind_tcp_port(tcp_server *, uint16_t port)` and
`accept_client(int server_fd)`, returning a `server_status_e`. `tcp_server` holds
the socket fd and `sockaddr_in`. `test_tcp.c` binds port 0 (ephemeral) so the test
never collides with a real listener.

**Server loop (`src/main.c`).** Currently single-shot: bind, accept **one**
client, print its method/path/protocol, close, exit. No request loop, no response,
no keep-alive.

**`debug_log(msg)`** (`include/main.h`) is a macro that prints `[file] msg` to
stderr; use it for diagnostics rather than bare `printf`.

**Warnings.** `cmake/CompilerWarnings.cmake` defines the INTERFACE target
`myhttp::warnings` (`-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wundef
-Wdouble-promotion -Wstrict-prototypes`), linked PRIVATE into every first-party
target. Not `-Werror` by default; enable with `-DMYHTTP_WARNINGS_AS_ERRORS=ON`.

**Tests.** Unity (ThrowTheSwitch) is vendored at `tests/vendor/unity/` (v2.7.0) so
the build needs no network. It is compiled as a `unity` static lib with a `SYSTEM`
include dir (its headers would otherwise trip the warning flags). Add a test with
the `myhttp_add_unity_test(name sources...)` helper in `tests/CMakeLists.txt` — it
builds the exe, links `myhttp::lib unity::framework myhttp::warnings`, and calls
`add_test`. Every test file needs its own `setUp`/`tearDown` and a `main()` that
runs `UNITY_BEGIN()` / `RUN_TEST(...)` / `UNITY_END()`.

## Conventions

- Keep build logic in `CMakePresets.json` and the CMake listfiles — not in the
  `justfile` and not in shell scripts. New settings become presets or target
  properties.
- Machine-specific build tweaks go in `CMakeUserPresets.json` (gitignored), never
  in `CMakePresets.json`.
- C23 (`CMAKE_C_STANDARD 23`, no GNU extensions). Toolchain here is GCC 16.
- This tracks a course, so some signatures and structures are deliberately kept
  close to the course code even when a cleaner form exists.

## Known rough edges

- `src/main.c` binds port **8081**; the README says 8080.
- `test_http.c` has a failing case — `read_http_request` corrupts the buffer while
  advancing past the first space (`src/http.c:26`), so `request.path` comes out
  garbled.
- `htons(port)` in `src/tcp.c:15` emits `-Wconversion`; left as-is to match the
  course signature.
