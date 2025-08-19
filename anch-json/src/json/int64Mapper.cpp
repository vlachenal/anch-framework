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
#include "json/primitiveMapper.hpp"

#include "json/lexer.hpp"


using anch::json::PrimitiveMapper;

// PrimitiveMapper specialization for int64_t +
template<>
PrimitiveMapper<int64_t>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<int64_t>,int64_t>() {
  // Nothing to do
}

template<>
PrimitiveMapper<int64_t>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
void
PrimitiveMapper<int64_t>::serializeValue(const int64_t& value, anch::json::WriterContext& context) const {
  context.output << value;
}

template<>
bool
PrimitiveMapper<int64_t>::deserializeValue(int64_t& value, anch::json::ReaderContext& context) const {
  return anch::json::lexInteger(value, context);
}

template class PrimitiveMapper<int64_t>;
// PrimitiveMapper specialization for int64_t -
