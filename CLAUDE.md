# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

AnCH Framework ("Another C++ Hack") is a set of small, independent C++ utility libraries (CLI parsing, configuration, logging, events, JSON, SQL, crypto, UUID, network, dates, ...). It is also a playground for new C++ standards: the code targets **C++23 and requires GCC >= 13**. Only POSIX systems are supported (`ANCH_POSIX` is defined; `ANCH_WINDOWS` paths exist but are not maintained). License: LGPLv3. Each source file starts with the LGPL header.

## Build

The framework is a single CMake project (CMake >= 3.25): the root `CMakeLists.txt` adds every module with `add_subdirectory()` in dependency order, and modules depend on each other through targets (`anch::utils`, `anch::conf`, `anch::log`, ...). Presets are defined in `CMakePresets.json` (`release`, `debug`, `no-db`; build dirs in `build/<preset>`):

```sh
cmake --preset debug                        # configure
cmake --build --preset debug                # build everything
cmake --build --preset debug --target anch-conf-parse   # build one target
ctest --preset debug                        # run all tests
ctest --preset debug -L conf                # run one module's tests (label = module name without 'anch-')
ctest --preset debug -R anch-conf-pattern   # run tests by name
```

Things to know about the build:

- **Helpers are in `cmake/AnchHelpers.cmake`**: `anch_add_library()` declares a module library (target `anch-<name>`, alias `anch::<name>`, include dir `inc/`, sources `src/*.cpp`, install rules), `anch_add_executable()` declares test/utility executables, `anch_copy_test_files()` copies fixtures. Each module's `CMakeLists.txt` only lists its public/private dependencies and definitions; tests live in `<module>/test/CMakeLists.txt`.
- **Options** are declared once in the root `CMakeLists.txt`: `ANCH_BUILD_TESTS`, `BUILD_SHARED_LIBS`, `ANCH_WARNINGS_AS_ERRORS`, `ANCH_HARDENING`, `ANCH_NATIVE_ARCH`, optional modules (`ANCH_BUILD_CRYPTO` and `ANCH_BUILD_SQL` ON; `ANCH_BUILD_REST` and `ANCH_BUILD_APP` OFF: anch-rest does not compile currently), features (`ANCH_UUID`, `ANCH_DATE`, `ANCH_CPU_AES`, `ANCH_CPU_DETECTION`, `ANCH_SQL_MYSQL`, `ANCH_SQL_POSTGRESQL`, `ANCH_SQL_SQLITE3`, ...).
- Warnings are errors: `-Wall -Wextra -Werror -Wconversion -Wsign-conversion ...` plus hardening (`-fstack-protector-all`, `-ftrapv`, `_FORTIFY_SOURCE=2` in optimized builds only, `-z relro -z now` at link time). Implicit narrowing/sign conversions will break the build.
- Definitions used by public headers (`ANCH_POSIX`, `ANCH_UUID` and `ANCH_DATE` for anch-json, `ANCH_CPU_*` for anch-crypto, `ANCH_SQL_*` for drivers) are PUBLIC target definitions: never add them with `add_definitions()`.
- Non-template functions defined in headers (`impl/` files) must be `inline`, otherwise linking two translation units that include them fails.
- `anch-sql` drivers (`anch-mysql`, `anch-postgresql`, `anch-sqlite`) are loaded with `dlopen("libanch-<driver>.so")` by the connection factory; tests set `LD_LIBRARY_PATH` to the build `lib/` directory. MySQL and PostgreSQL tests need running servers.
- `cmake --install` installs headers in `include/anch` and a CMake package: consumers use `find_package(Anch)` and `target_link_libraries(... anch::conf)`.

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

Assertions: `anch::ut::assertTrue(cond, msg)` and `anch::ut::assertThat(cond, "fmt {} {}", args...)` (std::format-style: the format string must be a literal and is checked at compile time, so literal braces are written `{{` / `}}`). Failures report file and line. `anch::ut::assert` is a deprecated alias (it conflicts with the `<cassert>` macro): use `assertThat`. Assertions made in threads must go through `anch::ut::ThreadErrors` (`ut/threadErrors.hpp`: `wrap()` the thread function, `rethrow()` after `join()`), otherwise a failure terminates the process. Tests run in registration order.

Each test executable is declared in `<module>/test/CMakeLists.txt` with `anch_add_executable()`, and registered once per test case with `add_test(NAME <ctest-name> COMMAND <exe> -t <test-name>)`. When adding a test case, add both the `.add(...)` registration and a matching `add_test` line. Executables are in `build/<preset>/bin/` and can be run directly from the test binary directory:

```sh
cd build/debug/anch-conf/test
../../bin/anch-conf-placeholders-test -l          # list tests
../../bin/anch-conf-placeholders-test -t single   # run one test (repeatable; default 'all')
```

Test fixture files (`.ini`, `.conf`, ...) are copied with `anch_copy_test_files()` into the test binary directory (`build/<preset>/anch-<module>/test`), which is the tests working directory.

## Code layout and conventions

- Per module: public headers in `inc/<name>/` (included as `"conf/configuration.hpp"`, `"ut/unit.hpp"`, `"cli/args.hpp"`, ...), sources in `src/<name>/`, tests in `test/`. Namespace is `anch::<name>` (e.g. `anch::conf`, `anch::ut`, `anch::cli`).
- Inline/template implementations go in `inc/<name>/impl/<file>.hpp`, `#include`d at the **end** of the matching public header. Keep declarations (with Doxygen comments) in the public header and definitions in `impl/`.
- Some modules are header-only (e.g. `anch-utils`, `anch-events`: no `src/`).
- Namespaces use nested namespace definitions: `namespace anch::sql {`, never `namespace anch { namespace sql {`. A sub-namespace gets its own block (`namespace anch::events::detail { ... }` then `namespace anch::events { ... }`) instead of being nested and indented.
- Headers use `#pragma once`. Classes are sectioned with paired markers like `// Attributes +` / `// Attributes -`, `// Constructors +` / `// Constructors -`, `// Methods +` / `// Methods -`; CMake files use the same `+`/`-` block markers. Members are prefixed with `_`. Return type goes on its own line before function definitions.
- Everything public is documented with Doxygen (`/*! ... */`, `\brief`, `\param`, `\return`, `\author`, `\since`). Each module has an `anch-<name>.dox` page; generated HTML is committed under `docs/` (published at https://vlachenal.github.io/anch-framework/) from the root `Doxyfile`.
- `*.todo` files (e.g. `anch-conf/cascade.todo`, `anch-app/anch-app.todo`) hold design notes, partly in French, for work in progress.
