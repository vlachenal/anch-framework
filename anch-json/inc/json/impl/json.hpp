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

#include "json/factory.hpp"

namespace anch {
  namespace json {

    template<typename T>
    inline
    void serialize(const T& value, std::ostream& out, const anch::json::MappingOptions& options) {
      anch::json::Factory<T>::getInstance().serialize(value, out, options);
    }

    template<typename T>
    inline
    void serialize(const std::vector<T>& value, std::ostream& out, const anch::json::MappingOptions& options) {
      anch::json::Factory<T>::getInstance().serialize(value, out, options);
    }

    template<typename T>
    inline
    void serialize(const std::list<T>& value, std::ostream& out, const anch::json::MappingOptions& options) {
      anch::json::Factory<T>::getInstance().serialize(value, out, options);
    }

    template<typename T>
    inline
    void serialize(const std::set<T>& value, std::ostream& out, const anch::json::MappingOptions& options) {
      anch::json::Factory<T>::getInstance().serialize(value, out, options);
    }

    template<typename T>
    inline
    std::string serialize(const T& value, const anch::json::MappingOptions& options) {
      std::ostringstream out;
      anch::json::Factory<T>::getInstance().serialize(value, out, options);
      return std::move(out.str());
    }

    template<typename T>
    inline
    std::string serialize(const std::vector<T>& value, const anch::json::MappingOptions& options) {
      std::ostringstream out;
      anch::json::Factory<T>::getInstance().serialize(value, out, options);
      return std::move(out.str());
    }

    template<typename T>
    inline
    std::string serialize(const std::list<T>& value, const anch::json::MappingOptions& options) {
      std::ostringstream out;
      anch::json::Factory<T>::getInstance().serialize(value, out, options);
      return std::move(out.str());
    }

    template<typename T>
    inline
    std::string serialize(const std::set<T>& value, const anch::json::MappingOptions& options) {
      std::ostringstream out;
      anch::json::Factory<T>::getInstance().serialize(value, out, options);
      return std::move(out.str());
    }

    template<typename T>
    inline
    void deserialize(T& value, std::istream& input, const anch::json::MappingOptions& options) {
      anch::json::Factory<T>::getInstance().deserialize(value, input, options);
    }

    template<typename T>
    inline
    T deserialize(std::istream& input, const anch::json::MappingOptions& options) {
      T value;
      anch::json::Factory<T>::getInstance().deserialize(value, input, options);
      return value;
    }

    template<typename T>
    inline
    void deserialize(std::vector<T>& values, std::istream& input, const anch::json::MappingOptions& options) {
      anch::json::Factory<T>::getInstance().deserialize(values, input, options);
    }

    template<typename T>
    inline
    void deserialize(std::list<T>& values, std::istream& input, const anch::json::MappingOptions& options) {
      anch::json::Factory<T>::getInstance().deserialize(values, input, options);
    }

    template<typename T>
    inline
    void deserialize(std::set<T>& values, std::istream& input, const anch::json::MappingOptions& options) {
      anch::json::Factory<T>::getInstance().deserialize(values, input, options);
    }

  }
}
