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
#pragma once

#include <string>
#include <string_view>
#include <set>
#include <vector>
#include <list>

namespace anch {
  namespace json {

    template<typename T>
    bool
    serializeStringArray(const T& array, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::ARRAY_BEGIN;
      for(auto iter = array.begin() ; iter != array.end() ; ++iter) {
	out << anch::json::STRING_DELIMITER << *iter << anch::json::STRING_DELIMITER << anch::json::FIELD_SEPARATOR; // \todo last ','
      }
      out << anch::json::ARRAY_END;
      return true;
    }

    // JSONPrimitiveMapper specialization for std::string +
    template<>
    JSONPrimitiveMapper<std::string>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<std::string>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string>::serialize(const std::string& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::STRING_DELIMITER << value << anch::json::STRING_DELIMITER;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string>::serialize(const std::string* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::STRING_DELIMITER << *value << anch::json::STRING_DELIMITER;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string>::serialize(const std::optional<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::STRING_DELIMITER << value.value() << anch::json::STRING_DELIMITER;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string>::serialize(const std::vector<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeStringArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string>::serialize(const std::list<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeStringArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string>::serialize(const std::set<std::string>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeStringArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<std::string>;
    // JSONPrimitiveMapper specialization for std::string -

    // JSONPrimitiveMapper specialization for std::string_view +
    template<>
    JSONPrimitiveMapper<std::string_view>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<std::string_view>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string_view>::serialize(const std::string_view& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::STRING_DELIMITER << value << anch::json::STRING_DELIMITER;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string_view>::serialize(const std::string_view* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::STRING_DELIMITER << *value << anch::json::STRING_DELIMITER;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string_view>::serialize(const std::optional<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::STRING_DELIMITER << value.value() << anch::json::STRING_DELIMITER;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string_view>::serialize(const std::vector<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeStringArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string_view>::serialize(const std::list<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeStringArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<std::string_view>::serialize(const std::set<std::string_view>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeStringArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<std::string_view>;
    // JSONPrimitiveMapper specialization for std::string_view -

  }  // json
}  // anch
