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
#include "json/parser.hpp"

#include <istream>
#include <sstream>
#include <optional>

#include "json/constants.hpp"

#include <iostream> // \todo remove


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
	std::cout << "Remove left 'spaces'" << std::endl;
	input.get();
	//if(++discarded > ANCH_MAX_DISCARD) {
	//  throw 8;
	//}
      }
      if(!input) {
	throw 0; // \todo raise another error ...
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
	  throw 4; // \todo raise another error ...
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
	throw 256; // \todo raise another error ...
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
      // \todo parse field
      //char expected = '"';
      int current = input.get();
      if(current != '"') {
	throw 2;
      }
      std::ostringstream value;
      while(input) {
	current = input.get();
	if(current == '\\') {
	  if(!input) {
	    throw 0;
	  }
	  value << input.get();
	} else if(current == '"') {
	  break;
	} else {
	  value << current;
	}
      }
      std::cout << "Found value " << value.str() << std::endl;
    }

    void
    parseFields(std::istream& input) {
      discardChars(input);

      // \todo parse field
      // look for field name
      //char expected = '"';
      int current = input.peek();
      if(current == '}') {
	return;
      }
      input.get();
      while(input) {
	std::ostringstream name;
	if(current != '"') {
	  std::cerr << "Expected \" ; found " << current << std::endl;
	}
	std::cout << "Start field name" << std::endl;
	while(input) {
	  current = input.get();
	  if(current == '"') {
	    std::cout << "End field name" << std::endl;
	    break;
	  }
	  name << current;
	}
	std::cout << "Found field name " << name.str() << std::endl;

	discardChars(input);
	if(input.get() != ':') {
	  throw 256;
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
	  std::cout << "Discard character" << std::endl;
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
	    std::cout << "Found null" << std::endl;
	    goto stopobj; // stop parsing for this object
	  default:
	    throw 1024;
	  }
	  ++nullBuf;

	} else if(current == '{') { // start object
	  T instance;
	  obj = std::move(instance);
	  std::cout << "Start object" << std::endl;
	  parseFields(input);

	} else if(current == '}') { // end object
	  std::cout << "End object" << std::endl;
	  if(!obj.has_value()) {
	    throw 512;
	  }
	  break;

	} else {
	  throw 2048;
	}

      }

    stopobj:
      return std::move(obj);
    }

  }  // json
}  // anch
