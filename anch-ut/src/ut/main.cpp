/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <optional>
#include <memory>
#include <cstdlib>
#include <exception>

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

namespace {

  /*! Unit tests options */
  struct UnitOptions {
    bool list = false;
    std::vector<std::string> tests;
    std::map<std::string,std::string> env;
  };

  /*!
   * Parse command line arguments
   *
   * \param argc the number of arguments
   * \param argv the arguments
   * \param opts the options to fill
   */
  void
  parseArgs(int argc, char** argv, UnitOptions& opts) {
    std::vector<anch::cli::Arg> args = {
      {.handler = anch::cli::bindTrue(opts.list), .sopt = 'l', .lopt = "list-tests", .description = "List tests"},
      {.handler = anch::cli::bindCol(opts.tests), .sopt = 't', .lopt = "test", .value = true, .name = "TEST", .multi = true, .description = "Test to run (can be defined several time, tests are run in command line order) ; 'all' will run all tests in registration order ; default to 'all'"}
    };
    anch::cli::ArgHandler handler(args);
    handler.printBanner(std::cout);
    handler.handle(argc, argv);
    // Remove duplicates and keep command line order +
    std::vector<std::string> tests;
    std::set<std::string> found;
    for(const std::string& test : opts.tests) {
      if(found.insert(test).second) {
	tests.push_back(test);
      }
    }
    opts.tests = std::move(tests);
    // Remove duplicates and keep command line order -
    if(opts.tests.size() > 1 && found.contains("all")) {
      std::cerr << "Test 'all' found with other tests ..." << std::endl;
      handler.printHelp(std::cerr);
      std::exit(EXIT_FAILURE);
    }
    if(opts.tests.empty()) {
      opts.tests.push_back("all");
    }
  }

}

ANCH_UT_EXPORT int
main(int argc, char** argv) {
  UnitTests tests;
  try {
    anch::ut::setup(tests);
  } catch(const std::exception& e) {
    std::cerr << "Unit tests setup failed: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch(...) {
    std::cerr << "Unit tests setup failed" << std::endl;
    return EXIT_FAILURE;
  }

  // Parse options from command line arguments +
  UnitOptions opts;
  parseArgs(argc, argv, opts);
  if(opts.list) {
    std::cout << "Unit tests:" << std::endl;
    for(const auto& test : tests.getTests()) {
      std::cout << " - " << test.first << std::endl;
    }
    return EXIT_SUCCESS;
  }
  if(opts.tests.front() == "all") {
    opts.tests.clear();
    for(const auto& test : tests.getTests()) {
      opts.tests.push_back(test.first);
    }
  } else {
    for(const std::string& test : opts.tests) {
      if(tests.find(test) == NULL) {
	std::cout << "Unit test " << test << " has not been registered." << std::endl;
	return EXIT_FAILURE;
      }
    }
  }
  // Parse options from command line arguments -

  std::unique_ptr<Runner> runner = std::make_unique<DefaultRunner>(tests);

  // Tests are not run, and uninitialization is not called, when initialization has failed
  if(!runner->startup()) {
    return EXIT_FAILURE;
  }
  std::size_t nbOK = 0;
  for(const std::string& test : opts.tests) {
    if(runner->test(test) == 0) {
      ++nbOK;
    }
  }
  bool stopped = runner->shutdown();

  if(nbOK != opts.tests.size() || !stopped) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
