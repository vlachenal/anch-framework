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
#include "json/constraints.hpp"

#include <algorithm>

#include "json/mappingError.hpp"
#include "json/readerContext.hpp"

using anch::json::ErrorCode;
using anch::json::MappingError;
using anch::json::ReaderContext;

void
anch::json::detail::checkRequired(const std::vector<std::string>& required, const uint64_t* found, ReaderContext& context) {
  // Fast path: every required field has been found +
  std::size_t nbWords = (required.size() + 63) / 64;
  bool complete = true;
  for(std::size_t i = 0 ; i < nbWords ; ++i) {
    std::size_t bits = std::min<std::size_t>(64, required.size() - i * 64);
    uint64_t mask = (bits == 64) ? ~uint64_t{0} : ((uint64_t{1} << bits) - 1);
    if(found[i] != mask) {
      complete = false;
      break;
    }
  }
  if(complete) {
    return;
  }
  // Fast path: every required field has been found -
  std::string missing;
  std::size_t count = 0;
  for(std::size_t i = 0 ; i < required.size() ; ++i) {
    if((found[i / 64] & (uint64_t{1} << (i % 64))) == 0) {
      if(count > 0) {
	missing += ", ";
      }
      missing += '\'';
      missing += required[i];
      missing += '\'';
      ++count;
    }
  }
  throw MappingError(ErrorCode::MISSING_FIELD, context, (count == 1 ? "Missing required field " : "Missing required fields ") + missing);
}

void
anch::json::detail::rejectNull(ReaderContext& context) {
  throw MappingError(ErrorCode::CONSTRAINT_VIOLATION, context, "Required field can not be null");
}

void
anch::json::detail::violation(ReaderContext& context, const std::string& message) {
  throw MappingError(ErrorCode::CONSTRAINT_VIOLATION, context, message);
}

void
anch::json::detail::violation([[maybe_unused]] anch::json::WriterContext& context, const std::string& message) {
  throw MappingError(ErrorCode::CONSTRAINT_VIOLATION, message);
}
