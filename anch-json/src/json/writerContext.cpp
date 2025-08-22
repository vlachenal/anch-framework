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
#include "json/writerContext.hpp"

#include "json/constants.hpp"

using anch::json::WriterContext;


// Constructors +
WriterContext::WriterContext(std::ostream& os, const anch::json::MappingOptions& options) noexcept:
  output(os), _options(options), _firsts() {
  // set output stream formatter for integer, float and boolean
  output << std::dec << std::defaultfloat << std::boolalpha;
}
// Constructors -

// Destructor +
WriterContext::~WriterContext() noexcept {
  output.flush();
}
// Destructor -

// Methods +
bool
WriterContext::writeNull(const std::string& field) {
  if(_options.serialize_null) {
    writeField(field);
    output.write("null", 4);
  }
  return _options.serialize_null;
}

bool
WriterContext::writeEmptyArray(const std::string& field) {
  if(_options.serialize_empty_col) {
    writeField(field);
    output.put(anch::json::ARRAY_BEGIN).put(anch::json::ARRAY_END);
  }
  return _options.serialize_empty_col;
}

bool
WriterContext::writeEmptyObject(const std::string& field) {
  if(_options.serialize_empty_col) {
    writeField(field);
    output.put(anch::json::OBJECT_BEGIN).put(anch::json::OBJECT_END);
  }
  return _options.serialize_empty_col;
}

void
WriterContext::writeField(const std::string& field) {
  next();
  output.put(anch::json::STRING_DELIMITER)
    .write(field.data(), static_cast<std::streamsize>(field.size()))
    .put(anch::json::STRING_DELIMITER)
    .put(anch::json::FIELD_VALUE_SEPARATOR);
}

void
WriterContext::beginArray() {
  output.put(anch::json::ARRAY_BEGIN);
  _firsts.push_back(true);
}

void
WriterContext::endArray() {
  output.put(anch::json::ARRAY_END);
  _firsts.pop_back();
}

void
WriterContext::beginObject() {
  output.put(anch::json::OBJECT_BEGIN);
  _firsts.push_back(true);
}

void
WriterContext::endObject() {
  output.put(anch::json::OBJECT_END);
  _firsts.pop_back();
}

void
WriterContext::next() {
  if(!_firsts.back()) {
    output.put(anch::json::FIELD_SEPARATOR);
  } else {
    _firsts.back() = false;
  }
  if(_options.prettify_nbs != 0) {
    output.put('\n');
    for(uint32_t i = 0 ; i < _firsts.size() * _options.prettify_nbs ; ++i) {
      output.put(' ');
    }
  }
}
// Methods -
