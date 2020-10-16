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
    void serialize(const T& value, std::ostream& out) {
      JSONFactory<T>::getInstance().serialize(value, out);
    }

    template<typename T>
    void serialize(const std::vector<T>& value, std::ostream& out) {
      JSONFactory<T>::getInstance().serialize(value, out);
    }

    template<typename T>
    void serialize(const std::list<T>& value, std::ostream& out) {
      JSONFactory<T>::getInstance().serialize(value, out);
    }

    template<typename T>
    void serialize(const std::set<T>& value, std::ostream& out) {
      JSONFactory<T>::getInstance().serialize(value, out);
    }

    template<typename T>
    std::string serialize(const T& value) {
      std::ostringstream out;
      JSONFactory<T>::getInstance().serialize(value, out);
      return std::move(out.str());
    }

    template<typename T>
    std::string serialize(const std::vector<T>& value) {
      std::ostringstream out;
      JSONFactory<T>::getInstance().serialize(value, out);
      return std::move(out.str());
    }

    template<typename T>
    std::string serialize(const std::list<T>& value) {
      std::ostringstream out;
      JSONFactory<T>::getInstance().serialize(value, out);
      return std::move(out.str());
    }

    template<typename T>
    std::string serialize(const std::set<T>& value) {
      std::ostringstream out;
      JSONFactory<T>::getInstance().serialize(value, out);
      return std::move(out.str());
    }

    template<typename T>
    void deserialize(T& value, std::istream& input) {
      JSONFactory<T>::getInstance().deserialize(value, input);
    }

    template<typename T>
    T deserialize(std::istream& input) {
      T value;
      JSONFactory<T>::getInstance().deserialize(value, input);
      return value;
    }

  }
}
