# anch-framework
AnCH Framework - Another C++ Hack "Framework"

AnCH framework is a set of small and independent C++ libraries for common programming needs (command line, configuration, logging, events, JSON, SQL, REST, cryptography, UUID, network, dates, unit tests, ...). Features are implemented to be used as simply as possible.

This framework is also a playground for new C++ standards: version 0.2 targets C++23 (GCC 13 or later, CMake 3.25 or later). Only POSIX systems are supported.

```sh
cmake --preset release
cmake --build --preset release
ctest --preset release
```

Doxygen documentation can be found on [project home page](https://vlachenal.github.io/anch-framework/).

## AI assisted development

Since version 0.2, part of the work (analyses, bug fixes, rewrites, tests, CI and documentation) has been done with [Claude Code](https://claude.com/claude-code), Anthropic's AI assistant. Design decisions remain the maintainer's. Commits written with the assistant carry a `Co-Authored-By: Claude` trailer.

## License

LGPL v3 (see [COPYING.LESSER](COPYING.LESSER)).
