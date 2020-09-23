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
#include <set>
#include <vector>
#include <list>


namespace anch {
  namespace json {

    template<typename T>
    bool
    serializeNumericArray(const T& array, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << anch::json::ARRAY_BEGIN;
      for(auto iter = array.begin() ; iter != array.end() ; ++iter) {
	out << *iter << anch::json::FIELD_SEPARATOR; // \todo last ','
      }
      out << anch::json::ARRAY_END;
      return true;
    }

    // JSONPrimitiveMapper specialization for uint64_t +
    template<>
    JSONPrimitiveMapper<uint64_t>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<uint64_t>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<uint64_t>::serialize(const uint64_t& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint64_t>::serialize(const uint64_t* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << *value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint64_t>::serialize(const std::optional<uint64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value.value();
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint64_t>::serialize(const std::vector<uint64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<uint64_t>::serialize(const std::list<uint64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<uint64_t>::serialize(const std::set<uint64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<uint64_t>;
    // JSONPrimitiveMapper specialization for uint64_t -

    // JSONPrimitiveMapper specialization for int64_t +
    template<>
    JSONPrimitiveMapper<int64_t>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<int64_t>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<int64_t>::serialize(const int64_t& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int64_t>::serialize(const int64_t* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << *value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int64_t>::serialize(const std::optional<int64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value.value();
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int64_t>::serialize(const std::vector<int64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<int64_t>::serialize(const std::list<int64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<int64_t>::serialize(const std::set<int64_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<int64_t>;
    // JSONPrimitiveMapper specialization for int64_t -

    // JSONPrimitiveMapper specialization for uint32_t +
    template<>
    JSONPrimitiveMapper<uint32_t>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<uint32_t>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<uint32_t>::serialize(const uint32_t& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint32_t>::serialize(const uint32_t* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << *value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint32_t>::serialize(const std::optional<uint32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value.value();
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint32_t>::serialize(const std::vector<uint32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<uint32_t>::serialize(const std::list<uint32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<uint32_t>::serialize(const std::set<uint32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<uint32_t>;
    // JSONPrimitiveMapper specialization for uint32_t -

    // JSONPrimitiveMapper specialization for int32_t +
    template<>
    JSONPrimitiveMapper<int32_t>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<int32_t>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<int32_t>::serialize(const int32_t& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int32_t>::serialize(const int32_t* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << *value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int32_t>::serialize(const std::optional<int32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value.value();
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int32_t>::serialize(const std::vector<int32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<int32_t>::serialize(const std::list<int32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<int32_t>::serialize(const std::set<int32_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<int32_t>;
    // JSONPrimitiveMapper specialization for int32_t -

    // JSONPrimitiveMapper specialization for uint16_t +
    template<>
    JSONPrimitiveMapper<uint16_t>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<uint16_t>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<uint16_t>::serialize(const uint16_t& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint16_t>::serialize(const uint16_t* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << *value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint16_t>::serialize(const std::optional<uint16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value.value();
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint16_t>::serialize(const std::vector<uint16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<uint16_t>::serialize(const std::list<uint16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<uint16_t>::serialize(const std::set<uint16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<uint16_t>;
    // JSONPrimitiveMapper specialization for uint16_t -

    // JSONPrimitiveMapper specialization for int16_t +
    template<>
    JSONPrimitiveMapper<int16_t>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<int16_t>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<int16_t>::serialize(const int16_t& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int16_t>::serialize(const int16_t* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << *value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int16_t>::serialize(const std::optional<int16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value.value();
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int16_t>::serialize(const std::vector<int16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<int16_t>::serialize(const std::list<int16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<int16_t>::serialize(const std::set<int16_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<int16_t>;
    // JSONPrimitiveMapper specialization for int16_t -

    // JSONPrimitiveMapper specialization for uint8_t +
    template<>
    JSONPrimitiveMapper<uint8_t>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<uint8_t>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<uint8_t>::serialize(const uint8_t& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint8_t>::serialize(const uint8_t* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << *value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint8_t>::serialize(const std::optional<uint8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value.value();
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<uint8_t>::serialize(const std::vector<uint8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<uint8_t>::serialize(const std::list<uint8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<uint8_t>::serialize(const std::set<uint8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<uint8_t>;
    // JSONPrimitiveMapper specialization for uint8_t -

    // JSONPrimitiveMapper specialization for int8_t +
    template<>
    JSONPrimitiveMapper<int8_t>::JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    JSONPrimitiveMapper<int8_t>::~JSONPrimitiveMapper() {
      // Nothing to do
    }

    template<>
    bool
    JSONPrimitiveMapper<int8_t>::serialize(const int8_t& value, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int8_t>::serialize(const int8_t* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << *value;
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int8_t>::serialize(const std::optional<int8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      if(field.has_value()) {
	out << anch::json::STRING_DELIMITER << field.value() << anch::json::STRING_DELIMITER << anch::json::FIELD_VALUE_SEPARATOR;
      }
      out << value.value();
      return true;
    }

    template<>
    bool
    JSONPrimitiveMapper<int8_t>::serialize(const std::vector<int8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<int8_t>::serialize(const std::list<int8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    template<>
    bool
    JSONPrimitiveMapper<int8_t>::serialize(const std::set<int8_t>& value, std::ostream& out, const std::optional<std::string>& field) {
      return serializeNumericArray(value, out, field);
    }

    extern template class JSONPrimitiveMapper<int8_t>;
    // JSONPrimitiveMapper specialization for int8_t -

  }
}
