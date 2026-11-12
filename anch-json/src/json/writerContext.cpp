/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "json/writerContext.hpp"

#include "json/constants.hpp"

using anch::json::WriterContext;


// Constructors +
WriterContext::WriterContext(std::ostream& os, const anch::json::MappingOptions& options) noexcept:
  output(os), _options(options), _firsts(), _sbuf(os.rdbuf()) {
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
    write("null");
  }
  return _options.serialize_null;
}

bool
WriterContext::writeEmptyArray(const std::string& field) {
  if(_options.serialize_empty_col) {
    writeField(field);
    write(anch::json::ARRAY_BEGIN);
    write(anch::json::ARRAY_END);
  }
  return _options.serialize_empty_col;
}

bool
WriterContext::writeEmptyObject(const std::string& field) {
  if(_options.serialize_empty_col) {
    writeField(field);
    write(anch::json::OBJECT_BEGIN);
    write(anch::json::OBJECT_END);
  }
  return _options.serialize_empty_col;
}

void
WriterContext::indent() {
  write('\n');
  for(uint32_t i = 0 ; i < _firsts.size() * _options.prettify_nbs ; ++i) {
    write(' ');
  }
}

void
WriterContext::writeEscapeSequence(char c) {
  switch(c) {
  case '"':
    write("\\\"");
    break;
  case '\\':
    write("\\\\");
    break;
  case '\b':
    write("\\b");
    break;
  case '\f':
    write("\\f");
    break;
  case '\n':
    write("\\n");
    break;
  case '\r':
    write("\\r");
    break;
  case '\t':
    write("\\t");
    break;
  default: // other control characters
    {
      static constexpr char HEX[] = "0123456789abcdef";
      unsigned char uc = static_cast<unsigned char>(c);
      char seq[6] = {'\\', 'u', '0', '0', HEX[uc >> 4], HEX[uc & 0x0F]};
      write(std::string_view(seq, sizeof(seq)));
    }
  }
}
// Methods -
