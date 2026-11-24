/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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

#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "mapping.hpp"
#include "sql/parameters.hpp"

namespace anch::sql {

  /*!
   * \brief Parameters of a described type: the common description of objects ( \c anch::Mapping<T> , anch-utils)
   * registered for SQL
   *
   * Each field which has a value (member or getter) is a parameter named as the field: fields which are only
   * written (setter) are ignored. Fields values have to be bindable ( \ref Bindable : integers, floating point
   * numbers, strings, dates (\c anch::date::Date , \c anch::date::DateTime , \c anch::date::Time ), \c std::optional and lists of them): a field of another type is refused at compile
   * time.
   * Constraints are not checked (validate objects with \c anch::val::validate ).
   *
   * \tparam T the described type
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<anch::Described T>
  class FieldsMapper {

  public:
    /*!
     * \brief Field which has a value
     */
    struct Field {
      /*! Field name (parameter name) */
      std::string name;

      /*! Field value reader */
      std::function<anch::sql::BoundValue(const T&)> read;
    };

    // Attributes +
  private:
    /*! Fields, in registration order */
    std::vector<Field> _fields;
    // Attributes -

    // Constructors +
  private:
    /*!
     * \ref FieldsMapper constructor: registers the fields of the description
     */
    FieldsMapper();
    // Constructors -

    // Methods +
  public:
    /*!
     * Mapper of \c T (built once)
     *
     * \return the mapper
     */
    static const FieldsMapper<T>& instance();

    /*!
     * Find field
     *
     * \param name the field name
     *
     * \return the field, \c nullptr when \c T has no such field (or when it is only written)
     */
    const Field* find(std::string_view name) const noexcept;

    /*!
     * Fields getter
     *
     * \return the fields which have a value, in registration order
     */
    const std::vector<Field>& fields() const noexcept;

    // Fields registration (anch::ObjectMapper concept ; constraints are ignored) +
    /*!
     * Register field by member
     *
     * \tparam P the field type
     * \tparam C the constraints types
     *
     * \param name the field name
     * \param member the member
     *
     * \return \c this
     */
    template<typename P, typename... C>
    FieldsMapper<T>& registerField(const std::string& name, P T::* member, C&&...);

    /*!
     * Register field by getter
     *
     * \tparam P the field type
     * \tparam C the constraints types
     *
     * \param name the field name
     * \param getter the getter
     *
     * \return \c this
     */
    template<typename P, typename... C>
    FieldsMapper<T>& registerField(const std::string& name, std::function<P(const T&)> getter, C&&...);

    /*!
     * Register field by setter: ignored (the field has no value)
     *
     * \tparam P the field type
     * \tparam C the constraints types
     *
     * \return \c this
     */
    template<typename P, typename... C>
    FieldsMapper<T>& registerField(const std::string&, std::function<void(T&, const P&)>, C&&...);

    /*!
     * Register field by getter and setter (the getter gives the value)
     *
     * \tparam P the field type
     * \tparam C the constraints types
     *
     * \param name the field name
     * \param getter the getter
     *
     * \return \c this
     */
    template<typename P, typename... C>
    FieldsMapper<T>& registerField(const std::string& name, std::function<P(const T&)> getter, std::function<void(T&, const P&)>, C&&...);
    // Fields registration -
    // Methods -

  };

  /*!
   * \brief Fields of an object, bound to the parameters of a query with the same names ( \ref fieldsOf )
   *
   * \tparam T the described type
   *
   * \since 0.3
   */
  template<anch::Described T>
  class FieldsOf {

    // Attributes +
  private:
    /*! The object */
    const T& _object;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref FieldsOf constructor
     *
     * \param object the object (it has to live while the fields are bound)
     */
    explicit FieldsOf(const T& object) noexcept;
    // Constructors -

    // Methods +
  public:
    /*!
     * Object getter
     *
     * \return the object
     */
    const T& object() const noexcept;
    // Methods -

  };

  /*!
   * Fields of an object as query parameters: each parameter of the query is bound to the field of the same name
   * ( \ref FieldsMapper ), the other fields are ignored ; a parameter which is not a field is not bound (error before
   * execution):
   * \snippet{trimleft} anch-sql/examples/connection.cpp fields
   *
   * \tparam T the described type
   *
   * \param object the object (it has to live while the fields are bound)
   *
   * \return the fields
   *
   * \since 0.3
   */
  template<anch::Described T>
  anch::sql::FieldsOf<T> fieldsOf(const T& object) noexcept;

}

namespace anch::sql {

  // Fields mapper +
  template<anch::Described T>
  FieldsMapper<T>::FieldsMapper(): _fields() {
    anch::describe<T>(*this);
  }

  template<anch::Described T>
  const FieldsMapper<T>&
  FieldsMapper<T>::instance() {
    static const FieldsMapper<T> mapper;
    return mapper;
  }

  template<anch::Described T>
  const typename FieldsMapper<T>::Field*
  FieldsMapper<T>::find(std::string_view name) const noexcept {
    for(const Field& field : _fields) {
      if(field.name == name) {
	return &field;
      }
    }
    return nullptr;
  }

  template<anch::Described T>
  const std::vector<typename FieldsMapper<T>::Field>&
  FieldsMapper<T>::fields() const noexcept {
    return _fields;
  }

  template<anch::Described T>
  template<typename P, typename... C>
  FieldsMapper<T>&
  FieldsMapper<T>::registerField(const std::string& name, P T::* member, C&&...) {
    static_assert(anch::sql::Bindable<std::remove_cvref_t<P>>, "SQL parameters are integers, floating point numbers, strings, dates,"
		  " std::optional and lists of them: this field of the description can not be bound");
    _fields.push_back(Field{name, [member](const T& object) { return anch::sql::toBound(object.*member); }});
    return *this;
  }

  template<anch::Described T>
  template<typename P, typename... C>
  FieldsMapper<T>&
  FieldsMapper<T>::registerField(const std::string& name, std::function<P(const T&)> getter, C&&...) {
    static_assert(anch::sql::Bindable<std::remove_cvref_t<P>>, "SQL parameters are integers, floating point numbers, strings, dates,"
		  " std::optional and lists of them: this field of the description can not be bound");
    _fields.push_back(Field{name, [getter](const T& object) { return anch::sql::toBound(getter(object)); }});
    return *this;
  }

  template<anch::Described T>
  template<typename P, typename... C>
  FieldsMapper<T>&
  FieldsMapper<T>::registerField(const std::string&, std::function<void(T&, const P&)>, C&&...) {
    return *this;
  }

  template<anch::Described T>
  template<typename P, typename... C>
  FieldsMapper<T>&
  FieldsMapper<T>::registerField(const std::string& name, std::function<P(const T&)> getter, std::function<void(T&, const P&)>, C&&...) {
    return registerField(name, getter);
  }
  // Fields mapper -

  // Fields of an object +
  template<anch::Described T>
  FieldsOf<T>::FieldsOf(const T& object) noexcept: _object(object) {
    // Nothing to do
  }

  template<anch::Described T>
  const T&
  FieldsOf<T>::object() const noexcept {
    return _object;
  }

  template<anch::Described T>
  anch::sql::FieldsOf<T>
  fieldsOf(const T& object) noexcept {
    return anch::sql::FieldsOf<T>(object);
  }
  // Fields of an object -

}
