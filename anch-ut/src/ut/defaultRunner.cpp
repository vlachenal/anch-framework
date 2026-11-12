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
#include "ut/defaultRunner.hpp"

#include <iostream>
#include <chrono>
#include <functional>
#include <source_location>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string>

#include "ut/error.hpp"
#include "cli/formatter.hpp"

using anch::ut::DefaultRunner;

namespace {

  /*! Description of the running step (initialization, test, uninitialization) */
  std::string currentStep;

  /*! Running step (\c NULL when no step is running): read by exit handler */
  std::atomic<const char*> runningStep(NULL);

  /*!
   * Exit handler: when \c exit() (or \c quick_exit() ) is called while a step is running (by a test or by the tested
   * code), the remaining tests would not be run and the exit code given to \c exit() (often \c 0 ) would be the result
   * of the unit tests. Report the step and exit with failure.
   */
  void
  onExit() {
    const char* step = runningStep.load();
    if(step == NULL) {
      return;
    }
    std::cout << std::endl << anch::cli::Formatter::format().fgColor(anch::cli::Color::RED)
	      << "exit() has been called during " << step << ": unit tests are failed"
	      << anch::cli::RESET << std::endl;
    std::fflush(NULL);
    std::_Exit(EXIT_FAILURE);
  }

  /*!
   * Register exit handler once
   */
  void
  registerExitHandler() {
    static std::once_flag once;
    std::call_once(once, []() {
      std::atexit(onExit);
      std::at_quick_exit(onExit);
    });
  }

  /*!
   * \brief Running step guard
   *
   * Set running step on construction and reset it on destruction.
   */
  class StepGuard {
  public:
    /*!
     * \c StepGuard constructor
     *
     * \param step the step description
     */
    explicit StepGuard(const std::string& step) {
      currentStep = step;
      runningStep = currentStep.c_str();
    }

    /*!
     * \c StepGuard destructor
     */
    ~StepGuard() {
      runningStep = NULL;
    }
  };

  /*!
   * Print error message
   *
   * \param what the step which has failed (\c "Unit test xxx execution" for example)
   * \param reason the failure reason
   * \param location the failed assertion location
   */
  void
  printFailure(const std::string& what,
	       const std::string& reason,
	       const std::source_location& location = std::source_location()) {
    std::cout << anch::cli::Formatter::format().fgColor(anch::cli::Color::RED) << what << " failed" << reason;
    if(location.line() != 0) {
      std::cout << std::endl << "  at " << location.file_name() << ':' << location.line()
		<< " in " << location.function_name();
    }
    std::cout << anch::cli::RESET << std::endl;
  }

  /*!
   * Run step function and report its failure
   *
   * \param what the step description (\c "Unit test xxx execution" for example)
   * \param func the function to run
   *
   * \return \c true when function has succeeded, \c false otherwise
   */
  bool
  runStep(const std::string& what, const std::function<void(void)>& func) {
    try {
      std::invoke(func);
      return true;
    } catch(const anch::ut::AssertException& e) {
      printFailure(what, std::string(" on assertion: ") + e.what(), e.location());
    } catch(const std::exception& e) {
      printFailure(what, std::string(": ") + e.what());
    } catch(...) {
      printFailure(what, "");
    }
    return false;
  }

  /*!
   * Print test end
   *
   * \param name the test name
   * \param start the test start time
   */
  void
  printEnd(const std::string& name, std::chrono::steady_clock::time_point start) {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);
    std::cout << std::endl << "Time: " << duration.count() << " µs" << std::endl;
    std::cout << anch::cli::Formatter::format().fgColor(anch::cli::Color::BLUE)
	      << "-- " << name << " --" << anch::cli::RESET << std::endl << std::endl;
  }

}


DefaultRunner::DefaultRunner(const UnitTests& tests): _tests(tests) {
  registerExitHandler();
}

bool
DefaultRunner::startup() {
  StepGuard guard("unit tests initialization");
  return runStep("Unit tests initialization", [this]() { _tests.initialize(); });
}

bool
DefaultRunner::shutdown() {
  StepGuard guard("unit tests uninitialization");
  return runStep("Unit tests uninitialization", [this]() { _tests.uninitialize(); });
}

uint16_t
DefaultRunner::test(const std::string& test) {
  auto start = std::chrono::steady_clock::now();
  const std::function<void(void)>* func = _tests.find(test);
  if(func == NULL) {
    std::cout << test << " has not been registered" << std::endl;
    printEnd(test, start);
    return 1;
  }

  std::cout << anch::cli::Formatter::format().fgColor(anch::cli::Color::BLUE)
	    << "-- " << test << " --" << anch::cli::RESET << std::endl;
  StepGuard guard("unit test " + test);

  if(!runStep("Unit test " + test + " initialization", [this]() { _tests.before(); })) {
    printEnd(test, start);
    return 2;
  }
  bool passed = runStep("Unit test " + test + " execution", *func);
  bool after = runStep("Unit test " + test + " uninitialization", [this]() { _tests.after(); });
  printEnd(test, start);

  if(!passed) {
    return 3;
  }
  if(!after) {
    return 4;
  }
  return 0;
}
