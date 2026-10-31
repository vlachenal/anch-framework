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
#include <algorithm>

using anch::json::MappingError;
using anch::json::ErrorCode;


std::string
computeErrorMessage(const ErrorCode& code, const anch::json::ReaderContext& context, std::string_view message) {
  int64_t offset = static_cast<int64_t>(context.offset);
  int64_t begin = offset - 10;
  if(begin < 0) {
    begin = 0;
  }
  std::ostringstream oss;
  if(!message.empty()) {
    oss << message;
  } else {
    switch(code) {
    case ErrorCode::INVALID_FORMAT:
      {
	char current = (offset > 0) ? context.buffer[offset - 1] : '\0';
	if(current == '\0') {
	  oss << "Unexpected end of data";
	} else {
	  oss << "Unexpected character '" << current << "' found at " << (offset - begin - 1);
	}
      }
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
  }
  // Try to get 20 characters around current error (10 before, 10 after) +
  oss << " near '";
  for(int64_t i = begin ; i < offset ; ++i) {
    char c = context.buffer[i];
    if(c != '\0') { // ignore end of data marker
      oss << c;
    }
  }
  int64_t end = std::min(offset + 10, static_cast<int64_t>(context.bufferSize));
  for(int64_t i = offset ; i < end ; ++i) {
    char c = context.buffer[i];
    if(c == '\0') { // end of data
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
  _msg = computeErrorMessage(code, context, std::string_view());
}

MappingError::MappingError(ErrorCode code, const anch::json::ReaderContext& context, std::string_view message):
  std::exception(), _code(code) {
  _msg = computeErrorMessage(code, context, message);
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
