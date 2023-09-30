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

#include "ut/runner.hpp"
#include "ut/unit.hpp"

namespace anch::ut {

  /*!
   * \brief Socket client unit tests runner
   *
   * This runner will execute test on receive messages
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class SocketClientRunner: public anch::ut::Runner {

  private:
    /*! Unit tests */
    anch::ut::UnitTests _tests;

  public:
    /*!
     * Forbids \ref SocketClientRunner default constructor
     */
    SocketClientRunner() = delete;

    /*!
     * Forbids \ref SocketClientRunner copy constructor
     */
    SocketClientRunner(const SocketClientRunner&) = delete;

    /*!
     * \ref SocketClientRunner constructor
     *
     * \param uri the socket URI
     */
    SocketClientRunner(const std::string& uri);

  public:
    /*!
     * \brief SocketClientRunner destructor
     */
    ~SocketClientRunner();

  public:
    /*!
     * Startup test runner
     *
     * \return \c true when startup is OK, \c false otherwise
     */
    virtual bool startup() override;

    /*!
     * Shutdown test runner
     *
     * \return \c true when shutdown is OK, \c false otherwise
     */
    virtual bool shutdown() override;

    /*!
     * Run test
     *
     * \param test the test to run
     *
     * \return \c 0 when OK, \c 1 when before test processing failed, \c 2 when test failed and \c 3 when after test processing failed
     */
    virtual uint16_t test(const std::string& test) override;

  };

}  // anch::ut
