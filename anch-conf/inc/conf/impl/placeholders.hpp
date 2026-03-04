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

#include <vector>
#include <optional>

namespace anch::conf {

  /*!
   * \brief Event when configuration's value has been resolved
   *
   * Declare anch::conf::ConfEvt which is the event that will be send when configuration has been resolved.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ResConfEvt {

    /*! Configuration path */
    std::string conf;

    /*! Value */
    std::string val;

  };

  /*!
   * Configuration value placeholders
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ConfPlaceholders {

    /*! Configuration path */
    std::string path;

    /*! Placeholders for this configuration */
    std::map<std::string, std::vector<anch::conf::SubPlaceholder>> placeholders;

    /*! Placeholders has been resolved flag */
    bool resolved = false;

  };

  inline
  bool
  Placeholders::collect() {
    collect(_section);
    return !_values.empty();
  }

}
