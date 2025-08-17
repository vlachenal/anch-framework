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
WriterContext::WriterContext(std::ostream& os, const anch::json::MappingOptions& mapOpts) noexcept:
  output(os), options(mapOpts), _firsts() {
}

WriterContext::WriterContext(const WriterContext& context): output(context.output), options(context.options) {
}
// Constructors -

// Destructor +
WriterContext::~WriterContext() noexcept {
  // Nothing to do
}
// Destructor -

// Methods +
bool
WriterContext::writeNull(const std::string& field) {
  if(!options.serialize_null) {
    return false;
  }
  writeField(field);
  output.write("null", 4);
  return true;
}

bool
WriterContext::writeEmptyArray(const std::string& field) {
  writeField(field);
  output.put(anch::json::ARRAY_BEGIN).put(anch::json::ARRAY_END); // \todo check empty array options
  return true;
}

bool
WriterContext::writeEmptyObject(const std::string& field) {
  writeField(field);
  output.put(anch::json::OBJECT_BEGIN).put(anch::json::OBJECT_END); // \todo check empty array options
  return true;
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
  if(!_firsts.back()) { // \todo manage indentation when needed
    output.put(anch::json::FIELD_SEPARATOR);
  } else {
    _firsts.back() = false;
  }
}
// Methods -
