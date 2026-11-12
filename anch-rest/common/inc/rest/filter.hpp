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

#include <functional>

#include "rest/request.hpp"
#include "rest/response.hpp"

namespace anch::rest {

  /*! Next step of a filter chain (next filter, then endpoint on server side or transport on client side) */
  using Next = std::function<anch::rest::Response(anch::rest::Request&)>;

  /*!
   * Filter: executed around endpoints (server) or requests (client), it can change the request or the
   * response, or answer itself without calling \c next
   */
  using Filter = std::function<anch::rest::Response(anch::rest::Request&, const anch::rest::Next&)>;

}
