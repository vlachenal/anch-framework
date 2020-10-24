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
#include "json/mappingFunctions.hpp"

#include <istream>
#include <sstream>
#include <optional>

#include "json/constants.hpp"
#include "json/json.hpp"
#include "json/mapper.hpp"


inline
bool
isDiscardChar(int car) {
  return car == ' ' || car == '\t' || car == '\n' || car == '\r';
}

void
anch::json::discardChars(std::istream& input, const anch::json::MappingOptions& options) {
  int discarded = 0;
  while(input && isDiscardChar(input.peek())) {
    input.get();
    if(options.deserialize_max_discard_char != -1 && ++discarded > options.deserialize_max_discard_char) {
      throw anch::json::MappingError(anch::json::ErrorCode::POTENTIAL_OVERFLOW, input);
    }
  }
  if(!input) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input);
  }
}

bool
anch::json::isNull(std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::discardChars(input, options);
  char nullChar = 'n';
  uint16_t index = 0;
  bool null = false;
  while(input) {
    if(input.peek() == nullChar) {
      switch(index) {
      case 0:
	null = true;
	nullChar = 'u';
	break;
      case 1:
	nullChar = 'l';
	break;
      case 2:
	break; // wait for 'l'
      case 3:
	input.get();
	goto stopnull; // stop parsing for this object
      }
      input.get();
      ++index;
    } else if(null) {
      throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(input.peek()));
    } else {
      break;
    }
  }
 stopnull:
  return null;
}

std::optional<std::string>
anch::json::getFieldName(std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::discardChars(input, options);
  std::optional<std::string> name;
  int current = input.peek();
  if(current != anch::json::STRING_DELIMITER) {
    return name;
  }
  input.get();
  int nbChar = 0;
  std::ostringstream buffer;
  while(input) {
    current = input.get();
    if(current == anch::json::STRING_DELIMITER) {
      break;
    }
    if(options.deserialize_max_field_char != -1 && ++nbChar > options.deserialize_max_field_char) {
      throw anch::json::MappingError(anch::json::ErrorCode::POTENTIAL_OVERFLOW, input);
    }
    buffer << static_cast<char>(current);
  }
  anch::json::discardChars(input, options);
  if(input.get() != anch::json::FIELD_VALUE_SEPARATOR) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(input.peek()));
  }
  name = buffer.str();
  return name;
}

bool
anch::json::hasMoreField(std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::discardChars(input, options);
  if(input.peek() == anch::json::FIELD_SEPARATOR) {
    input.get();
    return true;
  } else {
    return false;
  }
}

struct UnknownObject {
};

template<>
void
anch::json::registerObject([[maybe_unused]] anch::json::ObjectMapper<UnknownObject>& mapper) {
  // Nothing to do
}

void
consumeStringValue(std::istream& input, [[maybe_unused]] const anch::json::MappingOptions& options) {
  int current = input.get();
  while(input) {
    // \todo max char in value ...
    current = input.get();
    if(current == '\\') {
      if(!input) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input);
      }
    } else if(current == anch::json::STRING_DELIMITER) {
      break;
    }
  }
  if(!input && current != anch::json::STRING_DELIMITER) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
  }
}

void
consumeRawValue(std::istream& input, [[maybe_unused]] const anch::json::MappingOptions& options) {
  uint8_t nbChar = 0;
  while(input.get()) {
    if(++nbChar > 20) { // max uint64_t is 18446744073709551615 => 20 chars and max int64_t is -9223372036854775806 => 20 chars
      break;
    }
  }
}

void
anch::json::consumeUnknownField(std::istream& input, const anch::json::MappingOptions& options) {
  anch::json::discardChars(input, options);
  char start = static_cast<char>(input.peek());
  if(start == anch::json::OBJECT_BEGIN) {
    UnknownObject obj;
    anch::json::deserialize(obj, input, options);
  } else if(start == anch::json::ARRAY_BEGIN) {
    // \todo parse array
  } else if(start == anch::json::STRING_DELIMITER) {
    input.get();
    consumeStringValue(input, options);
    // \todo parse string
  } else if(start == anch::json::OBJECT_END
	    || start == anch::json::ARRAY_END
	    || start == anch::json::FIELD_VALUE_SEPARATOR
	    || start == anch::json::FIELD_SEPARATOR) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(input.peek()));
  } else {
    consumeRawValue(input, options);
  }
}
