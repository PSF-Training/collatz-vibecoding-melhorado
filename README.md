# collatz-vibecoding-melhorado

Self-contained C++ application that presents **Collatz sequences** for a series of input numbers.

It ships four artifacts built from one vcpkg manifest:

| Target | Kind | Purpose |
|---|---|---|
| `collatz_core` | library | 64-bit Collatz engine with an overflow guard |
| `collatz` | executable | Command-line front-end that prints sequences and statistics |
| `collatz-gui` | executable | Minimal wxWidgets window (detailed interface deferred) |
| `collatz_tests` | executable | Catch2 unit tests for the core engine |

## Requirements

- A C++20 compiler (GCC 11+ or Clang 14+)
- CMake 3.21+ and Ninja
- [vcpkg](https://github.com/microsoft/vcpkg) with `VCPKG_ROOT` set to its checkout

Dependencies (`wxwidgets`, `catch2`, `cli11`) are resolved automatically by vcpkg in
manifest mode; there is nothing to install by hand.

## Configure, build and test

```bash
export VCPKG_ROOT=/path/to/vcpkg

cmake --preset debug        # first run installs the vcpkg dependencies
cmake --build --preset debug
ctest --preset debug
```

Swap `debug` for `release` for an optimised build in `build/release/`.

The first configure is slow because vcpkg compiles wxWidgets and GTK from source.
To skip the GUI while working on the core, configure headless:

```bash
cmake --preset debug -DCOLLATZ_BUILD_GUI=OFF
cmake --build --preset debug
```

## Command line

```bash
./build/debug/collatz 6 27       # values as arguments
echo "6 27" | ./build/debug/collatz   # values on standard input
```

For each value the program prints a metrics line and the full orbit:

```
27: steps=111 max=9232 stopping_time=96
27 82 41 124 ... 2 1
```

Exit codes: `0` success, `1` no input, `2` at least one invalid token.

## GUI

```bash
./build/debug/collatz-gui
```

The current window is a placeholder: type numbers, press **Generate**, read the sequences.
The detailed interface will be specified separately.

## Layout

```
include/collatz/collatz.hpp   Public engine API
src/collatz.cpp               Engine implementation (overflow guard, step cap)
src/main_cli.cpp              CLI11 front-end
src/main_gui.cpp              wxWidgets placeholder shell
tests/test_collatz.cpp        Catch2 tests
specs/                        Specification and execution plan for each feature
```
