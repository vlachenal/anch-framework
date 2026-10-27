# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

AnCH Framework ("Another C++ Hack") is a set of small, independent C++ utility libraries (CLI parsing, configuration, logging, events, JSON, SQL, crypto, UUID, network, dates, ...). It is also a playground for new C++ standards: the code targets **C++23 and requires GCC >= 13**. Only POSIX systems are supported (`ANCH_POSIX` is defined; `ANCH_WINDOWS` paths exist but are not maintained). License: LGPLv3. Each source file starts with the LGPL header.

## Build

Each `anch-<module>/` directory is a standalone CMake project with its own `build/` directory (git-ignored). The usual workflow is per-module, out-of-source inside `build/`:

```sh
cd anch-conf/build
cmake ..
make
ctest                        # run all of the module's tests
ctest -R anch-conf-pattern   # run a subset by CTest name
```

There is also a root `CMakeLists.txt` that does `add_subdirectory()` on most modules (build in `./build`), but some modules are not listed there (e.g. `anch-ini`, `anch-app`, `anch-csv`), and `anch-rest` is commented out.

Things to know about the build:

- **Module settings live in `<Module>.cmake.in`** (e.g. `anch-conf/AnchConf.cmake.in`), `include()`d by the module's `CMakeLists.txt`. They define optimization flags, install dirs, static-lib switch (`ANCH_<MOD>_STATIC_LIB`), unit-test switch (`ANCH_<MOD>_UNITARY_TEST`, ON by default) and dependency paths.
- **Inter-module dependencies are resolved by hard-coded paths**: `$ENV{HOME}/projects/anch-framework/<dep>/inc` and `<dep>/build/lib`. A module therefore links against the dependency's already-built `build/lib/*.so`, so dependencies must be built first (e.g. `anch-conf` needs `anch-utils`, `anch-ini`, `anch-events`, and for tests `anch-cli` + `anch-ut`). `Anch.cmake.in` at the root is a newer, centralized version of these settings using `${PROJECT_SOURCE_DIR}`.
- Warnings are errors: `-Wall -Wextra -Werror -Wconversion -Wsign-conversion ...` plus hardening flags (`-fstack-protector-all`, `-D_FORTIFY_SOURCE=2`, `-ftrapv`). Implicit narrowing/sign conversions will break the build.
- Sources are picked up with `GLOB_RECURSE src/*`; re-run `cmake` after adding a file. Each library is built as an OBJECT library, then a shared lib (and optionally a static one).
- `anch-sql` has optional backends toggled by `ANCH_SQL_MYSQL`, `ANCH_SQL_POSTGRESQL`, `ANCH_SQL_SQLITE3`.

## Tests (anch-ut)

Tests use the framework's own unit test library, `anch-ut` (linked as `anch-unit`). `anch-ut` provides `main()`; a test file does not define `main` but implements `anch::ut::setup`:

```cpp
#include "ut/assert.hpp"
#include "ut/unit.hpp"

void anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("...")
    .description("...")
    .initialize(startup)       // also: uninitialize, beforeTest, afterTest
    .add("single", parseSingle);
}
```

Assertions: `anch::ut::assertTrue(cond, msg)` and `anch::ut::assert(cond, "fmt {} {}", args...)` (std::format-style).

Each test executable is registered in the module `CMakeLists.txt` once per test case with `add_test(<ctest-name> ${EXECUTABLE_OUTPUT_PATH}/<exe> -t <test-name>)`. When adding a test case, add both the `.add(...)` registration and a matching `add_test` line. Executables can be run directly from `build/bin/`:

```sh
./bin/anch-conf-placeholders-test -l          # list tests
./bin/anch-conf-placeholders-test -t single   # run one test (repeatable; default 'all')
```

Test fixture files (`.ini`, `.conf`, ...) in `test/` are copied into the build dir with `configure_file(... COPYONLY)`, so tests must run from `build/`.

## Code layout and conventions

- Per module: public headers in `inc/<name>/` (included as `"conf/configuration.hpp"`, `"ut/unit.hpp"`, `"cli/args.hpp"`, ...), sources in `src/<name>/`, tests in `test/`. Namespace is `anch::<name>` (e.g. `anch::conf`, `anch::ut`, `anch::cli`).
- Inline/template implementations go in `inc/<name>/impl/<file>.hpp`, `#include`d at the **end** of the matching public header. Keep declarations (with Doxygen comments) in the public header and definitions in `impl/`.
- Some modules are header-only (e.g. `anch-utils`, `anch-events`: no `src/`).
- Headers use `#pragma once`. Classes are sectioned with paired markers like `// Attributes +` / `// Attributes -`, `// Constructors +` / `// Constructors -`, `// Methods +` / `// Methods -`; CMake files use the same `+`/`-` block markers. Members are prefixed with `_`. Return type goes on its own line before function definitions.
- Everything public is documented with Doxygen (`/*! ... */`, `\brief`, `\param`, `\return`, `\author`, `\since`). Each module has an `anch-<name>.dox` page; generated HTML is committed under `docs/` (published at https://vlachenal.github.io/anch-framework/) from the root `Doxyfile`.
- `*.todo` files (e.g. `anch-conf/cascade.todo`, `anch-app/anch-app.todo`) hold design notes, partly in French, for work in progress.
