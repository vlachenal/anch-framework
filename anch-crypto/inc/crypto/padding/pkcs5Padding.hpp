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

#include <cstddef>
#include <stdint.h>


namespace anch::crypto {

  /*!
   * \brief PKCS5 padding implementation.
   *
   * Padding used in PKCS5 public-key cryptography standard.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class PKCS5Padding {

  public:
    /*!
     * Pad data
     *
     * \param data the data to pad
     * \param len the data length
     * \param expLen the expected length
     */
    static void pad(uint8_t* data, std::size_t len, std::size_t expLen);

    /*!
     * Unpad data
     *
     * \param data the data to unpad
     * \param len the block length
     *
     * \return the data length
     */
    static std::size_t length(uint8_t* data, std::size_t len);

  };

}
