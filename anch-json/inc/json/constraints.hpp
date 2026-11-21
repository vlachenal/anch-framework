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

#include "anch-json-export.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "val/constraints.hpp"

namespace anch::json {

  // Reader context early declaration
  class ReaderContext;

  // Writer context early declaration
  class WriterContext;

}  // anch::json

namespace anch::json::detail {

  /*!
   * Check required fields have been found. It is not inlined to keep mappers code small.
   *
   * \param required the required fields
   * \param found the found required fields: one bit per required field (\c 64 fields per word)
   * \param context the reader context
   *
   * \throw anch::json::MappingError \c MISSING_FIELD when at least one required field has not been found
   */
  ANCH_JSON_EXPORT void checkRequired(const std::vector<std::string>& required, const uint64_t* found, anch::json::ReaderContext& context);

  /*!
   * Raise \c null value in required field error. It is not inlined to keep mappers code small.
   *
   * \param context the reader context
   *
   * \throw anch::json::MappingError \c CONSTRAINT_VIOLATION
   */
  [[noreturn]] ANCH_JSON_EXPORT void rejectNull(anch::json::ReaderContext& context);

  /*!
   * Raise constraint violation error. It is not inlined to keep mappers code small.
   *
   * \param context the reader context
   * \param message the error message
   *
   * \throw anch::json::MappingError \c CONSTRAINT_VIOLATION
   */
  [[noreturn]] ANCH_JSON_EXPORT void violation(anch::json::ReaderContext& context, const std::string& message);

  /*!
   * Raise constraint violation error on serialization. It is not inlined to keep mappers code small.
   *
   * \param context the writer context
   * \param message the error message
   *
   * \throw anch::json::MappingError \c CONSTRAINT_VIOLATION
   */
  [[noreturn]] ANCH_JSON_EXPORT void violation(anch::json::WriterContext& context, const std::string& message);

  /*!
   * Validate nested values: objects (fields constraints), \c std::optional and pointers values, collections items
   * and maps values. Primitive values have no nested values.
   *
   * \tparam P the value type
   *
   * \param value the value
   * \param context the validation context
   */
  template<typename P>
  [[gnu::cold]] void validateNested(const P& value, anch::val::ValidationContext& context);

  /*!
   * Check if values of type \c P can contain objects (directly or in \c std::optional , pointers, collections or
   * maps): these values have to be validated
   *
   * \tparam P the value type
   *
   * \return \c true when values can contain objects
   */
  template<typename P>
  constexpr bool hasNestedObjects();

  /*!
   * Validate field: value constraints and nested values. It is cold code (not used by serialization and
   * deserialization): it does not use the inlining budget of mappers.
   *
   * \tparam P the value type
   * \tparam C the constraints types
   *
   * \param key the field name
   * \param value the field value
   * \param context the validation context
   * \param constraints the field constraints
   */
  template<typename P, typename... C>
  [[gnu::cold]] void validateField(const std::string& key, const P& value, anch::val::ValidationContext& context, const C&... constraints);

  /*!
   * Write empty required field: \c null for nullable values ( \c std::optional , smart pointers and pointers),
   * \c [] or \c {} for empty containers, whatever \c serialize_null and \c serialize_empty_col options are
   *
   * \tparam P the value type
   *
   * \param key the field name
   * \param value the value
   * \param context the writer context
   *
   * \return \c true when an empty value has been written, \c false when value is not empty (it has to be written)
   */
  template<typename P>
  bool writeEmptyRequired(const std::string& key, const P& value, anch::json::WriterContext& context);

}  // anch::json::detail

namespace anch::val {

  /*!
   * \brief anch-val constraints checked on deserialization: \c deserialize_check_constraints option, violations
   * raise \c anch::json::MappingError ( \c CONSTRAINT_VIOLATION , with the path of the value)
   */
  template<>
  struct CheckContext<anch::json::ReaderContext> {

    /*!
     * Check if constraints have to be checked
     *
     * \param context the reader context
     *
     * \return \c deserialize_check_constraints option
     */
    static bool enabled(const anch::json::ReaderContext& context);

    /*!
     * Raise violation
     *
     * \param context the reader context
     * \param message the message
     */
    [[noreturn]] static void violation(anch::json::ReaderContext& context, const std::string& message);

    /*!
     * Check map value: the key is added to the path of the violation
     *
     * \param context the reader context
     * \param key the map key
     * \param check the check function
     */
    template<typename F>
    static void item(anch::json::ReaderContext& context, std::string_view key, F&& check);

    /*!
     * Check collection item: the index is added to the path of the violation
     *
     * \param context the reader context
     * \param index the item index
     * \param check the check function
     */
    template<typename F>
    static void item(anch::json::ReaderContext& context, std::size_t index, F&& check);

  };

  /*!
   * \brief anch-val constraints checked on serialization: \c serialize_check_constraints option, violations raise
   * \c anch::json::MappingError ( \c CONSTRAINT_VIOLATION )
   */
  template<>
  struct CheckContext<anch::json::WriterContext> {

    /*!
     * Check if constraints have to be checked
     *
     * \param context the writer context
     *
     * \return \c serialize_check_constraints option
     */
    static bool enabled(const anch::json::WriterContext& context);

    /*!
     * Raise violation
     *
     * \param context the writer context
     * \param message the message
     */
    [[noreturn]] static void violation(anch::json::WriterContext& context, const std::string& message);

    /*!
     * Check map value: the key is added to the path of the violation
     *
     * \param context the writer context
     * \param key the map key
     * \param check the check function
     */
    template<typename F>
    static void item(anch::json::WriterContext& context, std::string_view key, F&& check);

    /*!
     * Check collection item: the index is added to the path of the violation
     *
     * \param context the writer context
     * \param index the item index
     * \param check the check function
     */
    template<typename F>
    static void item(anch::json::WriterContext& context, std::size_t index, F&& check);

  };

}  // anch::val

#include "json/impl/constraints.hpp"
