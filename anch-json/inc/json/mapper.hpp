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
#include <map>

#include "json/genericMapper.hpp"
#include "json/mappingOptions.hpp"


namespace anch::json {

  // JSON mapper early declaration
  template<typename T>
  class ObjectMapper;

  // JSON mapper early declaration
  template<typename T>
  class Factory;

  /*!
   * Fields registry function\n
   * This function has to be specialized for each class/structure which has to be serialized/deserialized.
   *
   * \tparam T the object type
   *
   * \param mapper the \ref ObjectMapper to use
   */
  template<typename T>
  void registerObject(ObjectMapper<T>& mapper);

   /*! Serialization function definition */
   template<typename T>
   using SerializeFn = std::function<void(const T&, std::ostream&, const anch::json::MappingOptions&)>;

   /*! Deserialization function definition */
   template<typename T>
   using DeserializeFn = std::function<bool(T&, anch::json::ReaderContext&)>;

  /*!
   * \brief JSON complex object mapper
   *
   * This is a generic template header implementation.\n
   * Type has to be a class or a structure. The fields have to be registered through the registerObject function implementation for the specific type.
   *
   * \tparam T the object type
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T>
  class ObjectMapper: public anch::json::GenericMapper<ObjectMapper<T>, T> {
    friend anch::json::Factory<T>;
    template<typename U>
    friend void anch::json::registerObject(ObjectMapper<U>&);

    // Attributes +
  private:
    /*! JSON writer functions registry */
    std::vector<std::function<bool((const T&, anch::json::WriterContext&))>> _writers;

    /*! JSON reader functions registry */
    std::map<std::string, anch::json::DeserializeFn<T>> _readers;
    // Attributes -

    // Constructors +
  private:
    /*!
     * \ref ObjectMapper default private constructor
     */
    ObjectMapper();

  public:
    /*!
     * Forbids \ref ObjectMapper copy constructor
     */
    ObjectMapper(const ObjectMapper& other) = delete;

    /*!
     * Forbids \ref ObjectMapper move constructor
     */
    ObjectMapper(ObjectMapper&& other) = delete;
    // Constructors -

  private:
    // Destructors +
    /*!
     * \ref ObjectMapper private destructor
     */
    virtual ~ObjectMapper();
    // Destructors -

    // Methods +
  public:
    /*!
     * Serialize value
     *
     * \param value the reference attribute to serialize
     * \param context the writer context
     */
    void serializeValue(const T& value, anch::json::WriterContext& context) const;

    /*!
     * Deserialize JSON
     *
     * \param value the value to write in
     * \param context the mapper context
     *
     * \return \c false when \c null found, \c true otherwise
     */
    bool deserializeValue(T& value, anch::json::ReaderContext& context) const;

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
     */
    template<typename P>
    ObjectMapper<T>& registerField(const std::string& key, P T::* value);

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
    ObjectMapper<T>& registerField(const std::string& key, P T::* value);

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
    ObjectMapper<T>& registerField(const std::string& key, std::function<P(const T&)> getter);

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
    ObjectMapper<T>& registerField(const std::string& key, std::function<void(T&, const P&)> setter);

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
    ObjectMapper<T>& registerField(const std::string& key, std::function<P(const T&)> getter, std::function<void(T&, const P&)> setter);
    // Methods -

  };

}  // anch::json

#include "json/impl/mapper.hpp"
