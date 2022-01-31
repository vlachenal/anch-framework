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

#include <sstream>

#include "rest/mapperException.hpp"
#include "json/json.hpp"
#include "rest/constants.hpp"


namespace anch::rest {

  /*auto& castMapper(const std::string& contentType, std::any& mapper) {
    if(contentType == anch::rest::MediaType::APP_JSON) {
      return std::any_cast<anch::json::JSONMapper&>(mapper);
    }
    std::ostringstream out;
    out << contentType << " has not been defined in mapper caster function";
    throw anch::rest::MapperException(out.str());
    }*/

  template<typename T>
  void
  MapperRegistry::registerMapper(const std::string& contentType, const T& mapper) {
    _mappers[contentType] = std::make_any<T>(mapper);
  }

  inline
  std::string
  formatNotRegiteredError(const std::string& contentType) {
    std::ostringstream out;
    out << contentType << " has not been registered";
    return out.str();
  }

  template<typename T>
  void
  MapperRegistry::serialize(const std::string& contentType, T value, std::ostream& out) {
    auto iter = _mappers.find(contentType);
    if(iter == _mappers.end()) {
      throw anch::rest::MapperException(anch::rest::formatNotRegiteredError(contentType));
    }
    castMapper(contentType, iter->second).serialize(value, out);
  }

  template<typename T>
  void
  MapperRegistry::deserialize(const std::string& contentType, T& value, std::istream& input) {
    auto iter = _mappers.find(contentType);
    if(iter == _mappers.end()) {
      throw anch::rest::MapperException(anch::rest::formatNotRegiteredError(contentType));
    }
    castMapper(contentType, iter->second).deserialize(value, input);
  }

  template<typename T>
  std::optional<T>
  MapperRegistry::mapper(const std::string& contentType) {
    auto iter = _mappers.find(contentType);
    if(iter == _mappers.end()) {
      return std::optional<T>();
    } else {
      return std::optional<T>(std::any_cast<T>(iter->second));
    }
  }

}
