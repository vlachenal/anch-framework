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
#include <string_view>
#include <format>

namespace anch::ut {

  /*!
   * Check if condition is \c true and raise unit test error otherwise
   *
   * \param check the condition to check
   * \param message the assertion message (default to empty string)
   */
  void assertTrue(bool check, const std::string& message = "");

  /*!
   * Check if condition is \c false and raise unit test error otherwise
   *
   * \param check the condition to check
   * \param message the assertion message (default to empty string)
   */
  void assertFalse(bool check, const std::string& message = "");

  /*!
   * Test has been failed ...
   *
   * \param message the assertion message (default to empty string)
   */
  void fail(const std::string& message = "");

  /*!
   * Assert clause with error message formatting
   *
   * \tparam T the arguments
   *
   * \param check \c true if test is OK, \c false otherwise
   * \param message the message which have to be formatted
   * \param args the message's arguments
   */
  template<typename... T>
  void assert(bool check, std::string_view message, const T&... args);

}  // anch::ut

#include "ut/impl/assert.hpp"
