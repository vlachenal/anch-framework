/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "cli/args.hpp"
#include "cli/utils.hpp"

using anch::cli::App;
using anch::cli::ArgHandler;

//! [example]
// Application options
struct Options {
  bool verbose = false;
  std::optional<std::string> str;
  std::optional<std::string> str2;
  std::optional<std::string> pos;
  std::optional<std::string> pos2;
  std::vector<std::string> multPos;
};

int
main(int argc, char** argv) {
  App application = {
    .name = "my-app",
    .version = "1.0.0",
    .author = "John Doe",
    .copyright = "Copyright (C) 2026 John Doe",
    .licence = "WTFPL 2.0",
    .banner = "My application"
  };
  Options opts;
  {
    ArgHandler handler(application, {
        {.handler = anch::cli::bindTrue(opts.verbose), .sopt = 'v', .lopt = "verbose", .description = "Verbose mode"},
        {.handler = anch::cli::bindStr(opts.str), .sopt = 'p', .lopt = "plop", .value = true, .name = "trestreslong", .description = "plop arg", .example = "PLOP!!!"},
        {.handler = anch::cli::bindStr(opts.str2), .lopt = "plip", .value = true, .description = "plip arg"},
        {.handler = anch::cli::bindStr(opts.pos), .value = true, .name = "POS1", .description = "pos"},
        {.handler = anch::cli::bindStr(opts.pos2), .value = true, .name = "POS2", .description = "pos2"},
        {.handler = anch::cli::bindCol(opts.multPos), .value = true, .name = "POSM", .multi = true, .description = "pos-mult"}
      });
    handler.printBanner(std::cerr);
    handler.handle(argc, argv); // prints help and exits on invalid or missing arguments
  }
  std::cout << "POS1=" << opts.pos.value() << " POS2=" << opts.pos2.value() << " POSM=" << opts.multPos.size() << " values" << std::endl;
  return 0;
}
//! [example]
