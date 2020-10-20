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


namespace anch {
  namespace json {

    inline bool isDiscardChar(int car) {
      return car == ' ' || car == '\t' || car == '\n' || car == '\r';
    }

    void
    discardChars(std::istream& input) {
      // \todo manage max discard char ...
      //int discarded = 0;
      while(input && anch::json::isDiscardChar(input.peek())) {
	input.get();
	// \todo max char
	//if(++discarded > ANCH_MAX_DISCARD) {
	//  throw 8;
	//}
      }
      if(!input) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input);
      }
    }

    bool
    isNull(std::istream& input) {
      anch::json::discardChars(input);
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
    getFieldName(std::istream& input) {
      anch::json::discardChars(input);
      std::optional<std::string> name;
      int current = input.peek();
      if(current != anch::json::STRING_DELIMITER) {
	return name;
      }
      input.get();
      std::ostringstream buffer;
      while(input) {
	current = input.get();
	if(current == anch::json::STRING_DELIMITER) {
	  break;
	}
	buffer << static_cast<char>(current);
      }
      anch::json::discardChars(input);
      if(input.get() != anch::json::FIELD_VALUE_SEPARATOR) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(input.peek()));
      }
      name = buffer.str();
      return name;
    }


    bool
    hasMoreField(std::istream& input) {
      anch::json::discardChars(input);
      if(input.peek() == anch::json::FIELD_SEPARATOR) {
	input.get();
	return true;
      } else {
	return false;
      }
    }

    void
    parseField(std::istream& input) {
      discardChars(input);
      int current = input.get();
      if(current != '"') {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
      }
      std::ostringstream value;
      while(input) {
	current = input.get();
	if(current == '\\') {
	  if(!input) {
	    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input);
	  }
	  value << input.get();
	} else if(current == '"') {
	  break;
	} else {
	  value << current;
	}
      }
    }

    void
    parseFields(std::istream& input) {
      discardChars(input);

      int current = input.peek();
      if(current == '}') {
	return;
      }
      input.get();
      while(input) {
	std::ostringstream name;
	if(current != '"') {
	  throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
	}
	while(input) {
	  current = input.get();
	  if(current == '"') {
	    break;
	  }
	  name << current;
	}

	discardChars(input);
	if(input.get() != ':') {
	  throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(input.peek()));
	}
	parseField(input);
	discardChars(input);
	if(input.peek() != ',') {
	  break;
	}
	input.get(); // consumes ','
	current = input.get();
      }
    }

    template<typename T>
    std::optional<T> parse(std::istream& input) {
      discardChars(input);

      // fetch '{'
      std::optional<T> obj;
      char nullChar = 'n';
      uint16_t nullBuf = 0;
      char current = 0x0;
      bool nullable = false;
      while(input) {
	current = input.get();

	if(!nullable && isDiscardChar(current)) {
	  continue;
	}

	if(current == nullChar) {
	  switch(nullBuf) {
	  case 0:
	    nullable = true;
	    nullChar = 'u';
	    break;
	  case 1:
	    nullChar = 'l';
	    break;
	  case 2:
	    break; // wait for 'l'
	  case 3:
	    goto stopobj; // stop parsing for this object
	  default:
	    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(nullBuf));
	  }
	  ++nullBuf;

	} else if(current == '{') { // start object
	  T instance;
	  obj = std::move(instance);
	  parseFields(input);

	} else if(current == '}') { // end object
	  if(!obj.has_value()) {
	    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
	  }
	  break;

	} else {
	  throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
	}

      }

    stopobj:
      return std::move(obj);
    }

  }  // json
}  // anch
