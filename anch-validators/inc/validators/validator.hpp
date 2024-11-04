/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2023 Vincent Lachenal

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

#include <regex>
#include <optional>


namespace anch::val {

  /*!
   * Validator configuration
   *
   * \param RT range type (for min and max check)
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename RT>
  struct ValidatorConfig {

    /*! Mandatory flag (default to \c false ) */
    bool mandatory = false;

    /*! Optional min value */
    std::optional<RT> min;

    /*! Optional max value */
    std::optional<RT> max;

    /*! Optional regexp */
    std::optional<std::regex> regexp;

    /*!
     * Check if current validator has any constraint
     *
     * \return \c true when any condtraint has been set, \c false otherwise
     */
    bool hasConstraint() noexcept;

  };

  /*!
   * \brief Type/structure validator
   *
   * Validate value (min, max, mandatory, empty, regexp)
   *
   * \tparam T type to validate
   * \tparam RT min/max validator range (default to \c T )
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  template<typename T, typename RT = T>
  class Validator {

  private:
    /*! Validator configuration */
    ValidatorConfig<RT> _config;

  public:
    /*!
     * \c Validator default constructor
     */
    Validator() = delete;

    /*!
     * \c Validator constructor
     *
     * \param config the validator configuration
     */
    Validator(const ValidatorConfig<RT>& config);

  public:
    /*!
     * \c Validator destructor
     */
    virtual ~Validator();

  public:
    /*!
     * Validate value
     *
     * \param value the value to check
     *
     * \return \c true if valid, \c false otherwise
     */
    bool validate(const T& value);

    /*!
     * Check mandatory
     *
     * \return \c true if not mandatory or if present, \c false otherwise
     */
    bool checkMandatory(const T& value);

    /*!
     * Check min value
     *
     * \return \c true if value superior to min, \c false otherwise
     */
    bool checkMin(const T& value);

    /*!
     * Check max value
     *
     * \return \c true if value inferior to max, \c false otherwise
     */
    bool checkMax(const T& value);

    /*!
     * Check string regexp
     *
     * \return \c true if match or not set, \c false otherwise
     */
    bool checkPattern(const T& value);

  private:

    /*!
     * Check mandatory
     *
     * \return \c true if not mandatory or if present, \c false otherwise
     */
    bool checkMandatoryImpl(const T& value);

    /*!
     * Check min value
     *
     * \return \c true if value superior to min, \c false otherwise
     */
    bool checkMinImpl(const T& value);

    /*!
     * Check max value
     *
     * \return \c true if value inferior to max, \c false otherwise
     */
    bool checkMaxImpl(const T& value);

    /*!
     * Check string regexp
     *
     * \return \c true if match or not set, \c false otherwise
     */
    bool checkPatternImpl(const T& value);

  };

}  // anch::val

#include "validators/impl/validators.hpp"
