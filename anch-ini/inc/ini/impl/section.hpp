/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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


namespace anch::ini {

  inline
  const std::map<std::string, Section>&
  Section::getSections() const {
    return _sections;
  }

  inline
  const std::map<std::string, std::string>&
  Section::getValues() const {
    return _values;
  }

  inline
  std::optional<std::string>
  Section::getValue(const std::string& path) const {
    return Section::getValue(path, *this);
  }

  template<typename T>
  inline
  T
  Section::getValue(const std::string& key, const T& defaultValue) const {
    std::optional<T> value = getValue<T>(key);
    if(value.has_value()) {
      return value.value();
    }
    return defaultValue;
  }

}
