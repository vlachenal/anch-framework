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

#include "json/lexer.hpp"

namespace anch::json {

  inline
  void
  serializeFieldName(std::ostream& out, const std::optional<std::string>& field) {
    if(field.has_value()) {
      anch::json::serializeFieldName(out, field.value());
    }
  }

  template<typename T>
  inline bool
  serialize(const T& value,
	    std::ostream& out,
	    anch::json::SerializeFn<T> serializeFunc,
	    const anch::json::MappingOptions& options,
	    const std::optional<std::string>& field) {
    anch::json::serializeFieldName(out, field);
    std::invoke(serializeFunc, value, out, options);
    return true;
  }

  template<typename T>
  inline bool
  serialize(const T* const value,
	    std::ostream& out,
	    anch::json::SerializeFn<T> serializeFunc,
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
	    anch::json::SerializeFn<T> serializeFunc,
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
		 anch::json::SerializeFn<T> serializeFunc,
		 const anch::json::MappingOptions& options,
		 const std::optional<std::string>& field) {
    anch::json::serializeFieldName(out, field);
    out.put(anch::json::ARRAY_BEGIN);
    for(auto iter = array.cbegin() ; iter != array.cend() ; ++iter) {
      if(iter != array.cbegin()) {
	out.put(anch::json::FIELD_SEPARATOR);
      }
      std::invoke(serializeFunc, *iter, out, options);
    }
    out.put(anch::json::ARRAY_END);
  }

  template<typename T>
  inline void
  serializeMap(const std::map<std::string,T>& map,
	       std::ostream& out,
	       anch::json::SerializeFn<T> serializeFunc,
	       const anch::json::MappingOptions& options,
	       const std::optional<std::string>& field) {
    anch::json::serializeFieldName(out, field);
    out.put(anch::json::OBJECT_BEGIN);
    for(auto iter = map.cbegin() ; iter != map.cend() ; ++iter) {
      if(iter != map.cbegin()) {
	out.put(anch::json::FIELD_SEPARATOR);
      }
      anch::json::serializeFieldName(out, iter->first);
      std::invoke(serializeFunc, iter->second, out, options);
    }
    out.put(anch::json::OBJECT_END);
  }

  template<typename T>
  inline void
  deserialize(T& value,
	      anch::json::ReaderContext& context,
	      anch::json::DeserializeFn<T> deserializeFunc) {
    if(anch::json::objectHasValueLex(context)) {
       std::invoke(deserializeFunc, value, context);
    }
  }

  template<typename T>
  inline void
  deserialize(T* value,
	      anch::json::ReaderContext& context,
	      anch::json::DeserializeFn<T> deserializeFunc) {
    if(anch::json::objectHasValueLex(context)) {
      value = new T();
      std::invoke(deserializeFunc, *value, context);
    } else {
      value = NULL;
    }
  }

  template<typename T>
  inline void
  deserialize(std::optional<T>& value,
	      anch::json::ReaderContext& context,
	      anch::json::DeserializeFn<T> deserializeFunc) {
    if(anch::json::objectHasValueLex(context)) {
      T instance;
      std::invoke(deserializeFunc, *value, context);
      value = std::move(instance);
    } else {
      value.reset();
    }
  }

  /*
  template<typename T>
  bool
  addToVector(std::vector<T>& value, anch::json::DeserializeFn<T> deser, anch::json::ReaderContext& context) {
    T val;
    if(deser(val, context)) {
      value.push_back(val);
    }
    return false;
  }

  template<typename T>
  bool
  addToList(std::list<T>& value, anch::json::DeserializeFn<T> deser, anch::json::ReaderContext& context) {
    T val;
    if(deser(val, context)) {
      value.push_back(val);
    }
    return false;
  }

  template<typename T>
  bool
  addToSet(std::set<T>& value, anch::json::DeserializeFn<T> deser, anch::json::ReaderContext& context) {
    T val;
    if(deser(val, context)) {
      value.insert(val);
    }
    return false;
  }
  */

}  // anch::json
