/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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
#include "csv/csvError.hpp"

using anch::csv::CsvError;
using anch::csv::ErrorCode;

namespace {

  std::string
  format(const std::string& message, std::size_t line, std::size_t column) {
    std::string res = "line " + std::to_string(line);
    if(column != 0) {
      res += ", column " + std::to_string(column);
    }
    return res + ": " + message;
  }

}

CsvError::CsvError(ErrorCode code, const std::string& message, std::size_t line, std::size_t column):
  std::runtime_error(format(message, line, column)), _code(code), _line(line), _column(column) {
  // Nothing to do
}

ErrorCode
CsvError::getErrorCode() const noexcept {
  return _code;
}

std::size_t
CsvError::getLine() const noexcept {
  return _line;
}

std::size_t
CsvError::getColumn() const noexcept {
  return _column;
}
