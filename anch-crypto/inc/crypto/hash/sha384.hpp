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

#include "crypto/hash/impl/sha384_512.hpp"

namespace anch::crypto {

  /*! SHA384 initial values */
  extern std::array<uint64_t,8> SHA384_VALUES;
  /*
   * SHA384 defintion
   */
  extern template class Hash<48,128>;
  extern template const std::array<uint8_t,48>& Hash<48,128>::digest(const std::string& data);
  extern template const std::array<uint8_t,48>& Hash<48,128>::digest(std::istream& stream);
  extern template class SHA2<48,128,uint64_t,80,SHA384_VALUES>;
  extern template class SHA384_512<48,SHA384_VALUES>;
  extern template SHA384_512<48,SHA384_VALUES>::SHA384_512(const std::string& data);
  extern template SHA384_512<48,SHA384_VALUES>::SHA384_512(std::istream& stream);
  /*!
   * SHA384 defintion
   */
  using SHA384 = SHA384_512<48,SHA384_VALUES>;

  /*! HMAC SHA384 definition */
  extern template SHA384 HMAC<SHA384>(const std::string&, const std::string&);

}
