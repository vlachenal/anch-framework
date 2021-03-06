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
#include "sql/preparedStatement.hpp"

#include "date/dateFormatter.hpp"

using anch::sql::PreparedStatement;


PreparedStatement::PreparedStatement() noexcept: _values(), _nbPlaceholders(0) {
  // Nothing to do
}

PreparedStatement::~PreparedStatement() noexcept {
  // Nothing to do
}

std::set<std::size_t>
PreparedStatement::getPlaceholders(const std::string& query) const {
  std::set<std::size_t> pos;
  bool singleVal = false;
  bool listVal = false;
  bool literral = false;
  for(std::size_t i = 0 ; i < query.length() ; ++i) {
    switch(query.at(i)) {
    case '?':
      if(singleVal || listVal) {
	pos.insert(i);
	singleVal = false;
      }
      break;
    case '=':
      if(!literral) {
	singleVal = true;
	listVal = false;
      }
      break;
    case '(':
      if(!literral) {
	listVal = true;
	singleVal = false;
      }
      break;
    case ')': // End of list
      if(!literral) {
	listVal = false;
	singleVal = false;
      }
      break;
    case '\'':
      if(literral && i < query.length() - 1 && query.at(i + 1) == '\'') { // Escape character in string
	++i; // Discard next "'"
      } else {
	literral = !literral;
      }
      break;
    case '\\': // Escape character in string
      if(literral) {
	++i;
      }
      break;
    case ' ':
    case '\n':
    case '\t':
    case '\r':
      // Not a revelant character
      break;
    default:
      singleVal = false;
    }
  }
  return pos;
}

void
PreparedStatement::set(std::size_t idx, int16_t value) {
  checkIndex(idx);
  std::ostringstream oss;
  oss << value;
  _values[idx] = oss.str();
}

void
PreparedStatement::set(std::size_t idx, uint16_t value) {
  checkIndex(idx);
  std::ostringstream oss;
  oss << value;
  _values[idx] = oss.str();
}

void
PreparedStatement::set(std::size_t idx, int32_t value) {
  checkIndex(idx);
  std::ostringstream oss;
  oss << value;
  _values[idx] = oss.str();
}

void
PreparedStatement::set(std::size_t idx, uint32_t value) {
  checkIndex(idx);
  std::ostringstream oss;
  oss << value;
  _values[idx] = oss.str();
}

void
PreparedStatement::set(std::size_t idx, int64_t value) {
  checkIndex(idx);
  std::ostringstream oss;
  oss << value;
  _values[idx] = oss.str();
}

void
PreparedStatement::set(std::size_t idx, uint64_t value) {
  checkIndex(idx);
  std::ostringstream oss;
  oss << value;
  _values[idx] = oss.str();
}

void
PreparedStatement::set(std::size_t idx, const std::string& value) {
  checkIndex(idx);
  _values[idx] = value;
}

void
PreparedStatement::set(std::size_t idx, const char* const value) {
  checkIndex(idx);
  _values[idx] = std::string(value);
}

void
PreparedStatement::set(std::size_t idx, const anch::sql::Date& value) {
  static anch::date::DateFormatter formatter("%Y-%m-%d");
  checkIndex(idx);
  formatter.format(value, _values[idx]);
}

void
PreparedStatement::set(std::size_t idx, const anch::sql::Time& value) {
  static anch::date::DateFormatter formatter("%H:%M:%S.%s");
  checkIndex(idx);
  formatter.format(value, _values[idx]);
}

void
PreparedStatement::set(std::size_t idx, const anch::sql::Timestamp& value) {
  static anch::date::DateFormatter formatter("%Y-%m-%d %H:%M:%S.%s");
  checkIndex(idx);
  formatter.format(value, _values[idx]);
}
