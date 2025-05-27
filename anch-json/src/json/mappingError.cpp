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
computeErrorMessage(ErrorCode code, const anch::json::ReaderContext& context) {
  const int32_t offset = static_cast<int32_t>(context.offset);
  int32_t begin = offset - 10;
  if(begin < 0) {
    begin = 0;
  }
  std::ostringstream oss;
  switch(code) {
  case ErrorCode::INVALID_FORMAT:
    oss << "Unexpected character '" << context.buffer[context.offset - 1] << "' found at " << (offset - begin -1);
    break;
  case ErrorCode::UNEXPECTED_FIELD:
    oss << "Unexpected field found";
    break;
  case ErrorCode::POTENTIAL_OVERFLOW:
    oss << "Potential overflow";
    break;
  default:
    oss << "Unexpected error found at " << context.offset;
  }
  // Try to get 20 characters around current error +
  oss << " near '";
  for(int32_t i = begin ; i < offset ; ++i) {
    oss << context.buffer[i];
  }
  for(int32_t i = offset ; i < 11 && offset + i < static_cast<int32_t>(context.bufferSize) ; ++i) {
    char c = context.buffer[i];
    if(c == '\0') {
      break;
    }
    oss << c;
  }
  // Try to get 20 characters around current error -
  oss << '\'';
  return oss.str();
}

// Constructors +
MappingError::MappingError(ErrorCode code, const anch::json::ReaderContext& context): std::exception(), _code(code) {
  _msg = computeErrorMessage(code, context);
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
