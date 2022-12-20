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

using anch::cutils::CStreambuf;
using anch::cutils::CIStream;
using anch::cutils::COStream;

// CStreambuf +
CStreambuf::CStreambuf(anch::cutils::cbuffer cbuffer):
  _buffer(cbuffer), _deleteBuffer(false) {
  if(_buffer.data == NULL) {
    _buffer.data = new char[_buffer.size];
    std::memset(_buffer.data, 0, _buffer.size);
    _deleteBuffer = true;
  }
  setg(0, 0, 0);
  setp(_buffer.data, _buffer.data + _buffer.size);
}

CStreambuf::~CStreambuf() {
  sync();
  if(_deleteBuffer) {
    delete[] _buffer.data;
  }
}

std::streambuf::int_type
CStreambuf::underflow() {
  std::size_t read = _buffer.handle(_buffer.data, _buffer.size);
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
    _buffer.handle(_buffer.data, static_cast<std::size_t>(write));
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
  std::istream(new CStreambuf(cbuffer)) {
  // Nothing to do
}

CIStream::~CIStream() {
  delete rdbuf();
}
// CIStream -

// COStream +
COStream::COStream(anch::cutils::cbuffer cbuffer):
  std::ostream(new CStreambuf(cbuffer)) {
  // Nothing to do
}

COStream::~COStream() {
  delete rdbuf();
}
// COStream -
