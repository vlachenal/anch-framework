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
#pragma once

#include <string>
#include <string_view>
#include <format>
#include <source_location>
#include <concepts>
#include <type_traits>
#include <iostream>
#include <optional>
#include <list>
#include <set>
#include <vector>
#include <memory>

namespace anch::ut {

  /*!
   * \brief Assertion message with its location
   *
   * Format string of \ref anch::ut::assertThat "assertThat" : the format string is checked at compile time against the arguments types
   * (as \c std::format does) and the location of the call is captured.\n
   * It is built implicitly from a string literal ; it should not be used directly.
   *
   * \tparam T the message's arguments types
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<typename... T>
  struct AssertMessage {

    /*! Format string checked at compile time */
    std::format_string<const T&...> format;

    /*! Assertion location */
    std::source_location location;

    /*!
     * \ref AssertMessage constructor
     *
     * \tparam S the format string type (string literal)
     *
     * \param fmt the format string
     * \param loc the assertion location (default to caller location)
     */
    template<typename S>
    requires std::convertible_to<const S&, std::string_view>
    consteval AssertMessage(const S& fmt, std::source_location loc = std::source_location::current());

  };

  /*!
   * Test has been failed ...
   *
   * \param message the assertion message (default to empty string)
   * \param location the assertion location (default to caller location)
   *
   * \throw anch::ut::AssertException always
   */
  [[noreturn]]
  void fail(std::string_view message = "", const std::source_location& location = std::source_location::current());

  /*!
   * Check if condition is \c true and raise unit test error otherwise
   *
   * \param check the condition to check
   * \param message the assertion message (default to empty string)
   * \param location the assertion location (default to caller location)
   *
   * \throw anch::ut::AssertException \c check is \c false
   */
  void assertTrue(bool check, std::string_view message = "", const std::source_location& location = std::source_location::current());

  /*!
   * Check if condition is \c false and raise unit test error otherwise
   *
   * \param check the condition to check
   * \param message the assertion message (default to empty string)
   * \param location the assertion location (default to caller location)
   *
   * \throw anch::ut::AssertException \c check is \c true
   */
  void assertFalse(bool check, std::string_view message = "", const std::source_location& location = std::source_location::current());

  /*!
   * Assert clause with error message formatting (\c std::format syntax with \c {} placeholders).\n
   * The message is checked at compile time (it has to be a string literal whose placeholders match the arguments) and
   * is formatted only when the assertion fails. The assertion location is reported with the message.
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
  void assertThat(bool check, std::type_identity_t<anch::ut::AssertMessage<T...>> message, const T&... args);

  /*!
   * Assert clause with error message formatting
   *
   * \deprecated Use \ref anch::ut::assertThat "assertThat" : \c assert is also the \c \<cassert\> macro, so calls are expanded by the macro when
   * \c \<cassert\> is included (the name is written between parentheses here so that this header always compiles).
   * The message is checked only when the assertion fails and the location is not reported.
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
  [[deprecated("Use anch::ut::assertThat: assert conflicts with the assert macro of cassert header")]]
  void (assert)(bool check, std::string_view message, const T&... args);

  /*!
   * Check if optional value is equals to expected pointer value ; print the field and expected versus current value when \c false ; return the result
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to <tt>std::cout</tt>)
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
   * \param out the output stream to print in when \c false (default to <tt>std::cout</tt>)
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
   * \param out the output stream to print in when \c false (default to <tt>std::cout</tt>)
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
   * \param out the output stream to print in when \c false (default to <tt>std::cout</tt>)
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
   * \param out the output stream to print in when \c false (default to <tt>std::cout</tt>)
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equals(const char* const field, const std::shared_ptr<T>& expected, const std::shared_ptr<T>& value, std::ostream& out = std::cout);

  /*!
   * Check if pointer value is equals to expected pointer value ; print the field and expected versus current value when \c false ; return the result.\n
   * Pointed values are compared when both pointers are not \c NULL .
   *
   * \tparam T the value type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false (default to <tt>std::cout</tt>)
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
   * \param out the output stream to print in when \c false (default to <tt>std::cout</tt>)
   *
   * \return \c true when expect equals to value, \c false otherwise
   */
  template<typename T>
  bool equals(const char* const field, const T& expected, const T& value, std::ostream& out = std::cout);

}  // anch::ut

#include "ut/impl/assert.hpp"
