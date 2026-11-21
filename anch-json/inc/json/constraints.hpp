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

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>


namespace anch::json {

  // Reader context early declaration
  class ReaderContext;

  // Writer context early declaration
  class WriterContext;

  /*!
   * \brief Constraint violation found by \ref anch::json::validate
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT Violation {
    /*! Path of the value (JSON Pointer, empty for root value) */
    std::string path;

    /*! Violation message */
    std::string message;
  };

  /*!
   * \brief Validation context
   *
   * Context used by \ref anch::json::validate : it keeps the path of the checked value and collects violations.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_JSON_EXPORT ValidationContext {

    // Attributes +
  private:
    /*! Current path segments (escaped) */
    std::vector<std::string> _path;

    /*! Violations */
    std::vector<anch::json::Violation> _violations;
    // Attributes -

    // Methods +
  public:
    /*!
     * Enter object field or map entry
     *
     * \param field the field name (escaped in path)
     */
    void enter(std::string_view field);

    /*!
     * Enter array item
     *
     * \param index the item index
     */
    void enter(std::size_t index);

    /*!
     * Leave the current field or item
     */
    void leave();

    /*!
     * Add violation on the current value
     *
     * \param message the violation message
     */
    void add(const std::string& message);

    /*!
     * Collected violations getter
     *
     * \return the violations
     */
    std::vector<anch::json::Violation>& violations();
    // Methods -

  };

  // Items constraint early declaration
  template<typename... C>
  struct Items;

  /*!
   * Number type which can be used in numeric constraints: integers (without \c bool and characters types) and
   * floating point numbers
   *
   * \tparam N the type
   *
   * \since 0.2
   */
  template<typename N>
  concept Number = std::floating_point<N>
    || (std::integral<N> && !std::same_as<N, bool> && !std::same_as<N, char> && !std::same_as<N, wchar_t>
	&& !std::same_as<N, char8_t> && !std::same_as<N, char16_t> && !std::same_as<N, char32_t>);

  /*!
   * Character string type ( \c std::string , \c std::string_view , ...)
   *
   * \tparam V the type
   *
   * \since 0.2
   */
  template<typename V>
  concept StringLike = std::convertible_to<const V&, std::string_view>;

  /*!
   * Map type (with \c key_type and \c mapped_type )
   *
   * \tparam V the type
   *
   * \since 0.2
   */
  template<typename V>
  concept MapLike = std::ranges::sized_range<const V> && requires {
    typename V::key_type;
    typename V::mapped_type;
  };

  /*!
   * Collection type which is neither a string nor a map ( \c std::vector , \c std::list , \c std::set , ...)
   *
   * \tparam V the type
   *
   * \since 0.2
   */
  template<typename V>
  concept Collection = std::ranges::sized_range<const V> && !StringLike<V> && !MapLike<V>;

}  // anch::json

namespace anch::json::detail {

  /*! Type can hold \c null value (default: \c false ) */
  template<typename P>
  struct IsNullable: std::is_pointer<P> {};

  /*! \c std::optional can hold \c null value */
  template<typename P>
  struct IsNullable<std::optional<P>>: std::true_type {};

  /*! \c std::shared_ptr can hold \c null value */
  template<typename P>
  struct IsNullable<std::shared_ptr<P>>: std::true_type {};

  /*! \c std::unique_ptr can hold \c null value */
  template<typename P, typename D>
  struct IsNullable<std::unique_ptr<P,D>>: std::true_type {};

  /*! Value type (default: the type itself) */
  template<typename P>
  struct UnwrapType {
    /*! Value type */
    using type = P;
  };

  /*! \c std::optional value type */
  template<typename P>
  struct UnwrapType<std::optional<P>> {
    /*! Value type */
    using type = P;
  };

  /*! \c std::shared_ptr value type */
  template<typename P>
  struct UnwrapType<std::shared_ptr<P>> {
    /*! Value type */
    using type = P;
  };

  /*! \c std::unique_ptr value type */
  template<typename P, typename D>
  struct UnwrapType<std::unique_ptr<P,D>> {
    /*! Value type */
    using type = P;
  };

