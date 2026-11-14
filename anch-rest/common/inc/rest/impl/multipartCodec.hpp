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
  anch::rest::MediaType
  Multipart::contentType(const T&) {
    anch::rest::MediaType res("multipart", "form-data");
    res.setParameter("boundary", Multipart::randomBoundary());
    return res;
  }

  template<anch::rest::FormMapped T>
  void
  Multipart::serialize(const T& value, std::ostream& out, const anch::rest::MediaType& contentType, anch::rest::Side side) {
    anch::rest::Form form;
    anch::rest::FormMapper<T>::toForm(value, form, side);
    Multipart::serialize(form, out, contentType, side);
  }

  template<anch::rest::FormMapped T>
  void
  Multipart::deserialize(T& value, std::istream& input, const anch::rest::MediaType& contentType, anch::rest::Side side) {
    anch::rest::Form form;
    Multipart::deserialize(form, input, contentType, side);
    anch::rest::FormMapper<T>::fromForm(value, form, side);
  }

}
