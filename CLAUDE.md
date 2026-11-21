# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

AnCH Framework ("Another C++ Hack") is a set of small, independent C++ utility libraries (CLI parsing, configuration, logging, events, JSON, codecs, REST, SQL, crypto, UUID, network, dates, ...). It is also a playground for new C++ standards: the code targets **C++23 and requires GCC >= 13**. Only POSIX systems are supported (`ANCH_POSIX` is defined; `ANCH_WINDOWS` paths exist but are not maintained). License: LGPLv3. Each source file starts with the LGPL header.

## Build

The framework is a single CMake project (CMake >= 3.25): the root `CMakeLists.txt` adds every module with `add_subdirectory()` in dependency order, and modules depend on each other through targets (`anch::utils`, `anch::conf`, `anch::log`, ...). Presets are defined in `CMakePresets.json` (`release`, `debug`, `no-db`, `asan` (ASan/UBSan, GCC Debug), `asan-clang` (ASan/UBSan, Clang Release `-O2`), `tsan` (ThreadSanitizer, suppressions of non instrumented system libraries in `cmake/sanitizers/tsan.supp`), `clang` (Release with Clang), `coverage` (Clang source-based coverage, report with `cmake/coverage-report.sh`), `fuzz` (libFuzzer targets of `fuzz/`, Clang); build dirs in `build/<preset>`):

```sh
cmake --preset debug                        # configure
cmake --build --preset debug                # build everything
cmake --build --preset debug --target anch-conf-parse   # build one target
ctest --preset debug -j "$(nproc)"          # run all tests (presets "jobs": 0 means unlimited parallelism)
ctest --preset debug -L conf                # run one module's tests (label = module name without 'anch-')
ctest --preset debug -R anch-conf-pattern   # run tests by name
```

Things to know about the build:

