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
#pragma once

#include <string>
#include <cstdint>

namespace anch::ut {

  /*!
   * \brief Unit tests runner
   *
   * Vitual class which define tests runner
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class Runner {

  public:
    /*!
     * Startup test runner
     *
     * \return \c true when startup is OK, \c false otherwise
     */
    virtual bool startup() = 0;

    /*!
     * Shutdown test runner
     *
     * \return \c true when shutdown is OK, \c false otherwise
     */
    virtual bool shutdown() = 0;

    /*!
     * Run test
     *
     * \param test the test to run
     *
     * \return \c 0 when OK, \c 1 when test has not ben registered, \c 2 when before test processing failed, \c 3 when test failed and \c 4 when after test processing failed
     */
    virtual uint16_t test(const std::string& test) = 0;

  };

}  // anch::ut
