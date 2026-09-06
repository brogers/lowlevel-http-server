# Thin task runner. All build logic lives in CMakePresets.json + CMakeLists.txt.
# Requires CMake >= 3.25 and Ninja.
#
# Pick a preset per invocation:  just preset=release build
#                          or:   PRESET=release just build

preset := env_var_or_default("PRESET", "debug")

# List available recipes
default:
    @just --list

# Configure (if needed) and build <preset>
build:
    #!/usr/bin/env bash
    set -euo pipefail
    [ -f "build/{{preset}}/CMakeCache.txt" ] || cmake --preset "{{preset}}"
    cmake --build --preset "{{preset}}"

# Build and run the test suite for <preset>
test: build
    ctest --preset "{{preset}}"

# Build and run the server
run: build
    ./build/{{preset}}/bin/myhttp

# Run the full configure -> build -> test workflow
ci:
    cmake --workflow --preset "{{preset}}"

# Format all C sources with clang-format
format:
    find src include tests -name '*.[ch]' -print0 | xargs -0 clang-format -i

# Remove build artifacts for <preset> (keep the CMake cache)
clean:
    cmake --build --preset "{{preset}}" --target clean

# Remove the entire build/ directory
distclean:
    rm -rf build