- **Helpers are in `cmake/AnchHelpers.cmake`**: `anch_add_library()` declares a module library (target `anch-<name>`, alias `anch::<name>`, include dir `inc/`, sources `src/*.cpp`, install rules), `anch_add_executable()` declares test/utility executables, `anch_copy_test_files()` copies fixtures. Each module's `CMakeLists.txt` only lists its public/private dependencies and definitions; tests live in `<module>/test/CMakeLists.txt`.
- **Options** are declared once in the root `CMakeLists.txt`: `ANCH_BUILD_TESTS`, `ANCH_BUILD_EXAMPLES` (documentation examples, default: `ANCH_BUILD_TESTS`), `BUILD_SHARED_LIBS`, `ANCH_WARNINGS_AS_ERRORS`, `ANCH_HARDENING`, `ANCH_OPTIMIZATION_LEVEL` (`-O<level>` of `Release` builds, 3 by default, explicit instead of CMake default flags), `ANCH_NATIVE_ARCH` (OFF: `-march=native` binaries do not run on other CPUs, and ccache shared between CI runners with different CPUs served such objects), optional modules (`ANCH_BUILD_CRYPTO`, `ANCH_BUILD_CSV`, `ANCH_BUILD_SQL`, `ANCH_BUILD_OAS` and `ANCH_BUILD_REST` ON; `ANCH_BUILD_APP` OFF; anch-rest design in `anch-rest/anch-rest.todo`; the nginx module is built with nginx sources, tested by `anch-rest/nginx/test/run-nginx-test.sh`; `ANCH_REST_CURL`, `ANCH_REST_PROTOBUF`, `ANCH_REST_GRPC` and `ANCH_REST_THRIFT` are ON but each library is skipped with a status message when its packages (libcurl, gRPC) or its codec plugin are not found; tests use the `ANCH_REST_*_ENABLED` variables), anch-val (fields constraints, always built: anch-json depends on it; design in `anch-val/anch-val.todo`), anch-codec (built with anch-rest; design in `anch-codec/anch-codec.todo`): a core without format dependency (`anch::codec`) and format plugins built according to `ANCH_CODEC_JSON` (anch-codec-json, needed by anch-rest), `ANCH_CODEC_PROTOBUF` and `ANCH_CODEC_THRIFT` (skipped with a status message when Protobuf, Thrift or Boost headers are not found, `ANCH_CODEC_*_ENABLED` variables); `anch-codec/all.hpp.in` generates `codec/all.hpp` (target `anch::codec-all`: plugins built and `anch::codec::Available`), features (`ANCH_UUID`, `ANCH_DATE`, `ANCH_CPU_AES`, `ANCH_CPU_DETECTION`, `ANCH_SQL_MYSQL`, `ANCH_SQL_POSTGRESQL`, `ANCH_SQL_SQLITE3`, ...) and `ANCH_LOG_MIN_LEVEL` (anch-log compile-time level, `TRACE` to `FATAL`).
- Warnings are errors: `-Wall -Wextra -Werror -Wconversion -Wsign-conversion ...` plus hardening (`-fstack-protector-all`, `-ftrapv`, `_FORTIFY_SOURCE=2` in optimized builds only, `-z relro -z now` at link time). Implicit narrowing/sign conversions will break the build.
- Definitions used by public headers (`ANCH_POSIX`, `ANCH_UUID` and `ANCH_DATE` for anch-json (private ones for anch-val formats), `ANCH_CPU_*` for anch-crypto, `ANCH_SQL_*` for drivers, `PROTOBUF_MESSAGE_GLOBALS_TEMPORARY_OPTOUT` for anch-codec-protobuf under ASan: Protobuf changes generated messages layout with AddressSanitizer and the system libprotobuf is not instrumented) are PUBLIC target definitions: never add them with `add_definitions()`.
- Non-template functions defined in headers (`impl/` files) must be `inline`, otherwise linking two translation units that include them fails.
- Libraries are built with hidden symbols visibility (`CXX_VISIBILITY_PRESET hidden`; inline functions keep the visibility of their class: consumers built without optimization call the inline members of `extern template` instantiations): public classes, free functions and `extern` variables declared in `inc/` must be marked with the module export macro (`class ANCH_CONF_EXPORT Configuration`, `ANCH_JSON_EXPORT void f();`, `extern ANCH_LOG_EXPORT const std::string LEVEL;`), from the generated `"anch-<target>-export.hpp"` header (macro `ANCH_<TARGET>_EXPORT`, also listed in `PREDEFINED` of the `Doxyfile`). Class templates explicitly instantiated in a source file and templates holding static state (`json::Factory`) are marked too; header-only `Singleton` and `events::detail::notifyingLists` use `[[gnu::visibility("default")]]` so their instance stays unique. `cmake/check-exports.sh build/<preset>/lib` checks that nothing else is exported outside the `anch` namespace.
- `anch-sql` drivers (`anch-mysql`, `anch-postgresql`, `anch-sqlite`) are loaded with `dlopen("libanch-<driver>.so")` by the connection factory; tests set `LD_LIBRARY_PATH` to the build `lib/` directory. MySQL and PostgreSQL tests need running servers.
- `cmake --install` installs headers in `include/anch` and a CMake package: consumers use `find_package(Anch)` and `target_link_libraries(... anch::conf)`. Installed binaries have a relative RPATH (`$ORIGIN`, option `ANCH_INSTALL_RPATH`). `cmake/install-test/run-install-test.sh <work dir> [CMake options]` installs the framework and builds a consumer project with it.
- The code has to build with GCC (>= 13) and Clang: Clang refuses explicit instantiations through a `using` name (qualify the template: `template class anch::json::PrimitiveMapper<int16_t>;`) and reports lambda captures unused by some `if constexpr` instantiations (`static_cast<void>(this);`).
- CI (`.github/workflows`): `sanitizers` (asan, asan-clang, tsan), `build` (GCC and Clang release on Arch Linux with exported symbols check, GCC 13 on Ubuntu 24.04, installation test with shared and static libraries), `coverage`, `fuzz` (60 s per target, 10 min weekly), `docs` (Doxygen, deployed to GitHub Pages from `master`) and `nginx` (module integration test, also with nginx and the application built with ThreadSanitizer).

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
- Everything public is documented with Doxygen (`/*! ... */`, `\brief`, `\param`, `\return`, `\author`, `\since`). Each module has an `anch-<name>.dox` page. `mkdir -p build && doxygen Doxyfile` generates HTML in `build/docs` (not committed; Doxygen does not create `build/`); the `docs` workflow generates it (Doxygen warnings are errors) and deploys it to GitHub Pages (https://vlachenal.github.io/anch-framework/) from `master`.
- Versions, `CHANGELOG.md` (Keep a Changelog, also a Doxygen page) and deprecation policy (before 1.0.0: breaking changes are made directly, without deprecation, and listed in "Breaking changes"; from 1.0.0: `[[deprecated("removed in X, use Y")]]` + `\deprecated`, removal at the next major version) are described in `index.dox` (Versions section). Record user visible changes in the `[Unreleased]` section of `CHANGELOG.md`.
- Documentation examples are real files in `<module>/examples/` (LGPL header, `CMakeLists.txt` with `anch_add_example(<name> SOURCES ... LIBS ... [RUN [ARGS ...]])`): `.dox` pages and headers include them with `\snippet{trimleft} <module>/examples/<file>.cpp <marker>` (`EXAMPLE_PATH` is the repository root), never with inline `\code{.cpp}` blocks (except `anch-app`). A snippet is the code between two `//! [marker]` lines (a marker appears exactly twice in a file; markers can be nested, inner marker lines are not shown). `RUN` examples are CTest tests (labels: module and `examples`): only examples which need neither database nor external network service are run, others are only built. Fixtures shown in documentation (`.ini`, ...) are included with `\include`.
- `*.todo` files (e.g. `anch-conf/cascade.todo`, `anch-app/anch-app.todo`) hold design notes, partly in French, for work in progress.
