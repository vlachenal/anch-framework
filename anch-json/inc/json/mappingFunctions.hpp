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

#include <istream>
#include <optional>
#include <map>

#include <functional>

#include "json/constants.hpp"
#include "json/mappingOptions.hpp"

namespace anch::json {

  /*!
   * Consumes discarded characters
   *
   * \param input the input stream to consumes
   * \param options the options to use
   *
   * \throw plop if stream reach its end
   */
  void discardChars(std::istream& input, const anch::json::MappingOptions& options);

  /*!
   * Check if stream starts with 'null' and consumes these characters
   *
   * \param input the input stream to consumes
   * \param options the options to use
   *
   * \return \c true if input stream starts with 'null', \c false otherwise
   *
   * \throw plop if stream reach its end
   */
  bool isNull(std::istream& input, const anch::json::MappingOptions& options);

  /*!
   * Parse input to get new field definition.\n
   * This will consume '"<field name>":'
   *
   * \param input the input stream to parse
   * \param options the options to use
   *
   * \return the field's name if found, \c empty otherwise
   */
  std::optional<std::string> getFieldName(std::istream& input, const anch::json::MappingOptions& options);

  /*!
   * Check if stream starts with ',' and consumes these characters
   *
   * \param input the input stream to consumes
   * \param options the options to use
   *
   * \return \c true if input stream starts with ',', \c false otherwise
   *
   * \throw plop if stream reach its end
   */
  bool hasMoreField(std::istream& input, const anch::json::MappingOptions& options);

  /*!
   * Consumes unknown field until next field
   *
   * \param input the input stream to parse
   * \param options the options to use
   */
  void consumeUnknownField(std::istream& input, const anch::json::MappingOptions& options);

  /*!
   * Serialize JSON value
   *
   * \tparam the object type
   *
   * \param value the value to serialize
   * \param out the output stream to write in
   * \param serializeFunc the serialization function
   * \param options the options to use
   * \param field the optional field name
   */
  template<typename T>
  bool serialize(const T& value,
		 std::ostream& out,
		 std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
		 const anch::json::MappingOptions& options,
		 const std::optional<std::string>& field);


  /*!
   * Serialize unset value according to options
   *
   * \param out the output stream to write in
   * \param options the options to use
   * \param field the optional field name
   */
  bool
  serializeNull(std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field);

  /*!
   * Serialize JSON pointer value
   *
   * \tparam the object type
   *
   * \param value the value to serialize
   * \param out the output stream to write in
   * \param serializeFunc the serialization function
   * \param options the options to use
   * \param field the optional field name
   */
  template<typename T>
  bool serialize(const T* const value,
		 std::ostream& out,
		 std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
		 const anch::json::MappingOptions& options,
		 const std::optional<std::string>& field);

  /*!
   * Serialize JSON optional value
   *
   * \tparam the object type
   *
   * \param value the value to serialize
   * \param out the output stream to write in
   * \param serializeFunc the serialization function
   * \param options the options to use
   * \param field the optional field name
   */
  template<typename T>
  bool serialize(const std::optional<T>& value,
		 std::ostream& out,
		 std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
		 const anch::json::MappingOptions& options,
		 const std::optional<std::string>& field);

  /*!
   * JSON array deserialization generic implementation
   *
   * \tparam A the container type
   * \tparam T the object type
   *
   * \param array the array to serialize
   * \param out the output stream to write in
   * \param serializeFunc the serialization function
   * \param options the options to use
   * \param field the optional field name
   */
  template<typename T, typename A>
  void serializeArray(const A& array,
		      std::ostream& out,
		      std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
		      const anch::json::MappingOptions& options,
		      const std::optional<std::string>& field);

  /*!
   * JSON map deserialization generic implementation
   *
   * \tparam T the object type
   *
   * \param map the map to serialize
   * \param out the output stream to write in
   * \param serializeFunc the serialization function
   * \param options the options to use
   * \param field the optional field name
   */
  template<typename T>
  void serializeMap(const std::map<std::string,T>& map,
		    std::ostream& out,
		    std::function<void((const T& value, std::ostream& out, const anch::json::MappingOptions& options))> serializeFunc,
		    const anch::json::MappingOptions& options,
		    const std::optional<std::string>& field);

  /*!
   * Deserialize JSON value
   *
   * \param value the value to fill
   * \param input the input stream to parse
   * \param options the options to use
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserialize(T& value,
		   std::istream& input,
		   const anch::json::MappingOptions& options,
		   std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc);

  /*!
   * Deserialize JSON value
   *
   * \param value the value to fill
   * \param input the input stream to parse
   * \param options the options to use
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserialize(T* value,
		   std::istream& input,
		   const anch::json::MappingOptions& options,
		   std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc);

  /*!
   * Deserialize JSON value
   *
   * \param value the value to fill
   * \param input the input stream to parse
   * \param options the options to use
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserialize(std::optional<T>& value,
		   std::istream& input,
		   const anch::json::MappingOptions& options,
		   std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc);

  /*!
   * JSON array deserialization generic implementation
   *
   * \tparam T the object type
   *
   * \param input the input stream to deserialize
   * \param pushFunc the push function according to container type
   * \param options the options to use
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserializeArray(std::istream& input,
			std::function<void(const T&)> pushFunc,
			const anch::json::MappingOptions& options,
			std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc);

  /*!
   * JSON map deserialization generic implementation
   *
   * \tparam T the object type
   *
   * \param input the input stream to deserialize
   * \param pushFunc the push function according to container type
   * \param options the options to use
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserializeMap(std::istream& input,
		      std::function<void(const T&)> pushFunc,
		      const anch::json::MappingOptions& options,
		      std::function<void((T& value, std::istream& input, const anch::json::MappingOptions& options))> deserializeFunc);

}  // anch::json

#include "json/impl/mappingFunctions.hpp"