  /*! Pointer value type */
  template<typename P>
  struct UnwrapType<P*> {
    /*! Value type */
    using type = P;
  };

  /*! Value type of nullable type ( \c std::optional , smart pointer or pointer), the type itself otherwise */
  template<typename P>
  using Unwrap = typename UnwrapType<std::remove_cvref_t<P>>::type;

  /*! Constraint \c C can check value of type \c V (constraint has a <tt>bool check(const V&, std::string&) const</tt> method) */
  template<typename C, typename V>
  struct IsValueConstraint: std::bool_constant<requires(const C& constraint, const V& value, std::string& message) {
      { constraint.check(value, message) } -> std::same_as<bool>;
    }> {};

  /*!
   * Check if items constraints can be applied to container items
   *
   * \tparam V the container type
   * \tparam C the items constraints types
   *
   * \return \c true when \c V is a container and every constraint can check its items
   */
  template<typename V, typename... C>
  constexpr bool itemsApplicable() {
    if constexpr (anch::json::MapLike<V>) {
      return (IsValueConstraint<C, Unwrap<typename V::mapped_type>>::value && ...);
    } else if constexpr (anch::json::Collection<V>) {
      return (IsValueConstraint<C, Unwrap<std::ranges::range_value_t<V>>>::value && ...);
    } else {
      return false;
    }
  }

  /*! \ref anch::json::Items constraint can check container items */
  template<typename... C, typename V>
  struct IsValueConstraint<anch::json::Items<C...>, V>: std::bool_constant<itemsApplicable<V, C...>()> {};

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
   * Check value format with the registered validator
   *
   * \param name the format name
   * \param value the value
   *
   * \return \c false when the format is known and the value is invalid, \c true otherwise
   */
  ANCH_JSON_EXPORT bool matchesFormat(const std::string& name, std::string_view value);

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
   * Add constraint violation to validation context (validation goes on)
   *
   * \param context the validation context
   * \param message the violation message
   */
  ANCH_JSON_EXPORT void violation(anch::json::ValidationContext& context, const std::string& message);

  /*!
   * Constraints are always checked by \ref anch::json::validate
   *
   * \param context the validation context
   *
   * \return \c true
   */
  bool checkEnabled(const anch::json::ValidationContext& context);

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
  [[gnu::cold]] void validateNested(const P& value, anch::json::ValidationContext& context);

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
  [[gnu::cold]] void validateField(const std::string& key, const P& value, anch::json::ValidationContext& context, const C&... constraints);

  /*!
   * Check if constraints have to be checked on deserialization ( \c deserialize_check_constraints option)
   *
   * \param context the reader context
   *
   * \return \c true when constraints have to be checked
   */
  bool checkEnabled(const anch::json::ReaderContext& context);

  /*!
   * Check if constraints have to be checked on serialization ( \c serialize_check_constraints option)
   *
   * \param context the writer context
   *
   * \return \c true when constraints have to be checked
   */
  bool checkEnabled(const anch::json::WriterContext& context);

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

  /*!
   * Number of Unicode code points in UTF-8 string (JSON Schema string length)
   *
   * \param str the string
   *
   * \return the number of code points
   */
  ANCH_JSON_EXPORT std::size_t codePoints(std::string_view str);

  /*!
   * Format value for error message: quoted string, number, or empty string when value can not be formatted
   *
   * \tparam V the value type
   *
   * \param value the value
   *
   * \return the formatted value
   */
  template<typename V>
  std::string formatValue(const V& value);

  /*!
   * Check one constraint on value. \c null values ( \c std::optional , smart pointers and pointers) are not checked.
   *
   * \tparam C the constraint type
   * \tparam P the value type
   * \tparam X the context type ( \c anch::json::ReaderContext or \c anch::json::WriterContext )
   *
   * \param constraint the constraint
   * \param value the value
   * \param context the reader or writer context
   *
   * \throw anch::json::MappingError \c CONSTRAINT_VIOLATION
   */
  template<typename C, typename P, typename X>
  void applyConstraint(const C& constraint, const P& value, X& context);

