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
#include "rest/endpoint.hpp"

#include <algorithm>


using anch::rest::EndPoint;
using anch::rest::Request;


std::regex EndPoint::PATH_PATTERN("\\{\\}");

// Constructors +
EndPoint::EndPoint():  verb(), pathPattern(), contentType(), accept(), _pathRegex(), _built(false) {
  // Nothing to do
}

EndPoint::EndPoint(const EndPoint& other):  verb(other.verb),
					    pathPattern(other.pathPattern),
					    contentType(other.contentType),
					    accept(other.accept),
					    _pathRegex(other._pathRegex),
					    _built(other._built) {
  // Nothing to do
}

EndPoint::EndPoint(EndPoint&& other):  verb(std::move(other.verb)),
				       pathPattern(std::move(other.pathPattern)),
				       contentType(std::move(other.contentType)),
				       accept(std::move(other.accept)),
				       _pathRegex(std::move(other._pathRegex)),
				       _built(other._built) {
}
// Constructors -

// Destructor +
EndPoint::~EndPoint() {
  // Nothing to do
}
// Destructor -

// Methods +
bool
EndPoint::match(const Request& request) const {
  return std::regex_match(request.getPath(), _pathRegex);
}

void
EndPoint::build() {
  if(!_built) {
    _pathRegex = std::regex(std::regex_replace(pathPattern, PATH_PATTERN, "([^/]+)"));
    _built = true;
  }
}
// Methods -
