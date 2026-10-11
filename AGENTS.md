# AGENTS.md

Local Spec-Driven Development register for this repository. Read it before writing code.

## What this project is
Self-contained C++ application that prints Collatz sequences for a series of input numbers.
Nothing outside this repository is referenced or required.

## Hard rules
- Never edit production code before `specs/<NNNNNN>-<name>/spec.md` and `plan.md` exist and GATE 2 is passed.
- Keep state on disk: tick `plan.md` checkboxes as tasks complete; do not track it only in chat.
- New code follows documentation-standards: language-native docstrings plus short "why" inline comments.
- `spec.md` + `plan.md` combined must stay <= 36 KB; this file must stay <= 10 KB.
- Engine stays 64-bit (`std::uint64_t`) with an explicit overflow guard; no arbitrary precision.

## Fixed decisions (approved 2026-10-11)
- C++20; CMake + Ninja in vcpkg manifest mode.
- Dependencies in `vcpkg.json`: `wxwidgets`, `catch2`, `cli11`.
- Deliverables: `collatz_core` library, `collatz` CLI, `collatz-gui` shell, `collatz_tests`.
- Option `COLLATZ_BUILD_GUI` defaults ON; `COLLATZ_BUILD_TESTS` defaults ON.
- wxWidgets detailed interface is deferred: only a buildable placeholder window exists for now.

## Commands
- Configure: `cmake --preset debug`
- Build:     `cmake --build --preset debug`
- Test:      `ctest --preset debug`
- Run CLI:   `./build/debug/collatz 6 27`
- Headless:  `cmake --preset debug -DCOLLATZ_BUILD_GUI=OFF`

## Current state
Feature `000001-collatz-project-scaffold` implemented and verified. All four targets build;
`ctest` passes; `collatz-gui` launches. The GUI interface itself is still deferred to a later spec.
Host prerequisite for the GUI path: `autoconf-archive` and `bison` must be installed.
