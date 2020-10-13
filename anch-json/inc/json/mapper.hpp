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
#include <istream>
#include <functional>
#include <list>
#include <set>

#include "json/constants.hpp"


namespace anch {
  namespace json {

    // JSON mapper early declaration
    template<typename T>
    class JSONMapper;

    // JSON mapper early declaration
    template<typename T>
    class JSONFactory;

    /*!
     * Fields registry function\n
     * This function has to be specialized for each class/structure which has to be serialized/deserialized.
     *
     * \param mapper the \ref JSONMapper to use
     */
    template<typename T>
    void registerFields(JSONMapper<T>& mapper);

    /*!
     * \brief JSON complex object mapper
     *
     * This is a generic template header implementation.\n
     * Type has to be a class or a structure. The fields have to be registered through the registerFields function implementation for the specific type.
     *
     * \tparam T the object type
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    template<typename T>
    class JSONMapper {
      friend anch::json::JSONFactory<T>;
      template<typename U>
      friend void anch::json::registerFields(JSONMapper<U>&);

      // Attributes +
    private:
      /*! JSON writer functions registry */
      std::vector<std::function<bool(const T&, std::ostream&)>> _writers;

      /*! JSON reader functions registry */
      std::map<std::string, std::function<void(T&, std::istream&)>> _readers;
      // Attributes -

      // Constructors +
    private:
      /*!
       * \ref JSONMapper default private constructor
       */
      JSONMapper();

    public:
      /*!
       * Forbids \ref JSONMapper copy constructor
       */
      JSONMapper(const JSONMapper& other) = delete;

      /*!
       * Forbids \ref JSONMapper move constructor
       */
      JSONMapper(JSONMapper&& other) = delete;
      // Constructors -

    private:
      // Destructors +
      /*!
       * \ref JSONMapper private destructor
       */
      virtual ~JSONMapper();
      // Destructors -

      // Methods +
    private:
      /*!
       * Register mapping by field
       *
       * \tparam P the parameter type
       *
       * \param key the JSON key
       * \param value the JSON value in object
       *
       * \return \c this
       */      template<typename P>
      JSONMapper<T>& registerField(const std::string& key, P T::* value);

      /*!
       * Register mapping by field specifying the mapper to use
       *
       * \tparam P the parameter type
       * \tparam MT the mapper type to use for this parameter
       *
       * \param key the JSON key
       * \param value the JSON value in object
       *
       * \return \c this
       */
      template<typename MT, typename P>
      JSONMapper<T>& registerField(const std::string& key, P T::* value);

      /*!
       * Register mapping by field
       *
       * \tparam P the parameter type
       * \tparam MT the mapper type to use for this parameter (default to parameter type)
       *
       * \param key the JSON key
       * \param getter the JSON value getter function
       *
       * \return \c this
       */
      template<typename P, typename MT = P>
      JSONMapper<T>& registerField(const std::string& key, std::function<P(const T&)> getter);

      /*!
       * Register mapping by field
       *
       * \tparam P the parameter type
       * \tparam MT the mapper type to use for this parameter (default to parameter type)
       *
       * \param key the JSON key
       * \param setter the JSON value setter function
       *
       * \return \c this
       */
      template<typename P, typename MT = P>
      JSONMapper<T>& registerField(const std::string& key, std::function<void(T&, const P&)> setter);

      /*!
       * Register mapping by field
       *
       * \tparam P the parameter type
       * \tparam MT the mapper type to use for this parameter (default to parameter type)
       *
       * \param key the JSON key
       * \param getter the JSON value getter function
       * \param setter the JSON value setter function
       *
       * \return \c this
       */
      template<typename P, typename MT = P>
      JSONMapper<T>& registerField(const std::string& key, std::function<P(const T&)> getter, std::function<void(T&, const P&)> setter);

