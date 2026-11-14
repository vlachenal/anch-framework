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

#include <concepts>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>

#include "uuid.hpp"

#include "rest/form.hpp"
#include "rest/request.hpp"

namespace anch::rest {

  /*!
   * Path parameter converter: customization point to bind path parameters to endpoint arguments.\n
   * A specialization provides <tt>static std::optional<T> convert(std::string_view value)</tt> ,
   * which returns an empty value when the parameter is invalid (the response is \c 400 ).\n
   * Integral and floating point types, \c bool , \c std::string and \c anch::UUID are supported.
   *
   * \tparam T the parameter type
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<typename T>
  struct ParamConverter;

  /*!
   * Integral path parameter (\c std::from_chars , the whole value has to be a number)
   *
   * \tparam T the integral type
   */
  template<typename T>
  requires (std::integral<T> && !std::same_as<T, bool>)
  struct ParamConverter<T> {
    /*!
     * Convert parameter
     *
     * \param value the parameter value
     *
     * \return the number, empty when invalid or out of range
     */
    static std::optional<T> convert(std::string_view value) noexcept;
  };

  /*!
   * Floating point path parameter (\c std::from_chars )
   *
   * \tparam T the floating point type
   */
  template<std::floating_point T>
  struct ParamConverter<T> {
    /*!
     * Convert parameter
     *
     * \param value the parameter value
     *
     * \return the number, empty when invalid or out of range
     */
    static std::optional<T> convert(std::string_view value) noexcept;
  };

  /*!
   * Boolean path parameter: \c true or \c false
   */
  template<>
  struct ParamConverter<bool> {
    /*!
     * Convert parameter
     *
     * \param value the parameter value
     *
     * \return the boolean, empty when invalid
     */
    static std::optional<bool> convert(std::string_view value) noexcept;
  };

  /*!
   * String path parameter
   */
  template<>
  struct ParamConverter<std::string> {
    /*!
     * Convert parameter
     *
     * \param value the parameter value
     *
     * \return the string
     */
    static std::optional<std::string> convert(std::string_view value);
  };

  /*!
   * UUID path parameter
   */
  template<>
  struct ParamConverter<anch::UUID> {
    /*!
     * Convert parameter
     *
     * \param value the parameter value
     *
     * \return the UUID, empty when invalid
     */
    static std::optional<anch::UUID> convert(std::string_view value);
  };

  /*!
   * Type which can be bound to a path parameter (see \ref anch::rest::ParamConverter "ParamConverter")
   *
   * \tparam T the type
   */
  template<typename T>
  concept PathParameter = requires(std::string_view value) {
    { anch::rest::ParamConverter<T>::convert(value) } -> std::same_as<std::optional<T>>;
  };

  /*!
   * Optional query parameter converted with \ref anch::rest::ParamConverter "ParamConverter"
   *
   * \tparam T the parameter type
   *
   * \param request the request
   * \param name the parameter name
   *
   * \return the parameter value, empty when the parameter is absent
   *
   * \throw anch::rest::HttpException \c 400 when value is invalid
   *
   * \since 0.2
   */
  template<anch::rest::PathParameter T>
  std::optional<T> queryParameter(const anch::rest::Request& request, std::string_view name);

  /*!
   * Required query parameter converted with \ref anch::rest::ParamConverter "ParamConverter"
   *
   * \tparam T the parameter type
   *
   * \param request the request
   * \param name the parameter name
   *
   * \return the parameter value
   *
   * \throw anch::rest::HttpException \c 400 when parameter is absent or its value is invalid
   *
   * \since 0.2
   */
  template<anch::rest::PathParameter T>
  T requiredQueryParameter(const anch::rest::Request& request, std::string_view name);

  /*!
   * Optional form field converted with \ref anch::rest::ParamConverter "ParamConverter" (first part
   * with this name)
   *
   * \tparam T the field type
   *
   * \param form the form
   * \param name the field name
   *
   * \return the field value, empty when the field is absent
   *
   * \throw anch::rest::HttpException \c 400 when value is invalid
   *
   * \since 0.2
   */
  template<anch::rest::PathParameter T>
  std::optional<T> formField(const anch::rest::Form& form, std::string_view name);

  /*!
   * Required form field converted with \ref anch::rest::ParamConverter "ParamConverter" (first part
   * with this name)
   *
   * \tparam T the field type
   *
   * \param form the form
   * \param name the field name
   *
   * \return the field value
   *
   * \throw anch::rest::HttpException \c 400 when field is absent or its value is invalid
   *
   * \since 0.2
   */
  template<anch::rest::PathParameter T>
  T requiredFormField(const anch::rest::Form& form, std::string_view name);


  /*!
   * Value which can be used as path or query parameter: strings, numbers, \c bool and types with
   * stream injection operator
   *
   * \tparam T the value type
   *
   * \since 0.2
   */
  template<typename T>
  concept ParameterValue = std::convertible_to<const T&, std::string_view>
    || std::is_arithmetic_v<T>
    || requires(std::ostream& out, const T& value) { out << value; };

  /*!
   * Format parameter value
   *
   * \param value the value
   *
   * \return the formatted value (not encoded)
   *
   * \since 0.2
   */
  template<anch::rest::ParameterValue T>
  std::string toParameterString(const T& value);

}

#include "rest/impl/paramConverter.hpp"
