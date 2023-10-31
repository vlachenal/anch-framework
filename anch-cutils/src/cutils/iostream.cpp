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
    _buffer.write = nullptr;
  }
  if((static_cast<short>(dir) ^ static_cast<short>(anch::cutils::Direction::OUT)) == 0) {
    _buffer.read = nullptr;
  }
  if(static_cast<short>(dir) & static_cast<short>(anch::cutils::Direction::IN) && _buffer.read == nullptr) {
    throw std::invalid_argument("input stream and null read function");
  }
  if(static_cast<short>(dir) & static_cast<short>(anch::cutils::Direction::OUT) && _buffer.write == nullptr) {
    throw std::invalid_argument("output stream and null write function");
  }
  // Check buffer options according to direction -

  initBuffer();
}

CStreambuf::CStreambuf() noexcept: _deleteBuffer(false) {
  // Nothing to do
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

void
CStreambuf::initBuffer() {
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
// CStreambuf -

// CIStream +
CIStream::CIStream(anch::cutils::cbuffer cbuffer):
  std::istream(new CStreambuf(cbuffer, anch::cutils::Direction::IN)) {
  // Nothing to do
}

CIStream::CIStream(): std::istream(new CStreambuf()) {
  // Nothing to do
}

CIStream::~CIStream() {
  delete rdbuf();
}

void
CIStream::setBuffer(anch::cutils::cbuffer cbuffer) {
  cbuffer.write = nullptr;
  if(cbuffer.read == nullptr) {
    throw std::invalid_argument("output stream and null write function");
  }
  static_cast<CStreambuf*>(rdbuf())->_buffer = cbuffer;
  static_cast<CStreambuf*>(rdbuf())->initBuffer();
}
// CIStream -

// COStream +
COStream::COStream(anch::cutils::cbuffer cbuffer):
  std::ostream(new CStreambuf(cbuffer, anch::cutils::Direction::OUT)) {
  // Nothing to do
}

COStream::COStream(): std::ostream(new CStreambuf()) {
  // Nothing to do
}

COStream::~COStream() {
  delete rdbuf();
}

void
COStream::setBuffer(anch::cutils::cbuffer cbuffer) {
  cbuffer.read = nullptr;
  if(cbuffer.write == nullptr) {
    throw std::invalid_argument("input stream and null read function");
  }
  static_cast<CStreambuf*>(rdbuf())->_buffer = cbuffer;
  static_cast<CStreambuf*>(rdbuf())->initBuffer();
}
// COStream -

// CIOStream +
CIOStream::CIOStream(anch::cutils::cbuffer cbuffer):
  std::iostream(new CStreambuf(cbuffer, anch::cutils::Direction::INOUT)) {
  // Nothing to do
}

CIOStream::CIOStream(): std::iostream(new CStreambuf()) {
  // Nothing to do
}

CIOStream::~CIOStream() {
  delete rdbuf();
}

void
CIOStream::setBuffer(anch::cutils::cbuffer cbuffer) {
  if(cbuffer.read == nullptr || cbuffer.write == nullptr) {
    throw std::invalid_argument("read and write function has to be defined");
  }
  static_cast<CStreambuf*>(rdbuf())->_buffer = cbuffer;
  static_cast<CStreambuf*>(rdbuf())->initBuffer();
}
// CIOStream -
