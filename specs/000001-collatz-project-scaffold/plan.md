# Execution Plan: Collatz Project Scaffold, Core Engine, CLI and wxWidgets Skeleton

## Objective
Initialize a self-contained, vcpkg-driven CMake C++ project with a tested 64-bit Collatz core, a CLI
front-end, and a minimal wxWidgets application shell, all building from one manifest.

## Status
All steps complete and verified. The four targets build and `ctest` passes; `collatz-gui` launches.
Two host packages (`autoconf-archive`, `bison`) were installed by the author to build the GUI deps.

## Proposed Changes
### Affected Files
- [x] `README.md`: replaced the one-line stub with build, run and test instructions.

### New Files to Create
- [x] `vcpkg.json`: manifest with `cli11` + `catch2` and a `gui` feature carrying `wxwidgets`.
- [x] `CMakeLists.txt`: top-level project, options, per-target wiring.
- [x] `CMakePresets.json`: `ninja` debug/release presets pointing at `$VCPKG_ROOT`.
- [x] `.gitignore`: ignores `build/`, `vcpkg_installed/`, and IDE artifacts.
- [x] `AGENTS.md`: short decision/action register (written at GATE 1).
- [x] `include/collatz/collatz.hpp`: public core API (sequence, statistics, overflow reporting).
- [x] `src/collatz.cpp`: core implementation with the 64-bit overflow guard.
- [x] `src/main_cli.cpp`: CLI11 front-end (positional args + stdin fallback).
- [x] `src/main_gui.cpp`: minimal wxWidgets frame, placeholder for the deferred interface.
- [x] `tests/test_collatz.cpp`: Catch2 tests for the core engine.
- [ ] `cmake/`: not needed; configuration stayed small.

## Step-by-Step Implementation Tasklist
Each step names the spec.md identifiers it delivers.

- [x] **Step 1: vcpkg manifest and CMake skeleton (R5)**
  - [x] Write `vcpkg.json` and a minimal `CMakeLists.txt`. `wxwidgets` sits behind a `gui`
        feature so the headless build never solves the GTK tree.
  - [x] Add `CMakePresets.json` with `VCPKG_ROOT` toolchain wiring and a `COLLATZ_BUILD_GUI` option.
  - [x] Add `.gitignore`; `cmake --preset debug -DCOLLATZ_BUILD_GUI=OFF` configures successfully.
- [x] **Step 2: Core engine (R1, R2, R3, EC1, EC2, EC3, EC6)**
  - [x] Define `include/collatz/collatz.hpp`: value type, sequence result, statistics, status enum.
  - [x] Implement sequence generation and statistics in `src/collatz.cpp` with docstrings and inline
        "why" comments.
  - [x] Implement the overflow guard and the maximum-step cap.
- [x] **Step 3: CLI front-end (R4, EC4, EC5)**
  - [x] Parse positional values with CLI11; fall back to whitespace/line-separated stdin.
  - [x] Print each sequence and its statistics; exit `1` for empty input, `2` for invalid tokens.
- [x] **Step 4: wxWidgets placeholder shell (R6, EC7)**
  - [x] Add a `collatz-gui` target gated by `COLLATZ_BUILD_GUI`, with a clear failure path.
  - [x] Implement a minimal frame that links against `collatz_core` (interface deferred).
  - [x] Verified: configures, builds, links and launches (window stays up until killed).
- [x] **Step 5: Tests and wiring (R7, AC1, AC2, AC3, AC4, AC5, AC6)**
  - [x] Add `tests/test_collatz.cpp`: sequences for `1` and `6`, statistics for `27`, overflow,
        truncation, validity, status tokens.
  - [x] Register the test target with CTest; `ctest --preset debug` passes 1/1.
  - [x] Update `README.md` with configure/build/run/test commands.

## Host packages (resolved)
The `wxwidgets` feature pulls 76 vcpkg ports built from source (GTK3, Cairo, Pango, X11/XCB).
Inspecting every port yields exactly two build-time host packages, both installed by the author:
`autoconf-archive` (the 20 `autoreconf` ports) and `bison` (the `gettext` port). No other host tool
is required: `nasm`, `perl`, `meson`, `ninja`, `pkgconf`, `clang` are auto-acquired by vcpkg,
`gperf` is built as a vcpkg port, and `gtk-doc`/`libltdl` are referenced by no port.

## Risk Mitigation & Edge Cases
- **Risk:** The vcpkg build compiles wxWidgets/GTK3 from source and is slow -> **Mitigation:** keep
  `COLLATZ_BUILD_GUI` toggleable so CLI/test work is never blocked by the heavy GUI dependency.
- **Risk:** `3n+1` silently wraps for large inputs -> **Mitigation:** check before the multiply-add
  and flag overflow (R3, EC3); covered by a regression test (AC4).
- **Risk:** A pathological input loops forever -> **Mitigation:** enforce the maximum-step cap and
  mark the result truncated (EC6).
- **Risk:** wxWidgets 3.3 breaks older `CMakeLists` conventions -> **Mitigation:** prefer the
  vcpkg-provided `wx::core`/`wx::base` targets, with a legacy `wxWidgets_LIBRARIES` fallback.
