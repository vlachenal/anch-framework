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

namespace anch::rest {

  template<anch::rest::FormMapped T>
  void
  FormUrlEncoded::serialize(const T& value, std::ostream& out, anch::codec::Side side) {
    anch::rest::Form form;
    anch::rest::FormMapper<T>::toForm(value, form, side);
    FormUrlEncoded::serialize(form, out);
  }

  template<anch::rest::FormMapped T>
  void
  FormUrlEncoded::deserialize(T& value, std::istream& input, const anch::codec::MediaType& contentType, anch::codec::Side side) {
    anch::rest::Form form;
    FormUrlEncoded::deserialize(form, input, contentType, side);
    anch::rest::FormMapper<T>::fromForm(value, form, side);
  }

}
