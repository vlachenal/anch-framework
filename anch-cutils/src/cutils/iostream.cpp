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
#include "cutils/iostream.hpp"

#include <stdexcept>

using anch::cutils::CStreambuf;
using anch::cutils::CIStream;
using anch::cutils::COStream;
using anch::cutils::CIOStream;
using anch::cutils::Direction;

namespace {

  bool
  hasDirection(Direction dir, Direction flag) {
    return (static_cast<int>(dir) & static_cast<int>(flag)) != 0;
  }

}

// CStreambuf +
CStreambuf::CStreambuf(const anch::cutils::cbuffer& cbuffer, Direction dir): CStreambuf() {
  setBuffer(cbuffer, dir);
}

CStreambuf::CStreambuf() noexcept:
  std::streambuf(),
  _read(nullptr),
  _write(nullptr),
  _size(0),
  _owned(),
  _in(nullptr),
  _out(nullptr) {
  setg(nullptr, nullptr, nullptr);
  setp(nullptr, nullptr);
}

CStreambuf::~CStreambuf() {
  try {
    flushOutput();
  } catch(...) {
    // Destructor must not throw
  }
}

void
CStreambuf::setBuffer(const anch::cutils::cbuffer& cbuffer, Direction dir) {
  // Check buffer options according to direction +
  const bool in = hasDirection(dir, Direction::IN);
  const bool out = hasDirection(dir, Direction::OUT);
  if(in && cbuffer.read == nullptr) {
    throw std::invalid_argument("input stream and null read function");
  }
  if(out && cbuffer.write == nullptr) {
    throw std::invalid_argument("output stream and null write function");
  }
  if(cbuffer.size == 0) {
    throw std::invalid_argument("buffer size has to be greater than 0");
  }
  // Check buffer options according to direction -

  flushOutput(); // pending data have to be sent with the previous write function

  // Allocate areas which have not been provided +
  std::size_t ownedSize = 0;
  if(cbuffer.data == nullptr) {
    ownedSize = (in ? cbuffer.size : 0) + (out ? cbuffer.size : 0);
  } else if(in && out) {
    ownedSize = cbuffer.size; // provided data is used for reading
  }
  std::unique_ptr<char[]> owned = ownedSize > 0 ? std::make_unique_for_overwrite<char[]>(ownedSize) : nullptr;
  char* in_ = nullptr;
  char* out_ = nullptr;
  if(cbuffer.data == nullptr) {
    in_ = in ? owned.get() : nullptr;
    out_ = out ? owned.get() + (in ? cbuffer.size : 0) : nullptr;
  } else {
    in_ = in ? cbuffer.data : nullptr;
    out_ = out ? (in ? owned.get() : cbuffer.data) : nullptr;
  }
  // Allocate areas which have not been provided -

  _read = in ? cbuffer.read : nullptr;
  _write = out ? cbuffer.write : nullptr;
  _size = cbuffer.size;
  _owned = std::move(owned);
  _in = in_;
  _out = out_;
  setg(_in, _in, _in); // empty: first read calls underflow
  if(_out != nullptr) {
    setp(_out, _out + _size);
  } else {
    setp(nullptr, nullptr);
  }
}

std::streambuf::int_type
CStreambuf::underflow() {
  if(gptr() < egptr()) {
    return traits_type::to_int_type(*gptr());
  }
  if(_read == nullptr) {
    return traits_type::eof();
  }
  if(!flushOutput()) { // send request before waiting for response
    return traits_type::eof();
  }
  std::size_t read = _read(_in, _size);
  if(read == 0) {
    return traits_type::eof();
  }
  if(read > _size) { // invalid read function result
    read = _size;
  }
  setg(_in, _in, _in + read);
  return traits_type::to_int_type(*gptr());
}

std::streambuf::int_type
CStreambuf::overflow(std::streambuf::int_type value) {
  if(_write == nullptr || !flushOutput()) {
    return traits_type::eof();
  }
  if(!traits_type::eq_int_type(value, traits_type::eof())) {
    *pptr() = traits_type::to_char_type(value); // writing area is empty after flush (and its size is not 0)
    pbump(1);
  }
  return traits_type::not_eof(value);
}

int
CStreambuf::sync() {
  return flushOutput() ? 0 : -1;
}

bool
CStreambuf::flushOutput() {
  if(_write == nullptr || pbase() == pptr()) {
    return true;
  }
  const char* data = pbase();
  std::size_t remaining = static_cast<std::size_t>(pptr() - pbase());
  setp(_out, _out + _size); // reset writing area: unsent data are dropped on error
  while(remaining > 0) {
    std::size_t written = _write(data, remaining);
    if(written == 0 || written > remaining) {
      return false;
    }
    data += written;
    remaining -= written;
  }
  return true;
}
// CStreambuf -

// CIStream +
CIStream::CIStream(const anch::cutils::cbuffer& cbuffer): std::istream(nullptr), _streambuf(cbuffer, Direction::IN) {
  rdbuf(&_streambuf);
}

CIStream::CIStream(): std::istream(nullptr), _streambuf() {
  rdbuf(&_streambuf);
}

CIStream::~CIStream() {
  // Nothing to do
}

void
CIStream::setBuffer(const anch::cutils::cbuffer& cbuffer) {
  _streambuf.setBuffer(cbuffer, Direction::IN);
  clear();
}
// CIStream -

// COStream +
COStream::COStream(const anch::cutils::cbuffer& cbuffer): std::ostream(nullptr), _streambuf(cbuffer, Direction::OUT) {
  rdbuf(&_streambuf);
}

COStream::COStream(): std::ostream(nullptr), _streambuf() {
  rdbuf(&_streambuf);
}

COStream::~COStream() {
  // Nothing to do: pending data are sent by the stream buffer destructor
}

void
COStream::setBuffer(const anch::cutils::cbuffer& cbuffer) {
  _streambuf.setBuffer(cbuffer, Direction::OUT);
  clear();
}
// COStream -

// CIOStream +
CIOStream::CIOStream(const anch::cutils::cbuffer& cbuffer): std::iostream(nullptr), _streambuf(cbuffer, Direction::INOUT) {
  rdbuf(&_streambuf);
}

CIOStream::CIOStream(): std::iostream(nullptr), _streambuf() {
  rdbuf(&_streambuf);
}

CIOStream::~CIOStream() {
  // Nothing to do: pending data are sent by the stream buffer destructor
}

void
CIOStream::setBuffer(const anch::cutils::cbuffer& cbuffer) {
  _streambuf.setBuffer(cbuffer, Direction::INOUT);
  clear();
}
// CIOStream -
