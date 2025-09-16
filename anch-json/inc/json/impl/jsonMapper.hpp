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

#include "json/factory.hpp"

namespace anch::json {

  // Serialization methods +
  template<typename T>
  inline
  void
  JSONMapper::serialize(const T& value, std::ostream& out) {
    anch::json::serialize(value, out, _options);
  }

  template<template<typename> typename C, typename T>
  inline
  void
  JSONMapper::serialize(const C<T>& value, std::ostream& out) {
    anch::json::serialize<C,T>(value, out, _options);
  }

  template<typename T>
  inline
  void
  JSONMapper::serialize(anch::Flux<T>& value, std::ostream& out) {
    anch::json::serialize(value, out, _options);
  }

  template<typename T>
  inline
  void
  JSONMapper::serialize(const std::map<std::string,T>& value, std::ostream& out) {
    anch::json::serialize(value, out, _options);
  }

  template<typename T>
  inline
  std::string
  JSONMapper::serialize(const T& value) {
    return anch::json::serialize(value, _options);
  }

  template<template<typename> typename C, typename T>
  inline
  std::string
  JSONMapper::serialize(const C<T>& value) {
    return anch::json::serialize<C,T>(value, _options);
  }

  template<typename T>
  inline
  std::string
  JSONMapper::serialize(const std::map<std::string,T>& value) {
    return anch::json::serialize(value, _options);
  }
  // Serialization methods -

  // Deserialization methods +
  template<typename T>
  inline
  void
  JSONMapper::deserialize(T& value, std::istream& input) {
    anch::json::deserialize(value, input, _options);
  }

  template<typename T>
  inline
  T
  JSONMapper::deserialize(std::istream& input) {
    return anch::json::deserialize<T>(input, _options);
  }

  template<template<typename> typename C, typename T>
  inline
  void
  JSONMapper::deserialize(C<T>& values, std::istream& input) {
    anch::json::deserialize<C,T>(values, input, _options);
  }

  template<typename T>
  inline
  void
  JSONMapper::deserialize(std::map<std::string,T>& values, std::istream& input) {
    anch::json::deserialize(values, input, _options);
  }
  // Deserialization methods -

}
