/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <optional>

#include "ut/unit.hpp"
#include "ut/runner.hpp"
#include "ut/defaultRunner.hpp"

#include "cli/args.hpp"
#include "cli/utils.hpp"

using anch::ut::UnitTests;
using anch::ut::Runner;
using anch::ut::DefaultRunner;

namespace anch::ut {

  enum class TestStep {
    INITIALIZED = 0,
    BEFORE_TEST,
    EXECUTE,
    AFTER_TEST
  };

  class TestReport {
  };

  class TestsReport {
  };

}  // anch::ut

struct UnitOptions {
  bool list = false;
  bool start = false;
  bool shutdown = false;
  bool genCTEST = false;
  std::optional<std::string> runner;
  std::set<std::string> tests;
  std::map<std::string,std::string> env;
};

void
parseArgs(int argc, char** argv, UnitOptions& opts) {
  std::vector<anch::cli::Arg> args = {
    {.handler = anch::cli::bindTrue(opts.list), .sopt = 'l', .lopt = "list-tests", .description = "List tests"},
    {.handler = anch::cli::bindTrue(opts.start), .lopt = "start", .description = "Start runner (error if runner is not defined)"},
    {.handler = anch::cli::bindTrue(opts.shutdown), .lopt = "stop", .description = "Stop runner (error if runner is not defined)"},
    {.handler = anch::cli::bindTrue(opts.genCTEST), .lopt = "gen-ctest", .description = "Generate CTest file"},
    {.handler = anch::cli::bindStr(opts.runner), .sopt = 'r', .lopt = "runner", .value = true, .name = "RUNNER", .description = "Use specific runner with URI specific (udp, tcp and file supported for now)", .example = "file://./test.sock or tcp://localhost:54321 or udp://mydomain.com:54321"},
    {.handler = anch::cli::bindCol(opts.tests), .sopt = 't', .lopt = "test", .value = true, .name = "TEST", .multi = true, .description = "Test to run (can be defined several time) ; 'all' will run all tests ; default to 'all'"}
  };
  anch::cli::ArgHandler handler(args);
  handler.printBanner(std::cout);
  handler.handle(argc, argv);
  if(opts.tests.size() > 1 && opts.tests.contains("all")) {
    std::cerr << "Test 'all' found with other tests ..." << std::endl;
    handler.printHelp(std::cerr);
    std::exit(1);
  }
  if(opts.tests.empty()) {
    opts.tests.insert("all");
  }
}

int
main(int argc, char** argv) {
  UnitTests tests;
  anch::ut::setup(tests);

  // Parse options from command line arguments +
  UnitOptions opts;
  parseArgs(argc, argv, opts);
  if(opts.list) {
    std::cout << "Unit tests:" << std::endl;
    for(auto iter = tests.getTests().cbegin() ; iter != tests.getTests().cend() ; ++iter) {
      std::cout << " - " << iter->first << std::endl;
    }
    std::exit(EXIT_SUCCESS);
  }
  if(opts.tests.contains("all")) {
    std::set<std::string> ts;
    for(auto iter = tests.getTests().cbegin() ; iter != tests.getTests().cend() ; ++iter) {
      ts.insert(iter->first);
    }
    opts.tests = ts;
  } else {
    for(auto test : opts.tests) {
      if(!tests.getTests().contains(test)) {
	std::cout << "Unit test " << test << " has not been registered." << std::endl;
	std::exit(EXIT_FAILURE);
      }
    }
  }
  // Parse options from command line arguments -

  // Manage unit tests runner +
  std::shared_ptr<Runner> runner;
  if(!opts.runner.has_value()) {
    runner = std::make_shared<DefaultRunner>(tests);
  } // \todo manage other runners (TCP, UDP, UDS)
  // Manage unit tests runner -

  bool started = runner->startup();
  uint16_t nbOK = 0;
  for(auto iter = opts.tests.cbegin() ; iter != opts.tests.cend() ; ++iter) {
    if(!started) {
      continue;
    }
    uint16_t res = runner->test(*iter);
    if(res == 0) {
      ++nbOK;
    }
  }
  runner->shutdown();

  if(nbOK != opts.tests.size()) {
    std::exit(EXIT_FAILURE);
  }

  return 0;
}
