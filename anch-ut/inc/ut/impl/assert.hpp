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

#include <format>
#include <sstream>

#include "ut/error.hpp"

namespace anch::ut {

  inline void assertFalse(bool check, const std::string& message) {
    assertTrue(!check, message);
  }

  inline void fail(const std::string& message) {
    assertTrue(false, message);
  }

  template<typename... T>
  void assert(bool check, std::string_view message, const T&... args) {
    if(!check) {
      anch::ut::fail(std::vformat(message, std::make_format_args(args...)));
    }
  }

  template<typename T>
  bool equals(const char* const field,
	      const std::set<T>& expected,
	      const std::set<T>& value,
	      std::ostream& out) {
    if(expected.size() != value.size()) {
      out << field << "'s size differs ; expected=" << expected.size()
	  << " / current=" <<  value.size() << std::endl;
      return false;
    }
    int idx = 0;
    auto itE = expected.begin();
    auto itV = value.begin();
    for(; itE != expected.end() ; ++itE, ++itV, ++idx) {
      std::ostringstream oss;
      oss << field << '[' << idx << ']';
      if(!anch::ut::equals(oss.str().data(), *itE, *itV, out)) {
	return false;
      }
    }
    return true;
  }

  template<typename T>
  bool equals(const char* const field,
	      const std::list<T>& expected,
	      const std::list<T>& value,
	      std::ostream& out) {
    if(expected.size() != value.size()) {
      out << field << "'s size differs ; expected=" << expected.size()
	  << " / current=" <<  value.size() << std::endl;
      return false;
    }
    int idx = 0;
    auto itE = expected.begin();
    auto itV = value.begin();
    for(; itE != expected.end() ; ++itE, ++itV, ++idx) {
      std::ostringstream oss;
      oss << field << '[' << idx << ']';
      if(!anch::ut::equals(oss.str().data(), *itE, *itV, out)) {
	return false;
      }
    }
    return true;
  }

  template<typename T>
  bool equals(const char* const field,
	      const std::vector<T>& expected,
	      const std::vector<T>& value,
	      std::ostream& out) {
    if(expected.size() != value.size()) {
      out << field << "'s size differs ; expected=" << expected.size()
	  << " / current=" <<  value.size() << std::endl;
      return false;
    }
    for(std::size_t i = 0 ; i < expected.size() ; ++i) {
      std::ostringstream oss;
      oss << field << '[' << i << ']';
      if(!anch::ut::equals(oss.str().data(), expected[i], value[i], out)) {
	return false;
      }
    }
    return true;
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
      std::cout << field
		<< (expected  == NULL ? " is null" : " is not null")
		<< " and "
		<< (value  == NULL ? " is null" : " is not null")
		<< std::endl;
      return false;
    }
    return anch::ut::equals(field, *expected, *value, out);
  }

  template<>
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
