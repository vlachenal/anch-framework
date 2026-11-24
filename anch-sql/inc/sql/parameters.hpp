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

#include "anch-sql-export.hpp"

#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "date/date.hpp"
#include "date/dateTime.hpp"
#include "sql/types/time.hpp"

namespace anch::sql {

  /*!
   * Parameter value bound to a prepared statement: values are sent as text, \c std::nullopt is SQL \c NULL
   *
   * \since 0.3
   */
  using ParameterValue = std::optional<std::string>;

  // Values conversion +
  /*!
   * Convert 16 bits signed integer to parameter value
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(int16_t value);

  /*!
   * Convert 16 bits unsigned integer to parameter value
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(uint16_t value);

  /*!
   * Convert 32 bits signed integer to parameter value
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(int32_t value);

  /*!
   * Convert 32 bits unsigned integer to parameter value
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(uint32_t value);

  /*!
   * Convert 64 bits signed integer to parameter value
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(int64_t value);

  /*!
   * Convert 64 bits unsigned integer to parameter value
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(uint64_t value);

  /*!
   * Convert floating point number to parameter value (shortest representation which reads back the same value ;
   * \c float values are converted as \c double )
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(double value);

  /*!
   * Convert string to parameter value
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(const std::string& value);

  /*!
   * Convert character array to parameter value
   *
   * \param value the value ( \c NULL pointer is SQL \c NULL )
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(const char* value);

  /*!
   * Convert date to parameter value ( \c YYYY-MM-DD )
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(const anch::date::Date& value);

  /*!
   * Convert SQL time to parameter value ( \c HH:MM:SS.mmm )
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(const anch::sql::Time& value);

  /*!
   * Convert point in time to parameter value ( \c YYYY-MM-DD HH:MM:SS.mmm , local time)
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(const anch::date::DateTime& value);

  /*!
   * SQL \c NULL parameter value
   *
   * \return \c std::nullopt
   *
   * \since 0.3
   */
  inline anch::sql::ParameterValue toParameter(std::nullopt_t) {
    return std::nullopt;
  }

  /*!
   * Convert optional value to parameter value ( \c std::nullopt is SQL \c NULL )
   *
   * \tparam T the value type
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  template<typename T>
  anch::sql::ParameterValue toParameter(const std::optional<T>& value);
  // Values conversion -

  /*!
   * Value which can be bound to a prepared statement parameter ( \c anch::sql::toParameter : integers, floating point
   * numbers, strings, dates (\c anch::date::Date , \c anch::date::DateTime , \c anch::sql::Time ), \c std::optional of them)
   *
   * \tparam T the value type
   *
   * \since 0.3
   */
  template<typename T>
  concept BindableScalar = requires(const T& value) {
    { anch::sql::toParameter(value) } -> std::same_as<anch::sql::ParameterValue>;
  };

  /*!
   * List of values ( \c std::vector , \c std::list , \c std::set , ... of \c BindableScalar values ; strings are
   * not lists) bound to a single parameter: a PostgreSQL array, an expanded list for other databases
   * ( \c IN (:ids) , see \c anch::sql::NamedQuery )
   *
   * \tparam T the list type
   *
   * \since 0.3
   */
  template<typename T>
  concept BindableList = std::ranges::input_range<const T> && !std::convertible_to<const T&, std::string_view>
    && anch::sql::BindableScalar<std::ranges::range_value_t<const T>>;

  /*!
   * Value or list of values which can be bound to a prepared statement parameter
   *
   * \tparam T the value type
   *
   * \since 0.3
   */
  template<typename T>
  concept Bindable = anch::sql::BindableScalar<T> || anch::sql::BindableList<T>;

  /*!
   * Values of a list parameter
   *
   * \since 0.3
   */
  using ParameterList = std::vector<anch::sql::ParameterValue>;

  /*!
   * PostgreSQL array literal of a list ( \c {"1","2",NULL} : elements are quoted, \c '\\' and \c '"' are escaped)
   *
   * \param elements the list values
   *
   * \return the array literal
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT std::string arrayLiteral(const anch::sql::ParameterList& elements);

  /*!
   * \brief Value bound to a parameter: value, or list of values
   *
   * \since 0.3
   */
  struct ANCH_SQL_EXPORT BoundValue {
    /*! The value ( \c std::nullopt for \c NULL ; PostgreSQL array literal of a list) */
    anch::sql::ParameterValue value;

