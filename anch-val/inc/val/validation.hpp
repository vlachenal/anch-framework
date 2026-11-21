/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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
#include <vector>

#include "mapping.hpp"

#include "val/constraints.hpp"

namespace anch::val {

  /*!
   * \brief Mapper which validates the fields of a described type ( \c anch::Mapping )
   *
   * It satisfies the \c anch::ObjectMapper concept: the description registers its fields and their constraints.
   * Fields with a value (member pointer or getter) are validated: their value constraints and their nested values
   * (described objects, \c std::optional and pointers values, collections items, maps values). Fields registered with
   * a setter only are not checked, and \ref anch::val::required is not checked (an object always has its fields).
   *
   * \tparam T the described type
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  template<typename T>
  class ValidationMapper {

    // Attributes +
  private:
    /*! Fields validators */
    std::vector<std::function<void(const T&, anch::val::ValidationContext&)>> _validators;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ValidationMapper constructor: fields are registered by \c anch::Mapping<T>
     */
    ValidationMapper();
    // Constructors -

    // Methods +
  public:
    /*!
     * Get the mapper of \c T (built on first use)
     *
     * \return the mapper
     */
    static const ValidationMapper<T>& instance();

    /*!
     * Validate object fields
     *
     * \param value the object
     * \param context the validation context
     */
    void validate(const T& value, anch::val::ValidationContext& context) const;

    /*!
     * Register field with its member
     *
     * \param name the field name
     * \param member the member
     * \param constraints the constraints
     *
     * \return \c this
     */
    template<typename P, typename... C>
    ValidationMapper<T>& registerField(const std::string& name, P T::* member, C&&... constraints);

    /*!
     * Register field with its getter
     *
     * \param name the field name
     * \param getter the getter
     * \param constraints the constraints
     *
     * \return \c this
     */
    template<typename P, typename... C>
    ValidationMapper<T>& registerField(const std::string& name, std::function<P(const T&)> getter, C&&... constraints);

    /*!
     * Register field with its setter: it is not validated (no value)
     *
     * \param name the field name
     * \param setter the setter
     * \param constraints the constraints
     *
     * \return \c this
     */
    template<typename P, typename... C>
    ValidationMapper<T>& registerField(const std::string& name, std::function<void(T&, const P&)> setter, C&&... constraints);

    /*!
     * Register field with its getter and its setter: the getter value is validated
     *
     * \param name the field name
     * \param getter the getter
     * \param setter the setter
     * \param constraints the constraints
     *
     * \return \c this
     */
    template<typename P, typename... C>
    ValidationMapper<T>& registerField(const std::string& name, std::function<P(const T&)> getter,
				       std::function<void(T&, const P&)> setter, C&&... constraints);

  private:
    /*!
     * Add field validator when there is something to check
     *
     * \param name the field name
     * \param get the value getter
     * \param constraints the constraints
     */
    template<typename G, typename... C>
    void add(const std::string& name, G get, C&&... constraints);
    // Methods -

  };

  /*!
   * Validate value without serializing it: constraints of the fields of described objects ( \c anch::Mapping ) and
   * of their nested values. Every violation is returned with its path (JSON Pointer), in fields registration order.
   * \snippet{trimleft} anch-val/examples/val.cpp validate
   *
   * \tparam T the value type (described object, collection, map, \c std::optional , ...)
   *
   * \param value the value
   *
   * \return the violations, empty when value is valid
   *
   * \since 0.3
   */
  template<typename T>
  std::vector<anch::val::Violation> validate(const T& value);

  /*!
   * Check if value is valid ( \ref anch::val::validate returns no violation)
   *
   * \tparam T the value type
   *
   * \param value the value
   *
   * \return \c true when value is valid
   *
   * \since 0.3
   */
  template<typename T>
  bool isValid(const T& value);

}

#include "val/impl/validation.hpp"
