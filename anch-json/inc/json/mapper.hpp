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

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include <istream>
#include <functional>
#include <limits>

#include "json/constraints.hpp"
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
   * Type has to be a class or a structure. The fields have to be registered through the registerObject function implementation for the specific type.\n
   * Field constraints ( \ref anch::val::Required , value constraints like \ref anch::val::MinLength , see
   * anch-val, \c val/constraints.hpp ) can be given at registration. They are checked on deserialization
   * ( \c deserialize_check_constraints option, enabled by default) and on serialization
   * ( \c serialize_check_constraints option, disabled by default).\n
   * Errors raised while deserializing or serializing a field value have this field in their path
   * ( \ref MappingError::getPath ).
   *
   * \tparam T the object type
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T>
  class ObjectMapper: public anch::json::GenericMapper<ObjectMapper<T>, T> {
    friend class anch::json::Factory<T>;
    template<typename U>
    friend void anch::json::registerObject(ObjectMapper<U>&);

    // Attributes +
  private:
    /*! Not required field index */
    static constexpr std::size_t NOT_REQUIRED = std::numeric_limits<std::size_t>::max();

    /*! Field reader */
    struct FieldReader {
      /*! Reader function */
      anch::json::DeserializeFn<T> read;

      /*! Index in required fields ( \c NOT_REQUIRED when field is not required) */
      std::size_t required = NOT_REQUIRED;

      /*! Raise error on \c null value (required field which can not hold \c null ) */
      bool rejectNull = false;
    };

    /*! Writer function */
    using WriteFn = std::function<bool(const T&, anch::json::WriterContext&)>;

    /*! Field writer */
    struct FieldWriter {
      /*! JSON key */
      std::string key;

      /*! Writer function */
      WriteFn write;
    };

    /*! JSON writer functions registry */
    std::vector<FieldWriter> _writers;

    /*! Fields validators (fields which can be read: member or getter) */
    std::vector<std::function<void(const T&, anch::val::ValidationContext&)>> _validators;

    /*! Field names comparison (inlined: it is called for each node of each field lookup) */
    struct FieldLess {
      /*!
       * Compare field names (as \c std::string::compare )
       *
       * \param a the first field name
       * \param b the second field name
       *
       * \return \c true when \c a is lower than \c b
       */
      [[gnu::always_inline]] bool operator()(const std::string& a, const std::string& b) const noexcept {
	int res = std::char_traits<char>::compare(a.data(), b.data(), std::min(a.size(), b.size()));
	return res < 0 || (res == 0 && a.size() < b.size());
      }
    };

    /*! JSON reader functions registry */
    std::map<std::string, FieldReader, FieldLess> _readers;

    /*! Required fields (in registration order) */
    std::vector<std::string> _required;
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

    /*!
     * Validate object fields: fields constraints and nested values (used by \ref anch::json::validate )
     *
     * \param value the object to validate
     * \param context the validation context
     *
     * \since 0.2
     */
    void validateValue(const T& value, anch::val::ValidationContext& context) const;

  private:
    /*!
     * Deserialize field value
     *
     * \param value the object to write in
     * \param field the field name
     * \param context the reader context
     *
     * \return the field reader, \c NULL for unknown field
     *
     * \throw anch::json::MappingError deserialization error (the field is added to the error path)
     */
    const FieldReader* readField(T& value, const std::string& field, anch::json::ReaderContext& context) const;

    /*!
     * Deserialize JSON object which has required fields. It is not inlined in \ref deserializeValue to keep its
     * code small when there is no required field.
     *
     * \param value the value to write in
     * \param context the reader context
     *
     * \throw anch::json::MappingError deserialization error or missing required field
     */
    [[gnu::noinline]] void deserializeRequired(T& value, anch::json::ReaderContext& context) const;

    /*!
     * Register field writer
     *
     * \tparam G the value getter type
     * \tparam W the value writer type
     * \tparam C the constraints types
     *
     * \param key the JSON key
     * \param get the value getter (from object)
     * \param write the value writer
     * \param constraints the field constraints: checked before writing value (when \c serialize_check_constraints
     *        option is \c true ), required fields are always written. The field validator
     *        ( \ref anch::json::validate ) is registered too.
     */
    template<typename G, typename W, typename... C>
    void addWriter(const std::string& key, G get, W write, const C&... constraints);

    /*!
     * Register field reader
     *
     * \param key the JSON key
     * \param reader the reader function
     * \param required the field is required
     * \param nullable the field can hold \c null value
     */
    void addReader(const std::string& key, anch::json::DeserializeFn<T>&& reader, bool required, bool nullable);

    /*!
     * Add value constraints check to field reader
     *
     * \tparam R the reader type
     * \tparam P the field type
     * \tparam C the constraints types
     *
     * \param read the field reader
     * \param value the field
     * \param constraints the field constraints
     *
     * \return the reader which checks value constraints after reading (the reader itself without value constraint)
     */
    template<typename R, typename P, typename... C>
    static anch::json::DeserializeFn<T> withConstraints(R& read, P T::* value, C&&... constraints);

    /*!
     * Register mapping by field
     *
     * \tparam P the parameter type
     * \tparam C the constraints types
     *
     * \param key the JSON key
     * \param value the JSON value in object
     * \param constraints the field constraints ( \ref anch::val::required , \ref anch::val::minLength , ...)
     *
     * \return \c this
     */
    template<typename P, typename... C>
    requires (anch::val::FieldConstraint<C,P> && ...)
    ObjectMapper<T>& registerField(const std::string& key, P T::* value, C&&... constraints);

    /*!
     * Register mapping by field specifying the mapper to use
     *
     * \tparam P the parameter type
     * \tparam MT the mapper type to use for this parameter
     * \tparam C the constraints types
     *
     * \param key the JSON key
     * \param value the JSON value in object
     * \param constraints the field constraints ( \ref anch::val::required , \ref anch::val::minLength , ...)
     *
     * \return \c this
     */
    template<typename MT, typename P, typename... C>
    requires (anch::val::FieldConstraint<C,P> && ...)
    ObjectMapper<T>& registerField(const std::string& key, P T::* value, C&&... constraints);

    /*!
     * Register mapping by field
     *
     * \tparam P the parameter type
     * \tparam MT the mapper type to use for this parameter (default to parameter type)
     * \tparam C the constraints types
     *
     * \param key the JSON key
     * \param getter the JSON value getter function
     * \param constraints the field constraints, checked on serialization
     *
     * \return \c this
     */
    template<typename P, typename MT = P, typename... C>
    requires (anch::val::FieldConstraint<C,P> && ...)
    ObjectMapper<T>& registerField(const std::string& key, std::function<P(const T&)> getter, const C&... constraints);

    /*!
     * Register mapping by field
     *
     * \tparam P the parameter type
     * \tparam MT the mapper type to use for this parameter (default to parameter type)
     * \tparam C the constraints types
     *
     * \param key the JSON key
     * \param setter the JSON value setter function
     * \param constraints the field constraints ( \ref anch::val::required , \ref anch::val::minLength , ...)
     *
     * \return \c this
     */
    template<typename P, typename MT = P, typename... C>
    requires (anch::val::FieldConstraint<C,P> && ...)
    ObjectMapper<T>& registerField(const std::string& key, std::function<void(T&, const P&)> setter, C&&... constraints);

    /*!
     * Register mapping by field
     *
     * \tparam P the parameter type
     * \tparam MT the mapper type to use for this parameter (default to parameter type)
     * \tparam C the constraints types
     *
     * \param key the JSON key
     * \param getter the JSON value getter function
     * \param setter the JSON value setter function
     * \param constraints the field constraints ( \ref anch::val::required , \ref anch::val::minLength , ...), checked on deserialization
     *
     * \return \c this
     */
    template<typename P, typename MT = P, typename... C>
    requires (anch::val::FieldConstraint<C,P> && ...)
    ObjectMapper<T>& registerField(const std::string& key, std::function<P(const T&)> getter, std::function<void(T&, const P&)> setter, C&&... constraints);
    // Methods -

  };

}  // anch::json

#include "json/impl/mapper.hpp"
