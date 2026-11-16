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
#ifdef ANCH_DATE
#include "json/primitiveMapper.hpp"

#include <chrono>
#include <stdexcept>
#include <string>
#include <string_view>

#include "date/date.hpp"
#include "date/rfc3339.hpp"

#include "json/lexer.hpp"
#include "json/mappingError.hpp"


using anch::json::PrimitiveMapper;
using std::chrono::year_month_day;


// PrimitiveMapper specialization for anch::date::Date (RFC 3339 date-time) +
template<>
PrimitiveMapper<anch::date::Date>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<anch::date::Date>,anch::date::Date>() {
  // Nothing to do
}

template<>
PrimitiveMapper<anch::date::Date>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
void
PrimitiveMapper<anch::date::Date>::serializeValue(const anch::date::Date& value, anch::json::WriterContext& context) const {
  char date[anch::date::rfc3339::DATE_TIME_MAX_SIZE + 2]; // '"' + date + '"' (no allocation)
  date[0] = anch::json::STRING_DELIMITER;
  std::size_t size = anch::date::rfc3339::formatDateTime(value, date + 1) + 1;
  date[size++] = anch::json::STRING_DELIMITER;
  context.write(std::string_view(date, size));
}

template<>
bool
PrimitiveMapper<anch::date::Date>::deserializeValue(anch::date::Date& value, anch::json::ReaderContext& context) const {
  std::string date;
  if(!anch::json::lexString(date, context)) {
    return false;
  }
  try {
    value = anch::date::rfc3339::parseDateTime(date);
  } catch(const std::invalid_argument& e) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context, e.what());
  }
  return true;
}

template class anch::json::PrimitiveMapper<anch::date::Date>;
// PrimitiveMapper specialization for anch::date::Date (RFC 3339 date-time) -

// PrimitiveMapper specialization for std::chrono::year_month_day (RFC 3339 full-date) +
template<>
PrimitiveMapper<year_month_day>::PrimitiveMapper():
  anch::json::GenericMapper<PrimitiveMapper<year_month_day>,year_month_day>() {
  // Nothing to do
}

template<>
PrimitiveMapper<year_month_day>::~PrimitiveMapper() {
  // Nothing to do
}

template<>
void
PrimitiveMapper<year_month_day>::serializeValue(const year_month_day& value, anch::json::WriterContext& context) const {
  char date[anch::date::rfc3339::DATE_SIZE + 2]; // '"' + date + '"' (no allocation)
  date[0] = anch::json::STRING_DELIMITER;
  anch::date::rfc3339::formatDate(value, date + 1);
  date[anch::date::rfc3339::DATE_SIZE + 1] = anch::json::STRING_DELIMITER;
  context.write(std::string_view(date, sizeof(date)));
}

template<>
bool
PrimitiveMapper<year_month_day>::deserializeValue(year_month_day& value, anch::json::ReaderContext& context) const {
  std::string date;
  if(!anch::json::lexString(date, context)) {
    return false;
  }
  try {
    value = anch::date::rfc3339::parseDate(date);
  } catch(const std::invalid_argument& e) {
    throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context, e.what());
  }
  return true;
}

template class anch::json::PrimitiveMapper<year_month_day>;
// PrimitiveMapper specialization for std::chrono::year_month_day (RFC 3339 full-date) -

#endif
