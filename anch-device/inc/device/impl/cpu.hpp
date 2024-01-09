/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

namespace anch::device {

  inline bool CPU::isAES() const {
    return _aes;
  }

  inline bool CPU::isMMX() const {
    return _mmx;
  }

  inline bool CPU::isSSE() const {
    return _sse;
  }

  inline bool CPU::isSSE2() const {
    return _sse2;
  }

  inline bool CPU::isSSE3() const {
    return _sse3;
  }

  inline bool CPU::isSSSE3() const {
    return _ssse3;
  }

  inline bool CPU::isSSE4() const {
    return _sse4;
  }

  inline bool CPU::isSSE4_1() const {
    return _sse4_1;
  }

  inline bool CPU::isSSE4_2() const {
    return _sse4_2;
  }

}
