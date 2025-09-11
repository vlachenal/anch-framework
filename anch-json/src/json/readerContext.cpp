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
#include "json/readerContext.hpp"

#include <cstring>

#include "json/lexer.hpp"

using anch::json::ReaderContext;

void
checkPayloadSize(ReaderContext& context, std::streamsize read) {
  context.read += static_cast<std::size_t>(read);
  if(context.read > context.options.max_payload) {
    throw anch::json::MappingError(anch::json::ErrorCode::POTENTIAL_OVERFLOW, context);
  }
}

void
notCheckPayloadSize([[ maybe_unused ]] ReaderContext& context,
		    [[ maybe_unused ]] std::streamsize read) {
  // Nothing to do
}

// Constructors +
ReaderContext::ReaderContext(std::istream& is, const anch::json::MappingOptions& mapOpts) noexcept:
  input(is),
  options(mapOpts),
  pbuf(),
  offset(0),
  bufferSize(0),
  read(0) {
  // Initialize buffers +
  pbuf.reserve(options.buffer_size);
  buffer = new char[options.buffer_size + 1];
  // Initialize buffers -

  // Input stream initial read +
  input.read(buffer, static_cast<std::streamsize>(options.buffer_size));
  auto count = input.gcount();
  std::memset(buffer + static_cast<int>(count), '\0', options.buffer_size - static_cast<std::size_t>(count) + 1);
  read += static_cast<std::size_t>(count);
  // Input stream initial read -

  // Determine lexer functions according to mapping options +
  if(options.deserialize_max_discard_char == 0) {
    _discardFunc = std::bind(&anch::json::discardFail, std::ref(*this));
  } else {
    _discardFunc = std::bind(&anch::json::lexDiscard, std::ref(*this));
  }
  if(options.deserialize_ignore_unknown_field) {
    _unknownFunc = std::bind(&anch::json::lexUnknown, std::ref(*this));
  } else {
    _unknownFunc = std::bind(&anch::json::lexUnknownFail, std::ref(*this));
  }
  if(options.max_payload != 0) {
    _checkSize = std::bind_front(&checkPayloadSize, std::ref(*this));
  } else {
    _checkSize = std::bind_front(&notCheckPayloadSize, std::ref(*this));
  }
  // Determine lexer functions according to mapping options -
}
// Constructors -

// Destructor +
ReaderContext::~ReaderContext() {
  if(buffer != NULL) {
    delete[] buffer;
  }
}
// Destructor -

// Methods +
bool
ReaderContext::next(char& current) {
  // Consume input stream in buffer when buffer end has been reached and input is available +
  if(offset == options.buffer_size) {
    refillBuffer();
  }
  // Consume input stream in buffer when buffer end has been reached and input is available -
  current = buffer[offset++];
  return (offset == options.buffer_size);
}

std::streamsize
ReaderContext::refillBuffer() {
  std::streamsize count = 0;
  if(input) {
    input.read(buffer, static_cast<std::streamsize>(options.buffer_size));
    count = input.gcount();
    read += static_cast<std::size_t>(count);
    std::memset(buffer + static_cast<int>(count), '\0', options.buffer_size - static_cast<std::size_t>(count));
  } else {
    std::memset(buffer, '\0', options.buffer_size);
  }
  offset = 0; // reset offset
  return count;
}
// Methods -
