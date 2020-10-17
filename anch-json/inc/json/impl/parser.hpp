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

namespace anch {
  namespace json {

    template<typename T>
    inline
    bool
    serialize(const T& value,
	      std::ostream& out,
	      std::function<void((const T& value, std::ostream& out))> serializeFunc,
	      const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      std::invoke(serializeFunc, value, out);
      return true;
    }

    template<typename T>
    inline
    bool
    serialize(const T* const value,
	      std::ostream& out,
	      std::function<void((const T& value, std::ostream& out))> serializeFunc,
	      const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      return serialize(*value, out, serializeFunc, field);
    }

    template<typename T>
    inline
    bool
    serialize(const std::optional<T>& value,
	      std::ostream& out,
	      std::function<void((const T& value, std::ostream& out))> serializeFunc,
	      const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      return serialize(value.value(), out, serializeFunc, field);
    }

    template<typename T, typename A>
    inline
    void
    serializeArray(const A& array,
		   std::ostream& out,
		   std::function<void((const T& value, std::ostream& out))> serializeFunc,
		   const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::ARRAY_BEGIN;
      for(auto iter = array.begin() ; iter != array.end() ; ++iter) {
	if(iter != array.begin()) {
	  out << anch::json::FIELD_SEPARATOR;
	}
	std::invoke(serializeFunc, *iter, out);
	//serialize(*iter, out, anch::json::EMPTY_FIELD);
      }
      out << anch::json::ARRAY_END;
    }

    template<typename T>
    inline
    void
    deserializeArray(std::istream& input,
		     std::function<void(const T&)> pushFunc,
		     std::function<void((T& value, std::istream& input))> deserializeFunc) {
      if(anch::json::isNull(input)) {
	return;
      }
      int current = input.get();
      if(current != anch::json::ARRAY_BEGIN) {
	throw 128;
      }
      anch::json::discardChars(input);
      if(input.peek() != anch::json::ARRAY_END) {
	while(input) {
	  T value;
	  std::invoke(deserializeFunc, value, input);
	  std::invoke(pushFunc, value);
	  if(!anch::json::hasMoreField(input)) {
	    break;
	  }
	  anch::json::discardChars(input);
	}
      }
      if(!input || input.get() != anch::json::ARRAY_END) {
	throw 2048; // \todo error ...
      }
    }

  }  // json
}  // anch
