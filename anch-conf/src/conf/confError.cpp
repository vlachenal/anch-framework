/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "conf/confError.hpp"

#include <sstream>

using anch::conf::ConfError;


ConfError::ConfError(const std::string& msg, ConfError::ErrorCode code): _code(code) {
  std::ostringstream oss;
  switch(_code) {
  case ConfError::ErrorCode::NOT_READABLE:
    oss << "NOT_READABLE: ";
    break;
  case ConfError::ErrorCode::PARSING_ERROR:
    oss << "PARSING_ERROR: ";
    break;
  case ConfError::ErrorCode::CONF_NOT_FOUND:
    oss << "CONF_NOT_FOUND: ";
    break;
  case ConfError::ErrorCode::RES_NOT_FOUND:
    oss << "RES_NOT_FOUND: ";
    break;
  case ConfError::ErrorCode::RES_PROTECTED:
    oss << "RES_PROTECTED: ";
    break;
  case ConfError::ErrorCode::NOT_RES:
    oss << "NOT_RES: ";
    break;
  case ConfError::ErrorCode::PARSER_NOT_FOUND:
    oss << "PARSER_NOT_FOUND: ";
    break;
  case ConfError::ErrorCode::PARSER_PROTECTED:
    oss << "PARSER_PROTECTED: ";
    break;
  case ConfError::ErrorCode::NOT_LOADED:
    oss << "NOT_LOADED: ";
    break;
  case ConfError::ErrorCode::CYCLE:
    oss << "CYCLE: ";
    break;
  default:
    oss << "UNKNOWN: ";
  }
  oss << msg;
  _msg = oss.str();
}

ConfError::~ConfError() noexcept {
  // Nothing to do
}
