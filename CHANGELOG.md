# Changelog {#changelog}

All notable changes of AnCH Framework are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Versions are numbered A.B.C: until
1.0.0, API and ABI can change on every B change (shared libraries SONAME is `libanch-<name>.so.A.B`); from 1.0.0, A
changes break API, B changes add features and C changes fix bugs.

Deprecation policy: a deprecated API is marked with `[[deprecated("...")]]` and `\deprecated` in its documentation,
which give its replacement and the version which removes it. Until 1.0.0, it is removed in the next B version; from
1.0.0, in the next A version.

## [Unreleased]

### Added

- anch-rest: `TransportConfig::maxResponseSize` bounds the response body read in memory by the client (64 MiB by
  default, `0` for no limit).

### Changed

- anch-oas: inconsistent constraints (`minLength` greater than `maxLength`, `minItems` greater than `maxItems`,
  `minProperties` greater than `maxProperties`, `minimum` greater than `maximum`) and required properties missing from
  `properties` are reported with a warning (error with `--strict`).
- anch-rest is built by default (`ANCH_BUILD_REST` is ON); `anch-rest-curl` is skipped with a CMake status message
  when libcurl is not found, as Protobuf, gRPC and Thrift libraries.
- anch-rest: the nginx module integration test covers HTTP/2 (h2c).

### Fixed

- anch-oas: bounds, `multipleOf` and enumeration values outside `double` range are ignored with a warning (generated
  code did not compile); `??` in descriptions and patterns no longer produces trigraphs in generated code.
- anch-rest: multipart names and file names keep backslashes (only `\"` and `\\` are escapes) and decode `%22`, `%0D`
  and `%0A` as browsers encode them; every value of multi-valued RPC response metadata is sent (Connect and
  gRPC-Web); `HEAD` responses have the `Content-Length` of the `GET` response instead of `0` with nginx.
- anch-rest: `CurlTransport` rewinds streamed request bodies to follow 307 / 308 redirections and refuses request
  headers with CR, LF or NUL (header injection).
- anch-crypto: `MD5`, `SHA1` and SHA-2 classes hid the data hashing methods of `Hash` (`digest(const std::string&)`,
  `digest(const uint8_t*, std::size_t)`, `digest(std::istream&)`, ...): a hash object can be reused.

## [0.2.0] - 2026-11-19

Version 0.2 is a stabilization of the whole framework: single CMake project, C++23, rewritten libraries (anch-network,
anch-date, anch-rest), many fixes found by analyses, sanitizers and fuzzing, and a continuous integration. Version
0.1 was never tagged: this section compares with the code before 2026-10-27.

### Breaking changes

Build and packaging:
- C++23 and GCC >= 13 (or Clang) are required. The build is a single CMake project (CMake >= 3.25) with presets:
  consumers use `find_package(Anch)` and `anch::<name>` targets (`anch::conf`, `anch::log`, ...).
- Shared libraries are built with hidden symbols visibility: only classes, functions and variables marked with the
  `ANCH_<NAME>_EXPORT` macros (generated `anch-<name>-export.hpp` headers) are exported. Helpers defined in source
  files have internal linkage and are no longer exported in the global namespace.
- Shared libraries SONAME is `libanch-<name>.so.0.2`.

Libraries:
- anch-utils: `Singleton<T>` prohibits copy and move, has protected constructor and destructor and checks that `T`
  inherits from it. `ThreadPool` is rewritten with persistent workers: `add()` returns `false` when the task is
  rejected, the destructor drops queued tasks. `convert` takes a `std::string_view` and refuses negative values for
  unsigned types and out of range values. `Optional` inherits publicly from `std::optional`.
- anch-events: `EventBus::fireEvent` no longer blocks (it behaves as former `scheduleDeferred`) and the bus uses a
  permanent thread; `Observer` has a virtual destructor.
- anch-network: rewritten. `TcpStream` is a `std::iostream`, `TcpServer` accepts connections as `TcpStream`,
  `UdpSocket` sends and receives datagrams, IPv4 and IPv6 are supported, `IOException` derives from
  `std::system_error`. Message based `TcpSocket`, `SocketEvent` events, `TcpStreamServer` / `TcpStreamClient` and
  Windows definitions are removed.
- anch-date: rewritten on `std::chrono`. Constructors and conversions are explicit, `DateFormatter` is not copyable and
  its `parse(str)` returns the `Date` by value; the 11 per field formatter classes are replaced by internal classes.
  Parsing is strict.
- anch-uuid: deprecated methods are removed; `UUID` is a 16 bytes value type.
- anch-cli: `bindNum` refuses empty and out of range values, `bindIFS` / `bindOFS` refuse files which can not be
  opened, a multiple positional argument has to be the last one. Formatting is disabled when the output is not a
  terminal, when `NO_COLOR` is set or when `ANCH_CLI_NO_FMT=1`.
- anch-ini: spaces before `=` are no longer part of the key; `getValue<bool>` accepts only `true` and `false` (case is
  ignored) and raises `ParserError` (`BAD_CAST`) otherwise; commented lines containing `=` are ignored and empty
  values are read.
- anch-conf: profiles are given by `ANCH_PROFILES` and the `default-profiles` key (`default-profile` is still
  accepted); loading order is main file, includes, then each profile of each file. anch-conf no longer depends on
  anch-events.
