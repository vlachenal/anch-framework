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

#include <ostream>
#include <string>
#include <iomanip>

#include "json/mapper.hpp"


namespace anch::json {

  /*!
   * Fields registry function\n
   * This function has to be specialized for each class/structure which has to be serialized/deserialized.
   *
   * \tparam T the object type
   *
   * \param mapper the \ref ObjectMapper to use
   */
  template<typename T>
  void registerObject(anch::json::ObjectMapper<T>& mapper);

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
  void serialize(const T& value, std::ostream& out, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  void serialize(const std::vector<T>& value, std::ostream& out, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  void serialize(const std::list<T>& value, std::ostream& out, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  void serialize(const std::set<T>& value, std::ostream& out, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  std::string serialize(const T& value, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  std::string serialize(const std::vector<T>& value, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  std::string serialize(const std::list<T>& value, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  std::string serialize(const std::set<T>& value, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  void deserialize(T& value, std::istream& input, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  T deserialize(std::istream& input, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  void deserialize(std::vector<T>& values, std::istream& input, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  void deserialize(std::list<T>& values, std::istream& input, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

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
  void deserialize(std::set<T>& values, std::istream& input, const anch::json::MappingOptions& options = anch::json::DEFAULT_MAPPING_OPTIONS);

  /*!
   * \brief JSON mapper with mapping options
   *
   * JSON mapper declaration which will always use same mapping options
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class JSONMapper {

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
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const std::vector<T>& value, std::ostream& out);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const std::list<T>& value, std::ostream& out);

    /*!
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the objects to serialize
     * \param out the output stream to write in
     */
    template<typename T>
    void serialize(const std::set<T>& value, std::ostream& out);

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
     * Serialize objects
     *
     * \tparam T the object type
     *
     * \param value the object to serialize
     *
     * \return the serialized objects as \c std::string
     */
    template<typename T>
    std::string serialize(const std::vector<T>& value);

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
    std::string serialize(const std::list<T>& value);

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
    std::string serialize(const std::set<T>& value);

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

    /*!
     * Deserialize object
     *
     * \tparam T the object type
     *
     * \param value the value to fill
     * \param input the input stream to parse
     *
     * \return \c true when value has been found, \c false otherwise
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
     * Deserialize objects
     *
     * \tparam T the object type
     *
     * \param values the values to fill
     * \param input the input stream to parse
     *
     * \return \c true
     */
    template<typename T>
    void deserialize(std::vector<T>& values, std::istream& input);

    /*!
     * Deserialize objects
     *
     * \tparam T the object type
     *
     * \param values the values to fill
     * \param input the input stream to parse
     *
     * \return \c true
     */
    template<typename T>
    void deserialize(std::list<T>& values, std::istream& input);

    /*!
     * Deserialize objects
     *
     * \tparam T the object type
     *
     * \param values the values to fill
     * \param input the input stream to parse
     *
     * \return \c true
     */
    template<typename T>
    void deserialize(std::set<T>& values, std::istream& input);

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
    // Methods -

  };

}  // anch::json

#include "json/impl/json.hpp"
