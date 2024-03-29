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

#include "json/mappingError.hpp"

namespace anch::json {

  template<typename T>
  inline bool
  serialize(const T& value,
	    std::ostream& out,
	    std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
	    const anch::json::MappingOptions& options,
	    const std::optional<std::string>& field) {
    if(field.has_value()) {
      out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
    }
    std::invoke(serializeFunc, value, out, options);
    return true;
  }

  template<typename T>
  inline bool
  serialize(const T* const value,
	    std::ostream& out,
	    std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
	    const anch::json::MappingOptions& options,
	    const std::optional<std::string>& field) {
    if(value == NULL) {
      return serializeNull(out, options, field);
    }
    return serialize(*value, out, serializeFunc, options, field);
  }

  template<typename T>
  inline bool
  serialize(const std::optional<T>& value,
	    std::ostream& out,
	    std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
	    const anch::json::MappingOptions& options,
	    const std::optional<std::string>& field) {
    if(!value.has_value()) {
      return serializeNull(out, options, field);
    }
    return serialize(value.value(), out, serializeFunc, options, field);
  }

  template<typename T, typename A>
  inline void
  serializeArray(const A& array,
		 std::ostream& out,
		 std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
		 const anch::json::MappingOptions& options,
		 const std::optional<std::string>& field) {
    if(field.has_value()) {
      out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
    }
    out << anch::json::ARRAY_BEGIN;
    for(auto iter = array.begin() ; iter != array.end() ; ++iter) {
      if(iter != array.begin()) {
	out << anch::json::FIELD_SEPARATOR;
      }
      std::invoke(serializeFunc, *iter, out, options);
    }
    out << anch::json::ARRAY_END;
  }

  template<typename T>
  inline void
  serializeMap(const std::map<std::string,T>& map,
	       std::ostream& out,
	       std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
	       const anch::json::MappingOptions& options,
	       const std::optional<std::string>& field) {
    if(field.has_value()) {
      out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
    }
    out << anch::json::OBJECT_BEGIN;
    for(auto iter = map.cbegin() ; iter != map.cend() ; ++iter) {
      if(iter != map.cbegin()) {
	out << anch::json::FIELD_SEPARATOR;
      }
      out << anch::json::STRING_DELIMITER << iter->first << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      std::invoke(serializeFunc, iter->second, out, options);
    }
    out << anch::json::OBJECT_END;
  }

  template<typename T>
  inline void
  deserialize(T& value,
	      std::istream& input,
	      const anch::json::MappingOptions& options,
	      std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc) {
    if(!anch::json::isNull(input, options)) { // this function discards 'spaces'
      std::invoke(deserializeFunc, value, input, options);
    }
  }

  template<typename T>
  inline void
  deserialize(T* value,
	      std::istream& input,
	      const anch::json::MappingOptions& options,
	      std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc) {
    if(anch::json::isNull(input, options)) { // this function discards 'spaces'
      value = NULL;
    } else {
      value = new T();
      std::invoke(deserializeFunc, *value, input, options);
    }
  }

  template<typename T>
  inline void
  deserialize(std::optional<T>& value,
	      std::istream& input,
	      const anch::json::MappingOptions& options,
	      std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc) {
    if(anch::json::isNull(input, options)) { // this function discards 'spaces'
      value.reset();
    } else {
      T instance;
      std::invoke(deserializeFunc, instance, input, options);
      value = std::move(instance);
    }
  }

  template<typename T>
  inline void
  deserializeArray(std::istream& input,
		   std::function<void(const T&)> pushFunc,
		   const anch::json::MappingOptions& options,
		   std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc) {
    if(anch::json::isNull(input, options)) {
      return;
    }
    int current = input.get();
    if(current != anch::json::ARRAY_BEGIN) {
      throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
    }
    anch::json::discardChars(input, options);
    if(input.peek() != anch::json::ARRAY_END) {
      while(input) {
	T value;
	std::invoke(deserializeFunc, value, input, options);
	std::invoke(pushFunc, value);
	if(!anch::json::hasMoreField(input, options)) {
	  break;
	}
	anch::json::discardChars(input, options);
      }
    }
    if(!input || input.get() != anch::json::ARRAY_END) {
      throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(input.peek()));
    }
  }

  template<typename T>
  void deserializeMap(std::istream& input,
		      std::function<void(const std::pair<std::string,T>&)> pushFunc,
		      const anch::json::MappingOptions& options,
		      std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc) {
    if(anch::json::isNull(input, options)) {
      return;
    }
    int current = input.get();
    if(current != anch::json::OBJECT_BEGIN) {
      throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
    }
    anch::json::discardChars(input, options);
    if(input.peek() == anch::json::OBJECT_END) {
      input.get();
      return;
    }
    do {
      std::optional<std::string> key = anch::json::getFieldName(input, options);
      if(!key) {
	break;
      }
      if(anch::json::isNull(input, options)) {
	continue;
      }
      T val;
      std::invoke(deserializeFunc, val, input, options);
      auto value = std::make_pair(key.value(), val);
      std::invoke(pushFunc, value);
      if(!anch::json::hasMoreField(input, options)) {
	break;
      }
    } while(true);
    if(!input || input.get() != anch::json::OBJECT_END) {
      throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(input.peek()));
    }
  }

}  // anch::json
