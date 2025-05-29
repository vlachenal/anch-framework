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

#include "json/mappingFunctions.hpp"

namespace anch::json {

  template<typename T>
  bool
  PrimitiveMapper<T>::deserialize(std::optional<T>& value, anch::json::ReaderContext& context) {
    T val;
    if(!deserialize(val, context)) {
      value.reset();
      return false;
    }
    value = std::move(val);
    return true;
  }

  template<typename T>
  bool
  PrimitiveMapper<T>::deserialize(T* value, anch::json::ReaderContext& context) {
    T val;
    if(!deserialize(val, context)) {
      value = NULL;
      return false;
    }
    value = new T();
    *value = std::move(val);
    return true;
  }

  template<typename T>
  bool
  PrimitiveMapper<T>::deserialize(std::vector<T>& value, anch::json::ReaderContext& context) {
    AddItem addFunc = [&](anch::json::ReaderContext& ctxt) -> bool {
      T val;
      if(!deserialize(val, ctxt)) {
	return false;
      }
      value.push_back(std::move(val));
      return true;
    };
    anch::json::lexArray(addFunc, context);
    return true;
  }

  template<typename T>
  bool
  PrimitiveMapper<T>::deserialize(std::list<T>& value, anch::json::ReaderContext& context) {
    AddItem addFunc = [&](anch::json::ReaderContext& ctxt) -> bool {
      T val;
      if(!deserialize(val, ctxt)) {
	return false;
      }
      value.push_back(std::move(val));
      return true;
    };
    anch::json::lexArray(addFunc, context);
    return true;
  }

  template<typename T>
  bool
  PrimitiveMapper<T>::deserialize(std::set<T>& value, anch::json::ReaderContext& context) {
    AddItem addFunc = [&](anch::json::ReaderContext& ctxt) -> bool {
      T val;
      if(!deserialize(val, ctxt)) {
	return false;
      }
      value.insert(std::move(val));
      return true;
    };
    anch::json::lexArray(addFunc, context);
    return true;
  }

  template<typename T>
  bool
  PrimitiveMapper<T>::deserialize(std::map<std::string,T>& value, anch::json::ReaderContext& context) {
    PushItem pushFunc = [&](const std::string& key, anch::json::ReaderContext& ctxt) -> void {
      T val;
      if(deserialize(val, ctxt)) {
	value.insert({key, std::move(val)});
      }
    };
    anch::json::lexMap(pushFunc, context);
    return true;
  }

}
