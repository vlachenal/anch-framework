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

#include <sstream>
#include <array>

#include "crypto/hash/hash.hpp"


namespace anch::crypto {

  /*!
   * Compute HMAC according to hash algorithm, key and message.\n
   * This function uses C++ string for key and message.
   *
   * \param key the key to use
   * \param message the message to use
   */
  template<typename H>
  H HMAC(const std::string& key, const std::string& message);

}

#include "crypto/impl/hmac.hpp"
