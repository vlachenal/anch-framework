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
#include <iostream>
#include <optional>
#include <list>
#include <set>
#include <vector>
#include <memory>

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
   * Assert clause with error message formatting (\c std::format syntax with \c {} placeholders).\n
   * The message is formatted only when the assertion fails.
   *
   * \tparam T the arguments
   *
   * \param check \c true if test is OK, \c false otherwise
   * \param message the message which have to be formatted
   * \param args the message's arguments
   *
   * \throw anch::ut::AssertException \c check is \c false
   */
  template<typename... T>
  void assertThat(bool check, std::string_view message, const T&... args);

  /*!
   * Assert clause with error message formatting
   *
   * \deprecated Use \ref assertThat : \c assert is also the \c <cassert> macro, so calls are expanded by the macro when
   * \c <cassert> is included (the name is written between parentheses here so that this header always compiles).
   *
   * \tparam T the arguments
   *
   * \param check \c true if test is OK, \c false otherwise
   * \param message the message which have to be formatted
   * \param args the message's arguments
   *
   * \throw anch::ut::AssertException \c check is \c false
   */
  template<typename... T>
  [[deprecated("Use anch::ut::assertThat: assert conflicts with <cassert> macro")]]
  void (assert)(bool check, std::string_view message, const T&... args);

  /*!
   * Check if optional value is equals to expected pointer value ; print the field and expected versus current value when \c false ; return the result
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to \c{std::cout})
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equals(const char* const field, const std::set<T>& expected, const std::set<T>& value, std::ostream& out = std::cout);

  /*!
   * Check if optional value is equals to expected pointer value ; print the field and expected versus current value when \c false ; return the result
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to \c{std::cout})
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equals(const char* const field, const std::list<T>& expected, const std::list<T>& value, std::ostream& out = std::cout);

  /*!
   * Check if optional value is equals to expected pointer value ; print the field and expected versus current value when \c false ; return the result
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to \c{std::cout})
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equals(const char* const field, const std::vector<T>& expected, const std::vector<T>& value, std::ostream& out = std::cout);

  /*!
   * Check if optional value is equals to expected pointer value ; print the field and expected versus current value when \c false ; return the result
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to \c{std::cout})
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equals(const char* const field, const std::optional<T>& expected, const std::optional<T>& value, std::ostream& out = std::cout);

  /*!
   * Check if shared pointer value is equals to expected shared pointer value ; print the field and expected versus current value when \c false ; return the result
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to \c{std::cout})
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equals(const char* const field, const std::shared_ptr<T>& expected, const std::shared_ptr<T>& value, std::ostream& out = std::cout);

  /*!
   * Check if pointer value is equals to expected pointer value ; print the field and expected versus current value when \c false ; return the result
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to \c{std::cout})
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equalsPtr(const char* const field, const T* const expected, const T* const value, std::ostream& out = std::cout);

  /*!
   * Check if value is equals to expected value ; print the field and expected versus current value when \c false ; return the result
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to \c{std::cout})
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equals(const char* const field, const T& expected, const T& value, std::ostream& out = std::cout);

}  // anch::ut

#include "ut/impl/assert.hpp"
