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

#include <functional>
#include "json/constants.hpp"

namespace anch {
  namespace json {

    /*!
     * Consumes discarded characters
     *
     * \param input the input stream to consumes
     *
     * \throw plop if stream reach its end
     */
    void discardChars(std::istream& input);

    /*!
     * Check if stream starts with 'null' and consumes these characters
     *
     * \param input the input stream to consumes
     *
     * \return \c true if input stream starts with 'null', \c false otherwise
     *
     * \throw plop if stream reach its end
     */
    bool isNull(std::istream& input);

    /*!
     * Parse input to get new field definition.\n
     * This will consume '"<field name>":'
     *
     * \param input the input stream to parse
     *
     * \return the field's name if found, \c empty otherwise
     */
    std::optional<std::string> getFieldName(std::istream& input);

    /*!
     * Check if stream starts with ',' and consumes these characters
     *
     * \param input the input stream to consumes
     *
     * \return \c true if input stream starts with ',', \c false otherwise
     *
     * \throw plop if stream reach its end
     */
    bool hasMoreField(std::istream& input);

    /*!
     * Serialize JSON value
     *
     * \tparam the object type
     *
     * \param value the value to serialize
     * \param out the output stream to write in
     * \param serializeFunc the serialization function
     * \param field the optional field name
     */
    template<typename T>
    bool serialize(const T& value,
		   std::ostream& out,
		   std::function<void((const T& value, std::ostream& out))> serializeFunc,
		   const std::optional<std::string>& field);

    /*!
     * Serialize JSON pointer value
     *
     * \tparam the object type
     *
     * \param value the value to serialize
     * \param out the output stream to write in
     * \param serializeFunc the serialization function
     * \param field the optional field name
     */
    template<typename T>
    bool serialize(const T* const value,
		   std::ostream& out,
		   std::function<void((const T& value, std::ostream& out))> serializeFunc,
		   const std::optional<std::string>& field);

    /*!
     * Serialize JSON optional value
     *
     * \tparam the object type
     *
     * \param value the value to serialize
     * \param out the output stream to write in
     * \param serializeFunc the serialization function
     * \param field the optional field name
     */
    template<typename T>
    bool serialize(const std::optional<T>& value,
		   std::ostream& out,
		   std::function<void((const T& value, std::ostream& out))> serializeFunc,
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
     */
    template<typename T, typename A>
    void serializeArray(const A& array,
			std::ostream& out,
			std::function<void((const T& value, std::ostream& out))> serializeFunc,
			const std::optional<std::string>& field);

    /*!
     * JSON array deserialization generic implementation
     *
     * \tparam T the object type
     *
     * \param input the input stream to deserialize
     * \param pushFunc the push function according to container type
     * \param deserializeFunc the value deserialization function
     */
    template<typename T>
    void deserializeArray(std::istream& input,
			  std::function<auto(const T&)> pushFunc,
			  std::function<void((T& value, std::istream& input))> deserializeFunc);

    class JSONParser {
    private:
      /*! Input stream to parse */
      std::istream& _input;

      // Constructors +
    public:
      /*!
       * Forbids \ref JSONParser default constructor
       */
      JSONParser() = delete;

      /*!
       * Forbids \ref JSONParser copy constructor
       *
       * \param other the \ref JSONParser not to copy
       */
      JSONParser(const JSONParser& other) = delete;

      /*!
       * Forbids \ref JSONParser move constructor
       *
       * \param other the \ref JSONParser not to move
       */
      JSONParser(JSONParser&& other) = delete;

      /*!
       * \ref JSONParser constructor
       *
       * \param input the input stream to parse
       */
      JSONParser(std::istream& input);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref JSONParser destructor
       */
      virtual ~JSONParser();
      // Destructor +

    };

  }  // json
}  // anch

#include "json/impl/parser.hpp"
