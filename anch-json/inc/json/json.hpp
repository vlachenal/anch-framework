/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "anch-json-export.hpp"

#include <ostream>
#include <istream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

#include "json/mapper.hpp"
#include "json/mappingOptions.hpp"

#include "flux.hpp"


namespace anch::json {

  // Serialization functions +
  // Functions which write result in output stream +
  /*!
   * Serialize object
   *
   * \tparam T the object type
   *
   * \param value the object to serialize
   * \param out the output stream to write in
   * \param options the options to use
   */
  template<typename T>
  void serialize(const T& value,
		 std::ostream& out,
		 const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Serialize objects (it can also work with \c std::optional, \c std::shared_ptr, ...)
   *
   * \tparam T the object type
   * \tparam C the container's type
   *
   * \param value the objects to serialize
   * \param out the output stream to write in
   * \param options the options to use
   */
  template<template<typename> typename C, typename T>
  void serialize(const C<T>& value,
		 std::ostream& out,
		 const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Serialize object's flux.\n
   * Error handler has to be set before this method call when needed.
   * Consumer and finalize functions are set in this method.\n
   * This method will lock the processing until \ref anch::Flux::finalize method has not be called (or error happened and throw itself).\n
   *
   * This function is implemented in header imkplementation instead uses \ref GenericMapper implementation.
   *
   * \tparam T the object type
   *
   * \param value the objects to serialize
   * \param out the output stream to write in
   * \param options the options to use
   */
  template<typename T>
  void serialize(anch::Flux<T>& value,
		 std::ostream& out,
		 const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Serialize objects
   *
   * \tparam T the object type
   *
   * \param value the objects to serialize
   * \param out the output stream to write in
   * \param options the options to use
   */
  template<typename T>
  void serialize(const std::map<std::string,T>& value,
		 std::ostream& out,
		 const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);
  // Functions which write result in output stream -

  // Functions which returns std::string +
  /*!
   * Serialize object
   *
   * \tparam T the object type
   *
   * \param value the object to serialize
   * \param options the options to use
   *
   * \return the serialized object as \c std::string
   */
  template<typename T>
  std::string serialize(const T& value,
			const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Serialize objects (it can also work with \c std::optional, \c std::shared_ptr, ...)
   *
   * \tparam T the object type
   * \tparam C the container's type
   *
   * \param value the object to serialize
   * \param options the options to use
   *
   * \return the serialized objects as \c std::string
   */
  template<template<typename> typename C, typename T>
  std::string serialize(const C<T>& value,
			const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Serialize objects
   *
   * \tparam T the object type
   *
   * \param value the object to serialize
   * \param options the options to use
   *
   * \return the serialized objects as \c std::string
   */
  template<typename T>
  std::string serialize(const std::map<std::string,T>& value,
			const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);
  // Functions which returns std::string -
  // Serialization functions -

  // Deserialization functions +
  /*!
   * Deserialize object
   *
   * \tparam T the object type
   *
   * \param value the value to fill
   * \param input the input stream to parse
   * \param options the options to use
   */
  template<typename T>
  void deserialize(T& value,
		   std::istream& input,
		   const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Deserialize object
   *
   * \tparam T the object type
   *
   * \param input the input stream to parse
   * \param options the options to use
   *
   * \return the deserialized object by copy
   */
  template<typename T>
  T deserialize(std::istream& input,
		const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Deserialize objects (it can also work with \c std::optional, \c std::shared_ptr, ...)
   *
   * \tparam T the object's type
   * \tparam C the container's type
   *
   * \param values the values to fill
   * \param input the input stream to parse
   * \param options the options to use
   */
  template<template<typename> typename C, typename T>
  void deserialize(C<T>& values,
		   std::istream& input,
		   const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Deserialize JSON array into objects' flux: each object is pushed to the flux consumer, then flux is finalized.\n
   * Consumer (and error handler when needed) have to be set before this method call.
   *
   * \tparam T the object type
   *
   * \param values the flux to push values in
   * \param input the input stream to parse
   * \param options the options to use
   *
   * \since 0.2
   */
  template<typename T>
  void deserialize(anch::Flux<T>& values,
		   std::istream& input,
		   const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * Deserialize objects
   *
   * \tparam T the object type
   *
   * \param values the values to fill
   * \param input the input stream to parse
   * \param options the options to use
   */
  template<typename T>
  void deserialize(std::map<std::string,T>& values,
		   std::istream& input,
		   const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);
  // Deserialization functions -

  // JSON mapper +
  /*!
   * \brief JSON mapper with mapping options
   *
   * JSON mapper declaration which will always use same mapping options
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class ANCH_JSON_EXPORT JSONMapper {

    // Attributes +
  private:
    /*! Mapping options */
    anch::json::MappingOptions _options;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref JSONMapper default constructor
     */
    JSONMapper() = delete;

    /*!
     * \ref JSONMapper constructor
     *
     * \param options the mapping options
     */
    JSONMapper(const anch::json::MappingOptions& options);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref JSONMapper destructor
     */
    virtual ~JSONMapper();
    // Destructor +

    // Methods +
  public:
    // Serialization methods +
    /*!
     * Serialize object
     *
     * \tparam T the object type
     *
     * \param value the object to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const T& value, std::ostream& out);

    /*!
     * Serialize objects (it can also work with \c std::optional, \c std::shared_ptr, ...)
     *
     * \tparam T the object's type
     * \tparam C the container's type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<template<typename> typename C, typename T>
    void serialize(const C<T>& value, std::ostream& out);

    /*!
     * Serialize object's flux.\n
     * Error handler has to be set before this method call when needed.
     * Consumer and finalize functions are set in this method.\n
     * This method will lock the processing until \ref anch::Flux::finalize method has not be called (or error happened and throw itself).
     *
     * \tparam T the object type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(anch::Flux<T>& value, std::ostream& out);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const std::map<std::string,T>& value, std::ostream& out);

    /*!
     * Serialize object
     *
     * \tparam T the object type
     *
     * \param value the object to serialize
     *
     * \return the serialized object as \c std::string
     */
    template<typename T>
    std::string serialize(const T& value);

    /*!
     * Serialize objects (it can also work with \c std::optional, \c std::shared_ptr, ...)
     *
     * \tparam T the object's type
     * \tparam C the container's type
     *
     * \param value the object to serialize
     *
     * \return the serialized objects as \c std::string
     */
    template<template<typename> typename C, typename T>
    std::string serialize(const C<T>& value);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the object to serialize
     *
     * \return the serialized objects as \c std::string
     */
    template<typename T>
    std::string serialize(const std::map<std::string,T>& value);
    // Serialization methods -

    // Deserialization methods +
    /*!
     * Deserialize object
     *
     * \tparam T the object type
     *
     * \param value the value to fill
     * \param input the input stream to parse
     */
    template<typename T>
    void deserialize(T& value, std::istream& input);

    /*!
     * Deserialize object
     *
     * \tparam T the object type
     *
     * \param input the input stream to parse
     *
     * \return the deserialized object by copy
     */
    template<typename T>
    T deserialize(std::istream& input);

    /*!
     * Deserialize objects (it can also work with \c std::optional, \c std::shared_ptr, ...)
     *
     * \tparam T the object's type
     * \tparam C the container's type
     *
     * \param values the values to fill
     * \param input the input stream to parse
     */
    template<template<typename> typename C, typename T>
    void deserialize(C<T>& values, std::istream& input);

    /*!
     * Deserialize JSON array into objects' flux (see \ref anch::json::deserialize )
     *
     * \tparam T the object type
     *
     * \param values the flux to push values in
     * \param input the input stream to parse
     *
     * \since 0.2
     */
    template<typename T>
    void deserialize(anch::Flux<T>& values, std::istream& input);

    /*!
     * Deserialize objects
     *
     * \tparam T the object type
     *
     * \param values the values to fill
     * \param input the input stream to parse
     */
    template<typename T>
    void deserialize(std::map<std::string,T>& values, std::istream& input);
    // Deserialization methods -
    // Methods -

  };
  // JSON mapper -

}  // anch::json

#include "json/impl/jsonSerialize.hpp" // Serialization functions implementation
#include "json/impl/jsonDeserialize.hpp" // Deserialization functions implementation
#include "json/impl/jsonMapper.hpp" // JSONMapper methods implementation
#include "json/validation.hpp" // Validation functions
