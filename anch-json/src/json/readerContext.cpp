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

// Constructors +
ReaderContext::ReaderContext(std::istream& is, const anch::json::MappingOptions& mapOpts):
  input(is),
  options(mapOpts),
  pbuf(),
  offset(0),
  bufferSize(mapOpts.buffer_size),
  read(0),
  depth(0) {
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

  // Check payload size +
  // Destructor is not called when constructor fails: release buffer before raising error
  try {
    checkPayloadSize();
  } catch(...) {
    delete[] buffer;
    buffer = NULL;
    throw;
  }
  // Check payload size -
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
std::streamsize
ReaderContext::refillBuffer() {
  std::streamsize count = 0;
  if(input) {
    input.read(buffer, static_cast<std::streamsize>(options.buffer_size));
    count = input.gcount();
    read += static_cast<std::size_t>(count);
    std::memset(buffer + static_cast<int>(count), '\0', options.buffer_size - static_cast<std::size_t>(count));
    checkPayloadSize();
  } else {
    std::memset(buffer, '\0', options.buffer_size);
  }
  offset = 0; // reset offset
  return count;
}

void
ReaderContext::checkPayloadSize() const {
  if(options.max_payload != 0 && read > options.max_payload) {
    throw anch::json::MappingError(anch::json::ErrorCode::POTENTIAL_OVERFLOW, *this);
  }
}

void
ReaderContext::throwDepthError() {
  --depth; // level has not been entered
  throw anch::json::MappingError(anch::json::ErrorCode::POTENTIAL_OVERFLOW, *this);
}
// Methods -
