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
#include "logger/levels.hpp"

using anch::logger::Level;

const std::map<const Level, const std::string> anch::logger::LEVEL_LABEL = {
  {Level::TRACE, "TRACE"},
  {Level::DEBUG, "DEBUG"},
  {Level::INFO, "INFO"},
  {Level::WARN, "WARN"},
  {Level::ERROR, "ERROR"},
  {Level::FATAL, "FATAL"},
};

const std::map<const std::string, const anch::logger::Level> anch::logger::LABEL_LEVEL = {
  {"TRACE", Level::TRACE},
  {"DEBUG", Level::DEBUG},
  {"INFO", Level::INFO},
  {"WARN", Level::WARN},
  {"ERROR", Level::ERROR},
  {"FATAL", Level::FATAL},
};
