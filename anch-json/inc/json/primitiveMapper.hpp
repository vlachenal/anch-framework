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
#include <vector>
#include <map>
#include <ostream>
#include <functional>
#include <list>
#include <set>

#include "json/constants.hpp"
#include "json/mappingOptions.hpp"


namespace anch::json {

  // JSON mapper early declaration
  template<typename T>
  class Factory;

  /*!
   * \brief JSON primitive types mapper
   *
   * JSON primitive types does not have any fields.
   *
   * \tparam T the object type
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T>
  class PrimitiveMapper {
    friend anch::json::Factory<T>;

    // Constructors +
  private:
    /*!
     * \ref PrimitiveMapper default private constructor
     */
    PrimitiveMapper();

  public:
    /*!
     * Forbids \ref PrimitiveMapper copy constructor
     *
     * \param other the other mapper not to copy
     */
    PrimitiveMapper(const PrimitiveMapper& other) = delete;

    /*!
     * Forbids \ref PrimitiveMapper move constructor
     *
     * \param other the other mapper not to move
     */
    PrimitiveMapper(PrimitiveMapper&& other) = delete;
    // Constructors -

  private:
    // Destructors +
    /*!
     * \ref PrimitiveMapper private destructor
     */
    virtual ~PrimitiveMapper();
    // Destructors -

    // Methods +
  public:
    /*!
     * Serialize reference attribute
     *
     * \param value the reference attribute to serialize
     * \param out the output stream to write the attribute
     * \param options the options to use
     * \param field the attribute's field name
     *
     * \return \c true
     */
    bool serialize(const T& value,
		   std::ostream& out,
		   const anch::json::MappingOptions& options,
		   const std::optional<std::string>& field = EMPTY_FIELD);

    /*!
     * Serialize pointer attribute
     *
     * \param value the pointer attribute to serialize
     * \param out the output stream to write the attribute
     * \param options the options to use
     * \param field the attribute's field name
     *
     * \return \c true if attribute is not \c NULL , \c false otherwise
     */
    bool serialize(const T* const value,
		   std::ostream& out,
		   const anch::json::MappingOptions& options,
		   const std::optional<std::string>& field = EMPTY_FIELD);

    /*!
     * Serialize optional attribute
     *
     * \param value the pointer attribute to serialize
     * \param out the output stream to write the attribute
     * \param options the options to use
     * \param field the attribute's field name
     *
     * \return \c true if attribute is set , \c false otherwise
     */
    bool serialize(const std::optional<T>& value,
		   std::ostream& out,
		   const anch::json::MappingOptions& options,
		   const std::optional<std::string>& field = EMPTY_FIELD);

    /*!
     * Serialize array (as \c std::vector ) attribute
     *
     * \param value the array attribute to serialize
     * \param out the output stream to write the attribute
     * \param options the options to use
     * \param field the attribute's field name
     *
     * \return \c true
     */
    bool serialize(const std::vector<T>& value,
		   std::ostream& out,
		   const anch::json::MappingOptions& options,
		   const std::optional<std::string>& field = EMPTY_FIELD);

    /*!
     * Serialize array (as \c std::list ) attribute
     *
     * \param value the array attribute to serialize
     * \param out the output stream to write the attribute
     * \param options the options to use
     * \param field the attribute's field name
     *
     * \return \c true
     */
    bool serialize(const std::list<T>& value,
		   std::ostream& out,
		   const anch::json::MappingOptions& options,
		   const std::optional<std::string>& field = EMPTY_FIELD);

    /*!
     * Serialize array (as \c std::set ) attribute
     *
     * \param value the array attribute to serialize
     * \param out the output stream to write the attribute
     * \param options the options to use
     * \param field the attribute's field name
     *
     * \return \c true
     */
    bool serialize(const std::set<T>& value,
		   std::ostream& out,
		   const anch::json::MappingOptions& options,
		   const std::optional<std::string>& field = EMPTY_FIELD);

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param input the input stream to parse
     * \param options the options to use
     */
    void deserialize(T& value, std::istream& input, const anch::json::MappingOptions& options);

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param input the input stream to parse
     * \param options the options to use
     */
    void deserialize(std::optional<T>& value, std::istream& input, const anch::json::MappingOptions& options);

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param input the input stream to parse
     * \param options the options to use
     */
    void deserialize(T* value, std::istream& input, const anch::json::MappingOptions& options);

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param input the input stream to parse
     * \param options the options to use
     */
    void deserialize(std::vector<T>& value, std::istream& input, const anch::json::MappingOptions& options);

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param input the input stream to parse
     * \param options the options to use
     */
    void deserialize(std::list<T>& value, std::istream& input, const anch::json::MappingOptions& options);

    /*!
     * Deserialize JSON value
     *
     * \param value the value to set
     * \param input the input stream to parse
     * \param options the options to use
     */
    void deserialize(std::set<T>& value, std::istream& input, const anch::json::MappingOptions& options);
    // Methods -

  };

}  // anch::json
