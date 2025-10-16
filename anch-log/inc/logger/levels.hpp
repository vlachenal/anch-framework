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

#include <string>
#include <map>

namespace anch::logger {

  /*!
   * Logging levels declaration
   *
   * \author Vincent Lachenal
   */
  enum class Level {
    /*! Trace logging level */
    TRACE = 0,

    /*! Debug logging level */
    DEBUG = 10,

    /*! Information logging level */
    INFO = 20,

    /*! Warning logging level */
    WARN = 30,

    /*! Error logging level */
    ERROR = 40,

    /*! Fatal logging level */
    FATAL = 50
  };

  /*! Level->labels for logging defintion */
  extern const std::map<const Level, const std::string> LEVEL_LABEL;

  /*! Labels->levels for logging definition */
  extern const std::map<const std::string, const Level> LABEL_LEVEL;

}