- anch-log: `FormatterType::ANCH_DATE` is renamed `PRECISE_DATE` (it clashed with the `ANCH_DATE` definition of
  anch-json); `LoggerFactory` diagnostics are written on standard error.
- anch-json: data after the root value is refused (`INVALID_FORMAT`), a trailing comma in objects is refused,
  `deserialize_max_field_char` (never applied) is removed and `max_payload` raises the new `PAYLOAD_TOO_LARGE` error
  code instead of `POTENTIAL_OVERFLOW`. `anch::date::Date` and `std::chrono::year_month_day` are mapped to RFC 3339
  strings when `ANCH_DATE` is defined.
- anch-sql: `Connection::release()` (called when a connection goes back to its pool) rolls back the current
  transaction: a transaction is committed only by `commit()`. Prepared statements refuse unbound parameters and
  invalid indexes; fatal errors invalidate the connection.
- anch-crypto: `Hash` no longer keeps a `_digest` pointer: `digest(...)` methods return the virtual `digest()`.
- anch-ut: assertions in threads have to go through `ThreadErrors`; tests run in registration order; runner options
  which are not implemented are refused.
- anch-rest: rewritten (HTTP model, server API, client, libcurl transport, nginx module); it is still not built by
  default (`ANCH_BUILD_REST`). The nginx module C ABI is at version 2: request and response bodies are streamed.

### Added

- Build: CMake presets `release`, `debug`, `no-db`, `asan`, `tsan`, `clang`, `coverage` and `fuzz`; installation with a
  CMake package and relative RPATH (`ANCH_INSTALL_RPATH`); `ANCH_LOG_MIN_LEVEL` option.
- Continuous integration: GCC and Clang release builds, GCC 13 on Ubuntu 24.04, AddressSanitizer,
  UndefinedBehaviorSanitizer and ThreadSanitizer, installation test with shared and static libraries, coverage,
  libFuzzer targets (JSON, multipart, forms, media types, UUID, dates, Base64, INI), nginx module integration test
  and exported symbols check (`cmake/check-exports.sh`).
- anch-utils: `ResourcePool` serves waiting borrowers in arrival order.
- anch-events: `EventHandler::unsubscribe`.
- anch-date: RFC 3339 date-time and full-date formatting and parsing (`anch::date::rfc3339`).
- anch-device: network interfaces listed with `getifaddrs` (every interface, IPv6), `NetworkInterface::isUp()` and
  `getIpv6Addresses()`; CPU detection compiles on non x86 architectures.
- anch-log: stream API with compile-time levels (`LOG << anch::log::lvl::debug << ...`); `max-size` with units and
  `max-rotate-files`.
- anch-conf: configuration cascade (includes and profiles) and event driven placeholders resolution
  (`${resolver=argument|default}`) with cycle detection.
- anch-json: required fields and value constraints (lengths, bounds, patterns, formats, ...), `validate()` /
  `isValid()`, JSON Pointer path in `MappingError`.
- anch-sql: SQL lexical scanner for placeholders, bounded prepared statements cache (LRU).
- anch-ut: assertion format checked at compile time (`assertThat`), failure location, `ThreadErrors`.
- anch-rest: content negotiation, forms, multipart/form-data (streamed parts), CORS, constraints checks by side,
  Protocol Buffers, gRPC-Web / Connect and Apache Thrift codecs and RPC.
- anch-oas: OpenAPI 3.0 models, constraints and anch-rest endpoints (server and client) generator (`anch-oas-gen`).

### Deprecated

- anch-ut: `anch::ut::assert` (conflicts with the `<cassert>` macro), replaced by `anch::ut::assertThat`; removed in
  0.3.

### Fixed

Most fixes are described in the `ANALYSE.md` files of each library. Main ones:
- anch-utils: `ThreadPool` threads which could outlive the pool, `ResourcePool` concurrency issues, `Fluent` copies,
  `Flux` readiness.
- anch-events: deadlocks and crashes when a handler used its bus or observable.
- anch-network: use-after-free and buffer overflow of the former sockets.
- anch-cutils: data corrupted in read / write mode, partial writes and write errors.
- anch-log: default pattern which did not write the message, `MDC`, `LowPriorityWriter`, file rotation, writers per
  logger, concurrent `getLogger`.
- anch-date: daylight saving time added twice, `timespec` milliseconds, `%y` and `%h`, years outside 1678-2261.
- anch-json: `0` refused, numeric overflows, unescaped / undecoded strings, unbounded recursion, ignored `max_payload`,
  ignored objects with several fields; lost wake-up in `Flux` serialization.
- anch-uuid: parsing validation, RFC 9562 compliant v3 / v5, monotonic v7.
- anch-sql: prepared statements placeholders, MySQL fatal errors never detected, DATE / TIME / TIMESTAMP columns read
  as the current date, transactions left open in pools.
- anch-crypto: out of bounds read in AES-192, uninitialized read and digest overflow in SHA-2, null SHA-1 digest,
  Base64 decoding of binary data and unpadded input.
- anch-conf: infinite loop with two profiles or includes.
- anch-ini: commented key / value lines read as values.
- anch-cli: out of range values, repeated options, ANSI codes in exception messages.

## 0.1

First version, never tagged: libraries written since 2012 (C++11 standard at first), built library by library.

[Unreleased]: https://github.com/vlachenal/anch-framework/compare/v0.2.0...HEAD
[0.2.0]: https://github.com/vlachenal/anch-framework/releases/tag/v0.2.0
