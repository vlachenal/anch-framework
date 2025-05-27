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
#include <vector>
#include <list>
#include <set>

#include <functional>

#include "json/constants.hpp"
#include "json/readerContext.hpp"
#include "json/mappingOptions.hpp"

namespace anch::json {

  /*! Serialization function definition */
  template<typename T>
  using SerializeFn = std::function<void(const T&, std::ostream&, const anch::json::MappingOptions&)>;

  /*! Deserialization function definition */
  template<typename T>
  using DeserializeFn = std::function<bool(T&, anch::json::ReaderContext&)>;

  /*!
   * Serialize field name according to value
   *
   * \param out the ouput stream to write in
   * \param field the optional field name to serialize (or not)
   */
  void
  serializeFieldName(std::ostream& out, const std::optional<std::string>& field);

  /*!
   * Serialize field name
   *
   * \param out the ouput stream to write in
   * \param field the field name to serialize
   */
  void
  serializeFieldName(std::ostream& out, const std::string& field);

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
		 anch::json::SerializeFn<T> serializeFunc,
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
		 anch::json::SerializeFn<T> serializeFunc,
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
		 anch::json::SerializeFn<T> serializeFunc,
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
		      anch::json::DeserializeFn<T> serializeFunc,
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
		    anch::json::SerializeFn<T> serializeFunc,
		    const anch::json::MappingOptions& options,
		    const std::optional<std::string>& field);

  /*!
   * Deserialize JSON value
   *
   * \param value the value to fill
   * \param context the mapping context
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserialize(T& value,
		   anch::json::ReaderContext& context,
		   anch::json::DeserializeFn<T> deserializeFunc);

  /*!
   * Deserialize JSON value
   *
   * \param value the value to fill
   * \param context the mapping context
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserialize(T* value,
		   anch::json::ReaderContext& context,
		   anch::json::DeserializeFn<T> deserializeFunc);

  /*!
   * Deserialize JSON value
   *
   * \param value the value to fill
   * \param context the mapping context
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserialize(std::optional<T>& value,
		   anch::json::ReaderContext& context,
		   anch::json::DeserializeFn<T> deserializeFunc);

  /*!
   * Deserialize item and add it to vector if not \c null
   *
   * \param value the vector to add in
   * \param deser the deserialization function
   * \param context the reader context
   */
  template<typename T>
  bool
  addToVector(std::vector<T>& value, anch::json::DeserializeFn<T> deser, anch::json::ReaderContext& context);

  /*!
   * Deserialize item and add it to list if not \c null
   *
   * \param value the list to add in
   * \param deser the deserialization function
   * \param context the reader context
   */
  template<typename T>
  bool
  addToList(std::list<T>& value, anch::json::DeserializeFn<T> deser, anch::json::ReaderContext& context);

  /*!
   * Deserialize item and add it to set if not \c null
   *
   * \param value the set to add in
   * \param deser the deserialization function
   * \param context the reader context
   */
  template<typename T>
  bool
  addToSet(std::set<T>& value, anch::json::DeserializeFn<T> deser, anch::json::ReaderContext& context);

  /*!
   * JSON array deserialization generic implementation
   *
   * \tparam T the object type
   *
   * \param pushFunc the push function according to container type
   * \param context the mapping context
   * \param deserializeFunc the value deserialization function
   */
  // template<typename T>
  // void deserializeArray(std::function<void(const T&)> pushFunc,
  // 			anch::json::Context& context,
  // 			anch::json::DeserializeFn<T> deserializeFunc);

  /*!
   * JSON map deserialization generic implementation
   *
   * \tparam T the object type
   *
   * \param context the mapping context
   * \param pushFunc the push function according to container type
   * \param deserializeFunc the value deserialization function
   */
  template<typename T>
  void deserializeMap(std::function<void(const std::pair<std::string,T>&)> pushFunc,
		      anch::json::ReaderContext& context,
		      anch::json::DeserializeFn<T> deserializeFunc);

}  // anch::json

#include "json/impl/mappingFunctions.hpp"