  /*!
   * Check field value constraints ( \ref anch::json::Required is ignored). Constraints are not checked when
   * \c deserialize_check_constraints (reader context) or \c serialize_check_constraints (writer context) option is
   * \c false .
   *
   * \tparam P the value type
   * \tparam X the context type ( \c anch::json::ReaderContext or \c anch::json::WriterContext )
   * \tparam C the constraints types
   *
   * \param value the value
   * \param context the reader or writer context
   * \param constraints the constraints
   *
   * \throw anch::json::MappingError \c CONSTRAINT_VIOLATION
   */
  template<typename P, typename X, typename... C>
  void checkConstraints(const P& value, X& context, const C&... constraints);

}  // anch::json::detail

namespace anch::json {

  /*!
   * \brief Required field constraint
   *
   * The field has to be present in JSON object, otherwise \ref anch::json::MappingError is raised with
   * \c MISSING_FIELD error code.\n
   * \c null value is accepted only when the field can hold it ( \c std::optional , \c std::shared_ptr ,
   * \c std::unique_ptr or pointer): otherwise \ref anch::json::MappingError is raised with \c CONSTRAINT_VIOLATION
   * error code.
   *
   * \snippet{trimleft} anch-json/examples/fragments.cpp required
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct Required {};

  /*! \ref Required constraint instance */
  inline constexpr anch::json::Required required{};

