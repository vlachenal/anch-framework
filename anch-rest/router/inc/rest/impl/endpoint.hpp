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
#pragma once

namespace anch::rest {

  inline const std::regex&
  anch::rest::EndPoint::getPathRegex() const {
    return _pathRegex;
  }

  inline bool operator==(const anch::rest::EndPoint& lhs, const anch::rest::EndPoint& rhs) {
    if(lhs.verb != rhs.verb || lhs.contentType != rhs.contentType || lhs.accept != rhs.accept) {
      return false;
    }
    return lhs.pathPattern == rhs.pathPattern;
  }

  inline bool operator<(const anch::rest::EndPoint& lhs, const anch::rest::EndPoint& rhs) {
    if(lhs.pathPattern == rhs.pathPattern) {
      if(rhs.accept == MediaType::ALL) {
	return false;
      }
      if(rhs.contentType == MediaType::ALL) {
	return false;
      }
      return lhs.verb < rhs.verb || lhs.contentType < rhs.contentType || lhs.accept < rhs.accept;
    } else {
      return lhs.pathPattern < rhs.pathPattern;
    }
  }

}