    /*! The values of a list, \c std::nullopt for a value which is not a list */
    std::optional<anch::sql::ParameterList> list;
  };

  /*!
   * Convert value or list of values to bound value
   *
   * \tparam T the value type
   *
   * \param value the value
   *
   * \return the bound value
   *
   * \since 0.3
   */
  template<anch::sql::Bindable T>
  anch::sql::BoundValue toBound(const T& value);

  /*!
   * \brief Named parameter value ( \ref Parameters item)
   *
   * \since 0.3
   */
  struct ANCH_SQL_EXPORT Parameter {
    /*! Parameter name (without \c ':' ) */
    std::string name;

    /*! Parameter value ( \c std::nullopt for \c NULL ; PostgreSQL array literal of a list) */
    anch::sql::ParameterValue value;

    /*! The values of a list, \c std::nullopt for a value which is not a list */
    std::optional<anch::sql::ParameterList> list;

    /*!
     * \ref Parameter constructor
     *
     * \tparam T the value type
     *
     * \param parameterName the parameter name
     * \param parameterValue the value
     */
    template<anch::sql::Bindable T>
    Parameter(std::string parameterName, const T& parameterValue);
  };

  /*!
   * \brief Named parameters of a query ( \c :name ), bound by \ref Connection
   *
   * Values are converted when they are added. A parameter which is added twice keeps its last value:
   * \snippet{trimleft} anch-sql/examples/connection.cpp parameters
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  class ANCH_SQL_EXPORT Parameters {

    // Attributes +
  private:
    /*! Parameters, in order of addition */
    std::vector<anch::sql::Parameter> _parameters;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Parameters constructor
     *
     * \param parameters the parameters
     */
    Parameters(std::initializer_list<anch::sql::Parameter> parameters = {});
    // Constructors -

    // Methods +
  public:
    /*!
     * Add parameter
     *
     * \tparam T the value type
     *
     * \param name the parameter name
     * \param value the value
     *
     * \return \c this
     */
    template<anch::sql::Bindable T>
    Parameters& add(std::string_view name, const T& value);

    /*!
     * Add SQL \c NULL parameter
     *
     * \param name the parameter name
     *
     * \return \c this
     */
    Parameters& addNull(std::string_view name);

    /*!
     * Add parameter (or replace parameter of the same name)
     *
     * \param parameter the parameter
     *
     * \return \c this
     */
    Parameters& add(anch::sql::Parameter parameter);

    /*!
     * Parameters getter
     *
     * \return the parameters, in order of addition
     */
    const std::vector<anch::sql::Parameter>& values() const noexcept;

  private:
    /*!
     * Add or replace parameter value
     *
     * \param name the parameter name
     * \param value the value
     */
    void put(std::string_view name, anch::sql::BoundValue value);
    // Methods -

  };

}

namespace anch::sql {

  template<typename T>
  anch::sql::ParameterValue
  toParameter(const std::optional<T>& value) {
    if(value.has_value()) {
      return anch::sql::toParameter(*value);
    }
    return std::nullopt;
  }

  template<anch::sql::Bindable T>
  anch::sql::BoundValue
  toBound(const T& value) {
    if constexpr (anch::sql::BindableScalar<T>) {
      return anch::sql::BoundValue{anch::sql::toParameter(value), std::nullopt};
    } else {
      anch::sql::ParameterList elements;
      for(const auto& element : value) {
	elements.push_back(anch::sql::toParameter(element));
      }
      std::string literal = anch::sql::arrayLiteral(elements);
      return anch::sql::BoundValue{std::move(literal), std::move(elements)};
    }
  }

  template<anch::sql::Bindable T>
  Parameter::Parameter(std::string parameterName, const T& parameterValue): name(std::move(parameterName)), value(), list() {
    anch::sql::BoundValue bound = anch::sql::toBound(parameterValue);
    value = std::move(bound.value);
    list = std::move(bound.list);
  }

  template<anch::sql::Bindable T>
  Parameters&
  Parameters::add(std::string_view name, const T& value) {
    put(name, anch::sql::toBound(value));
    return *this;
  }

}
