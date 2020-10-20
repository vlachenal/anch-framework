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
#include "json/mappingError.hpp"

#include <sstream>

using anch::json::MappingError;
using anch::json::ErrorCode;


std::string
computeErrorMessage(ErrorCode code, std::istream& input, std::optional<std::string> context) {
  std::streamoff pos = input.tellg();
  std::ostringstream oss;
  switch(code) {
  case ErrorCode::INVALID_FORMAT:
    oss << "Unexpected character found at " << pos;
    break;
  case ErrorCode::UNEXPECTED_FIELD:
    oss << "Unexpected field found at " << pos;
    break;
  case ErrorCode::POTENTIAL_OVERFLOW:
    return "HACKER !!!";
  default:
    oss << "Unexpected error found at " << pos;
  }
  if(context.has_value()) {
    oss << " for '" << context.value() << '\'';
  }
  // Try to get 20 characters around current error +
  oss << " near '";
  std::streamoff begin = pos - 10;
  if(begin < 0) {
    begin = 0;
  }
  for(std::streamoff i = begin ; i < pos ; ++i) {
    try {
      input.seekg(i);
    } catch(...) {
      continue;
    }
    oss << static_cast<char>(input.peek());
  }
  input.seekg(pos);
  for(std::streamoff i = 0 ; i < 10 ; ++i) {
    try {
      input.seekg(pos + i);
    } catch(...) {
      break;
    }
    oss << static_cast<char>(input.peek());
  }
  // Try to get 20 characters around current error -
  oss << '\'';
  return std::move(oss.str());
}

// Constructors +
MappingError::MappingError(ErrorCode code, std::istream& input, std::optional<std::string> context): std::exception(), _code(code) {
  _msg = computeErrorMessage(code, input, context);
}

MappingError::MappingError(ErrorCode code, std::istream& input, char context): std::exception(), _code(code) {
  std::ostringstream oss;
  oss << context;
  _msg = computeErrorMessage(code, input, std::optional<std::string>(oss.str()));
}
// Constructors -

// Destructor +
MappingError::~MappingError() {
  // Nothing to do
}
// Destructor -

// Methods +
const char*
MappingError::what() const noexcept {
  return _msg.data();
}
// Methods -