  /*!
   * \brief Minimum string length constraint ( \c minLength )
   *
   * Length is the number of Unicode code points (UTF-8), as in JSON Schema.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT MinLength {
    /*! Minimum length */
    std::size_t value;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param str the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::StringLike V>
    bool check(const V& str, std::string& message) const;
  };

  /*!
   * \brief Maximum string length constraint ( \c maxLength )
   *
   * Length is the number of Unicode code points (UTF-8), as in JSON Schema.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT MaxLength {
    /*! Maximum length */
    std::size_t value;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param str the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::StringLike V>
    bool check(const V& str, std::string& message) const;
  };

  /*!
   * \brief Minimum value constraint ( \c minimum and \c exclusiveMinimum )
   *
   * Integers are compared without conversion ( \c std::cmp_less ), floating point numbers as \c long \c double .
   *
   * \tparam N the bound type
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<anch::json::Number N>
  struct Minimum {
    /*! Minimum value */
    N value;

    /*! Value has to be greater than minimum (instead of greater or equal) */
    bool exclusive = false;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param number the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::Number V>
    bool check(const V& number, std::string& message) const;
  };

  /*!
   * \brief Maximum value constraint ( \c maximum and \c exclusiveMaximum )
   *
   * Integers are compared without conversion ( \c std::cmp_less ), floating point numbers as \c long \c double .
   *
   * \tparam N the bound type
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<anch::json::Number N>
  struct Maximum {
    /*! Maximum value */
    N value;

    /*! Value has to be lower than maximum (instead of lower or equal) */
    bool exclusive = false;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param number the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::Number V>
    bool check(const V& number, std::string& message) const;
  };

  /*!
   * \brief Multiple of constraint ( \c multipleOf )
   *
   * Integers are checked exactly. With floating point numbers, the quotient has to be an integer with a relative
   * tolerance ( \c 0.3 is a multiple of \c 0.1 ).
   *
   * \tparam N the divisor type
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<anch::json::Number N>
  struct MultipleOf {
    /*! Divisor (strictly positive) */
    N value;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param number the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::Number V>
    bool check(const V& number, std::string& message) const;
  };

  /*!
   * \brief Minimum number of items constraint ( \c minItems )
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT MinItems {
    /*! Minimum number of items */
    std::size_t value;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param items the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::Collection V>
    bool check(const V& items, std::string& message) const;
  };

  /*!
   * \brief Maximum number of items constraint ( \c maxItems )
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT MaxItems {
    /*! Maximum number of items */
    std::size_t value;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param items the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::Collection V>
    bool check(const V& items, std::string& message) const;
  };

  /*!
   * \brief Minimum number of map entries constraint ( \c minProperties )
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT MinProperties {
    /*! Minimum number of entries */
    std::size_t value;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param map the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::MapLike V>
    bool check(const V& map, std::string& message) const;
  };

  /*!
   * \brief Maximum number of map entries constraint ( \c maxProperties )
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT MaxProperties {
    /*! Maximum number of entries */
    std::size_t value;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param map the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::MapLike V>
    bool check(const V& map, std::string& message) const;
  };

  /*!
   * \brief Unique items constraint ( \c uniqueItems )
   *
   * Items have to be comparable with \c == . When they are also totally ordered, duplicates are found by sorting
   * (\c O(n.log(n)) ), otherwise every pair of items is compared (\c O(n²) ).
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT UniqueItems {
    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param items the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::Collection V>
    requires std::equality_comparable<std::ranges::range_value_t<V>>
    bool check(const V& items, std::string& message) const;
  };

  /*! \ref UniqueItems constraint instance */
  inline constexpr anch::json::UniqueItems uniqueItems{};

  /*!
   * \brief Allowed values constraint ( \c enum )
   *
   * \tparam A the allowed values type
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<typename A>
  struct Allowed {
    /*! Allowed values */
    std::vector<A> values;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param value the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<typename V>
    requires std::equality_comparable_with<V, A>
    bool check(const V& value, std::string& message) const;
  };

  /*!
   * \brief Items constraints
   *
   * Constraints are checked on every item of a collection or every value of a map. Error path contains the item
   * index or the map key.
   *
   * \snippet{trimleft} anch-json/examples/fragments.cpp items
   *
   * \tparam C the items constraints types
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<typename... C>
  struct Items {
    /*! Items constraints */
    std::tuple<C...> constraints;

    /*!
     * Check every item
     *
     * \tparam V the container type
     * \tparam X the context type ( \c anch::json::ReaderContext or \c anch::json::WriterContext )
     *
     * \param items the container
     * \param context the reader or writer context
     *
     * \throw anch::json::MappingError \c CONSTRAINT_VIOLATION (path contains the item index or the map key)
     */
    template<typename V, typename X>
    void apply(const V& items, X& context) const;
  };

  /*!
   * Pattern matcher: returns \c true when the value matches the pattern
   *
   * \since 0.2
   */
  using PatternMatcher = std::function<bool(std::string_view)>;

  /*!
   * Pattern compiler: builds the matcher of a pattern (ECMA-262 regular expression for JSON Schema)
   *
   * \since 0.2
   */
  using PatternCompiler = std::function<anch::json::PatternMatcher(std::string_view)>;

  /*!
   * Set the pattern compiler used by \ref anch::json::pattern . The default compiler uses \c std::regex
   * (ECMAScript grammar) and \c std::regex_search . Another engine (RE2, PCRE2, ...) can be used for performance or
   * for UTF-8 support: \c std::regex works on bytes ( \c . matches one byte, character classes do not contain
   * multi-bytes characters).\n
   * The compiler is used when patterns are built: it has to be set before mappers registration (mappers are
   * registered on their first use).
   *
   * \param compiler the compiler, \c nullptr to restore the default compiler
   *
   * \since 0.2
   */
  ANCH_JSON_EXPORT void setPatternCompiler(anch::json::PatternCompiler compiler);

  /*!
   * \brief String pattern constraint ( \c pattern )
   *
   * The pattern is searched in the value (it is not anchored: use \c ^ and \c $ to match the whole value), as in
   * JSON Schema.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT Pattern {
    /*! Pattern matcher */
    std::shared_ptr<const anch::json::PatternMatcher> matcher;

    /*! Pattern */
    std::string source;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param str the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    template<anch::json::StringLike V>
    bool check(const V& str, std::string& message) const;
  };

  /*!
   * Format validator: returns \c true when the value is valid
   *
   * \since 0.2
   */
  using FormatValidator = std::function<bool(std::string_view)>;

  /*!
   * Register format validator (thread safe). It replaces the validator already registered with this name.\n
   * Registered formats:
   *  - \c email : dot-atom local part (64 characters at most) and host name domain (no quoted local part nor
   *    address literal) ;
   *  - \c hostname : RFC 1123 host name (labels of 63 characters at most, 253 characters at most) ;
   *  - \c ipv4 and \c ipv6 : \c inet_pton (no leading zero in IPv4 addresses, no IPv6 zone) ;
   *  - \c uri : RFC 3986 scheme followed by URI characters (percent encoding is checked, not the URI structure) ;
   *  - \c byte : base64 with padding (RFC 4648) ;
   *  - \c uuid (with \c ANCH_UUID ), \c date-time and \c date (with \c ANCH_DATE , RFC 3339, anch-date
   *    range: years 1678 to 2261 for \c date-time ).
   *
   * \param name the format name
   * \param validator the validator, \c nullptr to remove the format
   *
   * \since 0.2
   */
  ANCH_JSON_EXPORT void registerFormat(const std::string& name, anch::json::FormatValidator validator);

  /*!
   * \brief String format constraint ( \c format )
   *
   * The validator is found in the formats registry when the value is checked ( \ref anch::json::registerFormat ).
   * Unknown formats are not checked (formats are annotations in JSON Schema).
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_JSON_EXPORT Format {
    /*! Format name */
    std::string name;

    /*!
     * Check value
     *
     * \tparam V the value type
     *
     * \param str the value
     * \param message the error message (set when value is invalid)
     *
     * \return \c true when value is valid or format is unknown
     */
    template<anch::json::StringLike V>
    bool check(const V& str, std::string& message) const;
  };

  /*!
   * \brief Custom constraint
   *
   * \tparam V the value type
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<typename V>
  struct Custom {
    /*! Predicate: \c true when value is valid */
    std::function<bool(const V&)> predicate;

    /*! Error message */
    std::string message;

    /*!
     * Check value
     *
     * \param value the value
     * \param error the error message (set when value is invalid)
     *
     * \return \c true when value is valid
     */
    bool check(const V& value, std::string& error) const;
  };

  /*!
   * Value constraint which can check value of type \c V
   *
   * \tparam C the constraint type
   * \tparam V the value type
   *
   * \since 0.2
   */
  template<typename C, typename V>
  concept ValueConstraint = anch::json::detail::IsValueConstraint<std::remove_cvref_t<C>, V>::value;

  /*!
   * Field constraint which can be given to \c ObjectMapper::registerField for field of type \c P : \ref anch::json::Required or
   * value constraint which can check the value type ( \c std::optional and pointers are unwrapped)
   *
   * \tparam C the constraint type
   * \tparam P the field type
   *
   * \since 0.2
   */
  template<typename C, typename P>
  concept FieldConstraint = std::same_as<std::remove_cvref_t<C>, anch::json::Required>
    || anch::json::ValueConstraint<C, anch::json::detail::Unwrap<P>>;

  /*!
   * Check if constraints contain \ref Required
   *
   * \tparam C the constraints types
   *
   * \return \c true when one of the constraints is \ref Required
   *
   * \since 0.2
   */
  template<typename... C>
  constexpr bool isRequired() {
    return (std::same_as<std::remove_cvref_t<C>, anch::json::Required> || ...);
  }

  /*!
   * Check if constraints contain value constraints (other than \ref anch::json::Required )
   *
   * \tparam C the constraints types
   *
   * \return \c true when one of the constraints is not \ref anch::json::Required
   *
   * \since 0.2
   */
  template<typename... C>
  constexpr bool hasValueConstraints() {
    return (!std::same_as<std::remove_cvref_t<C>, anch::json::Required> || ...);
  }

  /*!
   * Check if type can hold \c null value: \c std::optional , \c std::shared_ptr , \c std::unique_ptr or pointer
   *
   * \tparam P the type
   *
   * \return \c true when type can hold \c null value
   *
   * \since 0.2
   */
  template<typename P>
  constexpr bool isNullable() {
    return anch::json::detail::IsNullable<std::remove_cvref_t<P>>::value;
  }

  // Constraints factories +
  /*!
   * \c minLength constraint
   *
   * \param length the minimum length (Unicode code points)
   *
   * \return the constraint
   *
   * \since 0.2
   */
  constexpr anch::json::MinLength minLength(std::size_t length);

  /*!
   * \c maxLength constraint
   *
   * \param length the maximum length (Unicode code points)
   *
   * \return the constraint
   *
   * \since 0.2
   */
  constexpr anch::json::MaxLength maxLength(std::size_t length);

  /*!
   * \c minimum constraint
   *
   * \tparam N the bound type
   *
   * \param value the minimum value
   * \param exclusive value has to be greater than \c value
   *
   * \return the constraint
   *
   * \since 0.2
   */
  template<anch::json::Number N>
  constexpr anch::json::Minimum<N> minimum(N value, bool exclusive = false);

  /*!
   * \c exclusiveMinimum constraint: value has to be greater than bound
   *
   * \tparam N the bound type
   *
   * \param value the bound
   *
   * \return the constraint
   *
   * \since 0.2
   */
  template<anch::json::Number N>
  constexpr anch::json::Minimum<N> exclusiveMinimum(N value);

  /*!
   * \c maximum constraint
   *
   * \tparam N the bound type
   *
   * \param value the maximum value
   * \param exclusive value has to be lower than \c value
   *
   * \return the constraint
   *
   * \since 0.2
   */
  template<anch::json::Number N>
  constexpr anch::json::Maximum<N> maximum(N value, bool exclusive = false);

  /*!
   * \c exclusiveMaximum constraint: value has to be lower than bound
   *
   * \tparam N the bound type
   *
   * \param value the bound
   *
   * \return the constraint
   *
   * \since 0.2
   */
  template<anch::json::Number N>
  constexpr anch::json::Maximum<N> exclusiveMaximum(N value);

  /*!
   * \c multipleOf constraint
   *
   * \tparam N the divisor type
   *
   * \param value the divisor
   *
   * \return the constraint
   *
   * \throw std::invalid_argument divisor is not strictly positive
   *
   * \since 0.2
   */
  template<anch::json::Number N>
  anch::json::MultipleOf<N> multipleOf(N value);

  /*!
   * \c minItems constraint
   *
   * \param count the minimum number of items
   *
   * \return the constraint
   *
   * \since 0.2
   */
  constexpr anch::json::MinItems minItems(std::size_t count);

  /*!
   * \c maxItems constraint
   *
   * \param count the maximum number of items
   *
   * \return the constraint
   *
   * \since 0.2
   */
  constexpr anch::json::MaxItems maxItems(std::size_t count);

  /*!
   * \c minProperties constraint (map entries)
   *
   * \param count the minimum number of entries
   *
   * \return the constraint
   *
   * \since 0.2
   */
  constexpr anch::json::MinProperties minProperties(std::size_t count);

  /*!
   * \c maxProperties constraint (map entries)
   *
   * \param count the maximum number of entries
   *
   * \return the constraint
   *
   * \since 0.2
   */
  constexpr anch::json::MaxProperties maxProperties(std::size_t count);

  /*!
   * \c pattern constraint
   *
   * \param source the pattern (ECMA-262 regular expression with the default compiler)
   *
   * \return the constraint
   *
   * \throw std::regex_error invalid pattern (default compiler)
   *
   * \since 0.2
   */
  ANCH_JSON_EXPORT anch::json::Pattern pattern(std::string_view source);

  /*!
   * \c format constraint
   *
   * \param name the format name
   *
   * \return the constraint
   *
   * \since 0.2
   */
  anch::json::Format format(std::string name);

  /*!
   * Allowed values constraint ( \c enum ). Character strings literals are stored as \c std::string .
   *
   * \tparam A the values type
   *
   * \param values the allowed values
   *
   * \return the constraint
   *
   * \since 0.2
   */
  template<typename A>
  auto allowed(std::initializer_list<A> values);

  /*!
   * Items constraints
   *
   * \tparam C the items constraints types
   *
   * \param constraints the constraints to check on every item
   *
   * \return the constraint
   *
   * \since 0.2
   */
  template<typename... C>
  anch::json::Items<std::decay_t<C>...> items(C&&... constraints);

  /*!
   * Custom constraint
   *
   * \tparam V the value type
   *
   * \param predicate the predicate (\c true when value is valid)
   * \param message the error message
   *
   * \return the constraint
   *
   * \since 0.2
   */
  template<typename V>
  anch::json::Custom<V> custom(std::function<bool(const V&)> predicate, std::string message);
  // Constraints factories -

}  // anch::json

#include "json/impl/constraints.hpp"
