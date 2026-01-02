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

#include "conf/confError.hpp"


namespace anch::conf {

  inline
  Configuration&
  Configuration::loader() noexcept {
    static Configuration conf;
    return conf;
  }

  inline
  Configuration&
  Configuration::name(const std::string& name) noexcept {
    if(!_loaded) {
      _name = name;
    }
    return *this;
  }

  inline
  Configuration&
  Configuration::profiles(const std::vector<std::string>& profiles) noexcept {
    if(!_loaded) {
      _profiles = profiles;
    }
    return *this;
  }

  inline
  const Configuration&
  Configuration::inst() {
    Configuration& conf = Configuration::loader();
    if(!conf._loaded) {
      throw anch::conf::ConfError("Configuration has not been loaded yet", ConfError::ErrorCode::NOT_LOADED);
    }
    return conf;
  }

}
