# Specification: Collatz Project Scaffold, Core Engine, CLI and wxWidgets Skeleton

Feature ID: 000001-collatz-project-scaffold
Project: collatz-vibecoding-melhorado
Date: 2026-10-11

## Summary
This project is a self-contained C++ application that presents Collatz sequences for a series of
input numbers. Today it is an empty repository with only a stub `README.md`; nobody can build,
compute or test anything. This change initializes a vcpkg-based CMake project with a reusable core
engine, a command-line front-end, a Catch2 test target, and a minimal wxWidgets application shell
whose detailed interface is specified in a later iteration.

## Scope
In scope:
- A vcpkg manifest that pins the third-party dependencies and a CMake build that consumes it.
- A reusable `collatz_core` library: 64-bit sequence generation, statistics and an overflow guard.
- A command-line front-end that reads a series of starting values and prints each sequence and its
  statistics.
- A Catch2 unit-test target covering the core engine.
- A wxWidgets GUI target that compiles, links and launches a minimal placeholder window.
- Repository hygiene: `.gitignore`, `CMakePresets.json`, project `AGENTS.md`.

Out of scope:
- The detailed wxWidgets layout, controls and interactions: deferred to a follow-up specification
  (`Blocking on a later spec`, not on this one).
- Arbitrary-precision arithmetic (the engine is intentionally 64-bit only).
- Persistence, networking, databases, file formats and inter-service interfaces.
- Package/release packaging (installers, `.deb`, app bundles).

## Requirements
- R1: The engine generates the Collatz sequence for any start value `n >= 1` up to and including the
  first `1`, using the rules `n -> n/2` when even and `n -> 3n+1` when odd.
- R2: The engine computes statistics for a start value: number of steps to reach `1`, the maximum
  value reached, and the total stopping time.
- R3: The engine detects unsigned 64-bit overflow (a `3n+1` result exceeding `UINT64_MAX`), stops
  the sequence, and reports the condition instead of wrapping around.
- R4: A command-line front-end accepts a series of starting values as positional arguments and, when
  none are given, reads whitespace- or line-separated values from standard input; for each value it
  prints the full sequence and the statistics.
- R5: The project builds from a vcpkg manifest using CMake, producing the core library, the CLI
  executable, the GUI executable, and the test executable.
- R6: A wxWidgets executable target compiles, links and launches a window that is a placeholder for
  the interface to be specified later.
- R7: A Catch2 test target exercises the core engine and runs without any GUI, network or external
  service.

## Edge Cases and Failure Modes
- EC1: Start value `1` -> the sequence is `[1]` with `0` steps and a total stopping time of `0`.
- EC2: Start value `0` or a negative value -> rejected as an error; the Collatz map is defined only
  for positive integers.
- EC3: A sequence that would exceed `UINT64_MAX` -> generation stops at the last representable
  value, the overflow flag is set, and all values found so far are still reported (R3).
- EC4: No input at all (no arguments and empty standard input) -> a usage/diagnostic message is
  printed to standard error and the process exits with a non-zero status.
- EC5: A token that is not a valid positive integer -> a diagnostic naming the offending token is
  printed to standard error, that token is skipped, remaining valid tokens are processed, and the
  process exits with a non-zero status if any token was invalid.
- EC6: A pathologically long sequence -> generation is capped by a configurable maximum step count,
  marks the result as truncated instead of looping forever, and reports the truncation.
- EC7: The GUI target is built on a host without GUI libraries -> configuration must fail with a
  clear message rather than producing a broken or unbuildable target.

## Interface and Data Impact
- API: New public C++ library `collatz_core` (headers under `include/collatz/`). No network API.
- Database: None. The project stores no persistent state.
- Inter-service: None. The project is a single, self-contained process suite.
- CLI: New executable `collatz` (name reserved) with positional arguments and standard-input
  fallback; exit code `0` on success, non-zero on invalid input or empty input.
- GUI: New executable `collatz-gui` (name reserved) linking wxWidgets; interface deferred.

## Success Criteria
- AC1: `cmake --build` on a configured vcpkg preset produces `collatz`, `collatz-gui`,
  `collatz_core`, and the test binary without errors (proves R5, R6).
- AC2: Running the CLI on the series `6 27` prints, for `6`, `6 3 10 5 16 8 4 2 1` and for `27`,
  a sequence of 111 steps ending in `1` (proves R1, R4).
- AC3: For `27` the reported maximum value is `9232` and the step count is `111` (proves R2).
- AC4: For a crafted start value whose intermediate `3n+1` exceeds `UINT64_MAX`, the engine reports
  overflow and returns the values found so far rather than a wrapped number (proves R3, EC3).
- AC5: The Catch2 suite passes with `0` failures and needs no GUI, network or database (proves R7).
- AC6: Running the CLI with no input exits non-zero and prints a diagnostic (proves EC4).

## Cross-references
- None. This is a single-repository change.

## Open Questions
- Detailed wxWidgets interface (layout, controls, interactions) -> Blocking on a later specification;
  not blocking this one, which only scaffolds a buildable placeholder window.
- Whether the GUI target is built by default given the cost of compiling wxWidgets/GTK through vcpkg
  -> Recommendation: add a `COLLATZ_BUILD_GUI` option defaulting to `ON` for this environment, so
  the integration is proven end to end; the user may confirm or flip the default at GATE 1.
- Session context baseline -> `B = 48,000 bytes`, source: measured sizes of the two loaded skill
  files (`wc -c`, 31,078 B total) plus reconnaissance tool output and harness prompt.
