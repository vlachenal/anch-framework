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
#include "json/reader.hpp"

#include <charconv>
#include <cstring>

#include "json/lexer.hpp"
#include "json/constants.hpp"
#include "json/mappingError.hpp"


using anch::json::EventType;
using anch::json::JSONItem;
using anch::json::Reader;
using anch::json::ErrorCode;
using anch::json::MappingError;


//JSONItem +
JSONItem::JSONItem(EventType type, anch::json::ReaderContext& context): _type(type), _value() {
  switch(type) {
  case EventType::FIELD:
  case EventType::STRING:
    _value = context.pbuf;
    break;
  case EventType::NUMBER:
    {
      double value;
      char* last = context.pbuf.data() + context.pbuf.size();
      auto res = std::from_chars(context.pbuf.data(), last, value);
      if(res.ptr != last) {
	if(res.ec == std::errc::result_out_of_range) {
	  throw anch::json::MappingError(ErrorCode::POTENTIAL_OVERFLOW, context);
	} else {
	  throw anch::json::MappingError(ErrorCode::INVALID_FORMAT, context);
	}
      }
      _value = value;
    }
    break;
  default:
    // Nothing to do
    break;
  }
}

JSONItem::~JSONItem() noexcept {
  // Nothing to do
}
//JSONItem -


// Reader +
Reader::Reader(std::istream& is, const anch::json::MappingOptions& mappingOptions):
  anch::events::Observable<anch::json::JSONItem>(),
  anch::events::Observable<std::streamsize>(),
  anch::events::Observable<anch::json::MappingError>(),
  anch::json::ReaderContext(is, mappingOptions) {
  // Nothing to do
}

Reader::~Reader() {
  // Nothing to do
}

void
Reader::parse() noexcept {
  try {
    switch(discard()) {
    case anch::json::OBJECT_BEGIN:
      anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::BEGIN_OBJECT, *this));
      parseObject();
      break;
    case anch::json::ARRAY_BEGIN:
      anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::BEGIN_ARRAY, *this));
      parseArray();
      break;
    default:
      throw MappingError(ErrorCode::UNEXPECTED_FIELD, *this);
    }

  } catch(const MappingError& e) {
    try {
      std::memset(buffer, '\0', bufferSize);
      offset = 0;
      anch::events::Observable<MappingError>::notifyObservers(e);
    } catch(...) {
      // Nothing to do
    }

  } catch(...) {
    try {
      MappingError error = MappingError(ErrorCode::UNKNOWN, *this);
      std::memset(buffer, '\0', bufferSize);
      offset = 0;
      anch::events::Observable<MappingError>::notifyObservers(error);
    } catch(...) {
      // Nothing to do
    }
  }
}

std::streamsize
Reader::refillBuffer() {
  std::streamsize read = anch::json::ReaderContext::refillBuffer();
  anch::events::Observable<std::streamsize>::notifyObservers(read);
  return read;
}

void
Reader::parseField() {
  if(discard() != anch::json::STRING_DELIMITER) {
    throw MappingError(ErrorCode::UNEXPECTED_FIELD, *this);
  }
  anch::json::lexStringContent(*this);
  anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::FIELD, *this));
}

void
Reader::parseObject() {
  char current = discard();
  if(current == anch::json::OBJECT_END) {
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::END_OBJECT, *this));
    return;
  }
  --offset;
  do {
    parseField();
    current = discard();
    if(current != anch::json::FIELD_VALUE_SEPARATOR) {
      throw MappingError(ErrorCode::UNEXPECTED_FIELD, *this);
    }
    parseValue();
    current = discard();
    if(current == anch::json::FIELD_SEPARATOR) {
      continue;
    }
    if(current == anch::json::OBJECT_END) {
      anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::END_OBJECT, *this));
      break;
    }
    throw MappingError(ErrorCode::UNEXPECTED_FIELD, *this);
  } while(true);
}

void
Reader::parseValue() {
  char current = discard();
  switch(current) {
  case 'n':
    anch::json::lexNull(*this);
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::VNULL, *this));
    break;
  case 't':
    anch::json::lexTrue(*this);
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::TRUE, *this));
    break;
  case 'f':
    anch::json::lexFalse(*this);
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::FALSE, *this));
    break;
  case anch::json::STRING_DELIMITER:
    anch::json::lexStringContent(*this);
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::STRING, *this));
    break;
  case anch::json::ARRAY_BEGIN:
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::BEGIN_ARRAY, *this));
    parseArray();
    break;
  case anch::json::OBJECT_BEGIN:
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::BEGIN_OBJECT, *this));
    parseObject();
    break;
  case '-':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    back();
    anch::json::parseNumber(*this);
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::NUMBER, *this));
    break;
  default:
    throw MappingError(ErrorCode::UNEXPECTED_FIELD, *this);
  }
}

void
Reader::parseArray() {
  char current = discard();
  if(current == anch::json::ARRAY_END) {
    anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::END_ARRAY, *this));
    return;
  }
  --offset;
  do {
    parseValue();
    current = discard();
    if(current == anch::json::FIELD_SEPARATOR) {
      continue;
    }
    if(current == anch::json::ARRAY_END) {
      anch::events::Observable<JSONItem>::notifyObservers(JSONItem(EventType::END_ARRAY, *this));
      break;
    }
  } while(true);
}
// Reader -
