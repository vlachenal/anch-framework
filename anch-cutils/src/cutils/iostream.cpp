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

// CStreambuf +
CStreambuf::CStreambuf(anch::cutils::cbuffer cbuffer, anch::cutils::Direction dir):
  _buffer(cbuffer), _deleteBuffer(false) {
  // Check buffer options according to direction +
  if((static_cast<short>(dir) ^ static_cast<short>(anch::cutils::Direction::IN)) == 0) {
    _buffer.write = NULL;
  }
  if((static_cast<short>(dir) ^ static_cast<short>(anch::cutils::Direction::OUT)) == 0) {
    _buffer.read = NULL;
  }
  if(static_cast<short>(dir) & static_cast<short>(anch::cutils::Direction::IN) && _buffer.read == NULL) {
    throw std::invalid_argument("input stream and null read function");
  }
  if(static_cast<short>(dir) & static_cast<short>(anch::cutils::Direction::OUT) && _buffer.write == NULL) {
    throw std::invalid_argument("output stream and null write function");
  }
  // Check buffer options according to direction -
  // Intialize buffer if needed +
  if(_buffer.data == NULL) {
    _buffer.data = new char[_buffer.size];
    std::memset(_buffer.data, 0, _buffer.size);
    _deleteBuffer = true;
  }
  // Intialize buffer if needed -
  // Set ready +
  setg(0, 0, 0);
  setp(_buffer.data, _buffer.data + _buffer.size);
  // Set ready -
}

CStreambuf::~CStreambuf() {
  sync();
  if(_deleteBuffer) {
    delete[] _buffer.data;
  }
}

std::streambuf::int_type
CStreambuf::underflow() {
  std::size_t read = _buffer.read(_buffer.data, _buffer.size);
  if(read == 0) {
    return traits_type::eof();
  }
  setg(_buffer.data, _buffer.data, _buffer.data + read);
  return traits_type::to_int_type(*gptr());
}

std::streambuf::int_type
CStreambuf::overflow(std::streambuf::int_type value) {
  long write = pptr() - pbase();
  if(write) {
    _buffer.write(_buffer.data, static_cast<std::size_t>(write));
  }
  setp(_buffer.data, _buffer.data + _buffer.size);
  if(!traits_type::eq_int_type(value, traits_type::eof())) {
    sputc(static_cast<char>(value));
  }
  return traits_type::not_eof(value);
}

int
CStreambuf::sync() {
  std::streambuf::int_type result = this->overflow(traits_type::eof());
  return traits_type::eq_int_type(result, traits_type::eof()) ? -1 : 0;
}
// CStreambuf -

// CIStream +
CIStream::CIStream(anch::cutils::cbuffer cbuffer):
  std::istream(new CStreambuf(cbuffer, anch::cutils::Direction::IN)) {
  // Nothing to do
}

CIStream::~CIStream() {
  delete rdbuf();
}
// CIStream -

// COStream +
COStream::COStream(anch::cutils::cbuffer cbuffer):
  std::ostream(new CStreambuf(cbuffer, anch::cutils::Direction::OUT)) {
  // Nothing to do
}

COStream::~COStream() {
  delete rdbuf();
}
// COStream -

// CIOStream +
CIOStream::CIOStream(anch::cutils::cbuffer cbuffer):
  std::iostream(new CStreambuf(cbuffer, anch::cutils::Direction::INOUT)) {
  // Nothing to do
}

CIOStream::~CIOStream() {
  delete rdbuf();
}
// CIOStream -
