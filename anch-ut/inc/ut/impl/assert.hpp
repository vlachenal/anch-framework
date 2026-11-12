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

#include <format>
#include <sstream>
#include <iterator>
#include <concepts>

#include "ut/error.hpp"

namespace anch::ut {

  template<typename... T>
  template<typename S>
  requires std::convertible_to<const S&, std::string_view>
  consteval AssertMessage<T...>::AssertMessage(const S& fmt, std::source_location loc): format(fmt), location(loc) {
    // Nothing to do
  }

  inline void
  assertTrue(bool check, std::string_view message, const std::source_location& location) {
    if(!check) {
      anch::ut::fail(message, location);
    }
  }

  inline void
  assertFalse(bool check, std::string_view message, const std::source_location& location) {
    if(check) {
      anch::ut::fail(message, location);
    }
  }

  template<typename... T>
  void
  assertThat(bool check, std::type_identity_t<anch::ut::AssertMessage<T...>> message, const T&... args) {
    if(!check) {
      anch::ut::fail(std::format(message.format, args...), message.location);
    }
  }

  template<typename... T>
  void
  (assert)(bool check, std::string_view message, const T&... args) {
    if(!check) {
      throw anch::ut::AssertException(std::vformat(message, std::make_format_args(args...)));
    }
  }

  /*!
   * Compare collections' elements one by one. The name of an element is built only when it differs.
   *
   * \tparam C the collection type
   *
   * \param field the field name
   * \param expected the expected value
   * \param value the current value
   * \param out the output stream to print in when \c false
   *
   * \return \c true when collections are equals, \c false otherwise
   */
  template<typename C>
  bool
  equalsCollection(const char* const field, const C& expected, const C& value, std::ostream& out) {
    if(expected.size() != value.size()) {
      out << field << "'s size differs ; expected=" << expected.size()
	  << " / current=" <<  value.size() << std::endl;
      return false;
    }
    using T = typename C::value_type;
    if constexpr(std::equality_comparable<T>) { // collections' operator== is not constrained on element type
      if(expected == value) {
	return true;
      }
    }
    std::size_t idx = 0;
    auto itE = expected.begin();
    auto itV = value.begin();
    for(; itE != expected.end() ; ++itE, ++itV, ++idx) {
      if constexpr(std::equality_comparable<T>) {
	if(*itE == *itV) {
	  continue;
	}
      }
      std::string name = std::format("{}[{}]", field, idx);
      if(!anch::ut::equals(name.data(), *itE, *itV, out)) {
	return false;
      }
    }
    return true;
  }

  template<typename T>
  bool
  equals(const char* const field, const std::set<T>& expected, const std::set<T>& value, std::ostream& out) {
    return anch::ut::equalsCollection(field, expected, value, out);
  }

  template<typename T>
  bool
  equals(const char* const field, const std::list<T>& expected, const std::list<T>& value, std::ostream& out) {
    return anch::ut::equalsCollection(field, expected, value, out);
  }

  template<typename T>
  bool
  equals(const char* const field, const std::vector<T>& expected, const std::vector<T>& value, std::ostream& out) {
    return anch::ut::equalsCollection(field, expected, value, out);
  }

  template<typename T>
  bool equals(const char* const field,
	      const std::optional<T>& expected,
	      const std::optional<T>& value,
	      std::ostream& out) {
    if(expected.has_value() && !value.has_value()) {
      out << field << " differs ; expected=" << expected.value() << " / current is empty" << std::endl;
      return false;
    }
    if(!expected.has_value() && value.has_value()) {
      out << field << " differs ; expected should be empty / current=" << value.value() << std::endl;
      return false;
    }
    if(expected != value) {
      out << field << " differs ; expected='" << expected.value() << "' / current='" << value.value() << '\'' << std::endl;
      return false;
    }
    return true;
  }

  template<typename T>
  bool equals(const char* const field,
	      const std::shared_ptr<T>& expected,
	      const std::shared_ptr<T>& value,
	      std::ostream& out) {
    if(!expected && !value) {
      return true;
    }
    if(expected && !value) {
      out << field << " differs ; expected=" << *expected << " / current is NULL" << std::endl;
      return false;
    }
    if(!expected && value) {
      out << field << " differs ; expected should be NULL / current=" << *value << std::endl;
      return false;
    }
    if(*expected != *value) {
      out << field << " differs ; expected='" << *expected << "' / current='" << *value << '\'' << std::endl;
      return false;
    }
    return true;
  }

  template<typename T>
  bool equalsPtr(const char* const field, const T* const expected, const T* const value, std::ostream& out) {
    if(expected == value) {
      return true;
    } else if(expected == NULL || value == NULL) {
      out << field << " differs ; expected" << (expected == NULL ? " is null" : " is not null")
	  << " / current" << (value == NULL ? " is null" : " is not null") << std::endl;
      return false;
    }
    return anch::ut::equals(field, *expected, *value, out);
  }

  template<>
  inline
  bool equals(const char* const field, const long double& expected, const long double& value, std::ostream& out) {
    if(expected != value) {
      out << field << " differs ; expected=" << expected << " / current=" << value << std::endl;
      if(static_cast<double>(expected) == static_cast<double>(value)) {
	out << "(double)" << field << " is equals" << std::endl;
	return true;
      }
      return false;
    }
    return true;
  }

  template<typename T>
  bool equals(const char* const field, const T& expected, const T& value, std::ostream& out) {
    if(expected != value) {
      out << field << " differs ; expected='" << expected << "' / current='" << value << '\'' << std::endl;
      return false;
    }
    return true;
  }

}
