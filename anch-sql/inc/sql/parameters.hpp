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
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "sql/types/date.hpp"
#include "sql/types/time.hpp"
#include "sql/types/timestamp.hpp"

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
   * Convert SQL date to parameter value ( \c YYYY-MM-DD )
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(const anch::sql::Date& value);

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
   * Convert SQL timestamp to parameter value ( \c YYYY-MM-DD HH:MM:SS.mmm )
   *
   * \param value the value
   *
   * \return the parameter value
   *
   * \since 0.3
   */
  ANCH_SQL_EXPORT anch::sql::ParameterValue toParameter(const anch::sql::Timestamp& value);

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
   * Value which can be bound to a prepared statement parameter ( \c anch::sql::toParameter )
   *
   * \tparam T the value type
   *
   * \since 0.3
   */
  template<typename T>
  concept Bindable = requires(const T& value) {
    { anch::sql::toParameter(value) } -> std::same_as<anch::sql::ParameterValue>;
  };

  /*!
   * \brief Named parameter value ( \ref Parameters item)
   *
   * \since 0.3
   */
  struct ANCH_SQL_EXPORT Parameter {
    /*! Parameter name (without \c ':' ) */
    std::string name;

    /*! Parameter value */
    anch::sql::ParameterValue value;

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
    void put(std::string_view name, anch::sql::ParameterValue value);
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
  Parameter::Parameter(std::string parameterName, const T& parameterValue):
    name(std::move(parameterName)), value(anch::sql::toParameter(parameterValue)) {
    // Nothing to do
  }

  template<anch::sql::Bindable T>
  Parameters&
  Parameters::add(std::string_view name, const T& value) {
    put(name, anch::sql::toParameter(value));
    return *this;
  }

}