    public:
      /*!
       * Serialize reference attribute
       *
       * \param value the reference attribute to serialize
       * \param out the output stream to write the attribute
       * \param field the attribute's field name
       *
       * \return \c true
       */
      bool serialize(const T& value, std::ostream& out, const std::optional<std::string>& field = EMPTY_FIELD);

      /*!
       * Serialize pointer attribute
       *
       * \param value the pointer attribute to serialize
       * \param out the output stream to write the attribute
       * \param field the attribute's field name
       *
       * \return \c true if attribute is not \c NULL, \c false otherwise
       */
      bool serialize(const T* const value, std::ostream& out, const std::optional<std::string>& field = EMPTY_FIELD);

      /*!
       * Serialize optional attribute
       *
       * \param value the optional attribute to serialize
       * \param out the output stream to write the attribute
       * \param field the attribute's field name
       *
       * \return \c true if attribute is not empty, \c false otherwise
       */
      bool serialize(const std::optional<T>& value, std::ostream& out, const std::optional<std::string>& field = EMPTY_FIELD);

      /*!
       * Serialize array (as \c std::vector ) attribute
       *
       * \param value the array attribute to serialize
       * \param out the output stream to write the attribute
       * \param field the attribute's field name
       *
       * \return \c true
       */
      bool serialize(const std::vector<T>& value, std::ostream& out, const std::optional<std::string>& field = EMPTY_FIELD);

      /*!
       * Serialize array (as \c std::list ) attribute
       *
       * \param value the array attribute to serialize
       * \param out the output stream to write the attribute
       * \param field the attribute's field name
       *
       * \return \c true
       */
      bool serialize(const std::list<T>& value, std::ostream& out, const std::optional<std::string>& field = EMPTY_FIELD);

      /*!
       * Serialize array (as \c std::set ) attribute
       *
       * \param value the array attribute to serialize
       * \param out the output stream to write the attribute
       * \param field the attribute's field name
       *
       * \return \c true
       */
      bool serialize(const std::set<T>& value, std::ostream& out, const std::optional<std::string>& field = EMPTY_FIELD);

      /*!
       * Deserialize JSON
       *
       * \param input the input stream to parse
       * \param field the field
       */
      void deserialize(T& value, std::istream& input);

      /*!
       * Deserialize JSON
       *
       * \param input the input stream to parse
       * \param field the field
       */
      void deserialize(std::optional<T>& value, std::istream& input);

      /*!
       * Deserialize JSON
       *
       * \param input the input stream to parse
       * \param field the field
       */
      void deserialize(T* value, std::istream& input);

      /*!
       * Deserialize JSON
       *
       * \param input the input stream to parse
       * \param field the field
       */
      void deserialize(std::vector<T>& value, std::istream& input);

      /*!
       * Deserialize JSON
       *
       * \param input the input stream to parse
       * \param field the field
       */
      void deserialize(std::list<T>& value, std::istream& input);

      /*!
       * Deserialize JSON
       *
       * \param input the input stream to parse
       * \param field the field
       */
      void deserialize(std::set<T>& value, std::istream& input);

    private:
      /*!
       * Deserialize non null JSON object (which should start with '{')
       *
       * \param input the input stream to parse
       * \param field the field
       */
      void deserializeNonNull(T& value, std::istream& input);

      /*!
       * Desrialize array
       *
       * \param input the input stream to parse
       * \param pushFunc the push function to invoke to add item into array
       */
      void deserializeArray(std::istream& input, std::function<void(const T&)> pushFunc);
      // Methods -

      // Accessors +
    public:
      /*!
       * Writter mapping getter
       *
       * \return the writter mapping
       */
      const std::vector<std::function<bool(const T&, std::ostream&)>>& getWritterMapping() const;

      /*!
       * Reader mapping getter
       *
       * \return the reader mapping
       */
      const std::map<std::string, std::function<void(T&, std::istream&)>>& getReaderMapping() const;
      // Accessors +

    };

  } // json
}  // anch

#include "json/impl/mapper.hpp"
