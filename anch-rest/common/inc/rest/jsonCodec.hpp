/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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
#pragma once

#include "codec/json.hpp"

#include "rest/codec.hpp"

/*!
 * \file rest/jsonCodec.hpp
 * \deprecated removed in 0.4: the JSON codec has moved to anch-codec-json, use \c codec/json.hpp
 */

namespace anch::rest {

  /*! \deprecated removed in 0.4, use \ref anch::codec::Json */
  using Json [[deprecated("removed in 0.4, use anch::codec::Json")]] = anch::codec::Json;

}
