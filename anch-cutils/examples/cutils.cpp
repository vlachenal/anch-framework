/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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
//! [example]
#include "cutils/iostream.hpp"

#include <iostream>
#include <string>

int
main() {
  // C style write function (a socket, a C library callback, ...): here it collects data
  std::string sink;
  anch::cutils::cbuffer buffer;
  buffer.size = 1024;                              // internal buffer size
  buffer.write = [&sink](const char* data, std::size_t size) {
    sink.append(data, size);
    return size;                                   // number of bytes written
  };
  {
    anch::cutils::COStream out(buffer);
    out << "answer=" << 42 << std::endl;           // flushed through write function
  }

  // C style read function: returns 0 at end of data
  std::string source = "1 2 3";
  std::size_t offset = 0;
  anch::cutils::cbuffer input;
  input.size = 2;
  input.read = [&](char* data, std::size_t size) {
    std::size_t nb = std::min(size, source.size() - offset);
    source.copy(data, nb, offset);
    offset += nb;
    return nb;
  };
  anch::cutils::CIStream in(input);
  int a, b, c;
  in >> a >> b >> c;
  std::cout << sink << a + b + c << std::endl;
  return 0;
}
//! [example]
