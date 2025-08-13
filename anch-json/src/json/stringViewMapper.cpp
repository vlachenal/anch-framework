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

#include <string_view>
#include <set>
#include <vector>
#include <list>
#include <map>

#include "json/mappingError.hpp"

using anch::json::PrimitiveMapper;

using StringViewMapper = anch::json::GenericMapper<PrimitiveMapper<std::string_view>,std::string_view>;

// PrimitiveMapper specialization for std::string_view +
template<>
PrimitiveMapper<std::string_view>::PrimitiveMapper() {
  // Nothing to do
}

template<>
PrimitiveMapper<std::string_view>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
void
PrimitiveMapper<std::string_view>::serializeValue(const std::string_view& value, anch::json::WriterContext& context) const {
  context.output.put(anch::json::STRING_DELIMITER)
    .write(value.data(), static_cast<std::streamsize>(value.size()))
    .put(anch::json::STRING_DELIMITER);
}

template<>
bool
PrimitiveMapper<std::string_view>::deserializeValue(std::string_view& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::string_view& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::optional<std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::string_view* value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::vector<std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::list<std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::set<std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template<>
bool
StringViewMapper::deserialize(std::map<std::string,std::string_view>& value, anch::json::ReaderContext& context) const = delete;

template class PrimitiveMapper<std::string_view>;
// PrimitiveMapper specialization for std::string_view -
