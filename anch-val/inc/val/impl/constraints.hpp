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

#include <algorithm>
#include <cmath>
#include <format>
#include <limits>
#include <stdexcept>
#include <utility>


namespace anch::val::detail {

  /*! Constraint is not an \ref anch::val::Items constraint */
  template<typename C>
  struct IsItems: std::false_type {};

  /*! Constraint is an \ref anch::val::Items constraint */
  template<typename... C>
  struct IsItems<anch::val::Items<C...>>: std::true_type {};

  /*!
   * Compare numbers: integers without conversion, floating point numbers as \c long \c double
   *
   * \return \c true when \c a is lower than \c b
   */
  template<anch::val::Number A, anch::val::Number B>
  constexpr bool
  lessThan(A a, B b) {
    if constexpr (std::integral<A> && std::integral<B>) {
      return std::cmp_less(a, b);
    } else {
      return static_cast<long double>(a) < static_cast<long double>(b);
    }
  }

  /*!
   * Compare values (integers are compared without conversion)
   *
   * \return \c true when values are equal
   */
  template<typename A, typename B>
  constexpr bool
  equals(const A& a, const B& b) {
    if constexpr (anch::val::Number<A> && anch::val::Number<B> && std::integral<A> && std::integral<B>) {
      return std::cmp_equal(a, b);
    } else {
      return a == b;
    }
  }

  template<typename V>
  std::string
  formatValue(const V& value) {
    if constexpr (anch::val::StringLike<V>) {
      std::string res = "'";
      res += std::string_view(value);
      res += '\'';
      return res;
    } else if constexpr (std::is_default_constructible_v<std::formatter<V, char>>) {
      return std::format("{}", value);
    } else {
      return std::string();
    }
  }

  template<typename C, typename P, typename X>
  void
  applyConstraint(const C& constraint, const P& value, X& context) {
    if constexpr (std::same_as<C, anch::val::Required>) {
      // Required is checked by object mapper
    } else if constexpr (anch::val::isNullable<P>()) {
      if(value) {
	applyConstraint(constraint, *value, context);
      }
    } else if constexpr (IsItems<C>::value) {
      constraint.apply(value, context);
    } else {
      std::string message;
      if(!constraint.check(value, message)) [[unlikely]] {
	anch::val::CheckContext<X>::violation(context, message);
      }
    }
  }

  template<typename P, typename X, typename... C>
  void
  checkConstraints(const P& value, X& context, const C&... constraints) {
    if(!anch::val::CheckContext<X>::enabled(context)) {
      return;
    }
    (applyConstraint(constraints, value, context), ...);
  }

}  // anch::val::detail

namespace anch::val {

  // Validation context +
  /*!
   * \brief Constraints checked by a validation: every constraint is checked and violations are collected
   */
  template<>
  struct CheckContext<anch::val::ValidationContext> {

    /*!
     * Constraints are always checked by a validation
     *
     * \return \c true
     */
    static bool
    enabled(const anch::val::ValidationContext&) noexcept {
      return true;
    }

    /*!
     * Add violation (validation goes on)
     *
     * \param context the validation context
     * \param message the violation message
     */
    static void
    violation(anch::val::ValidationContext& context, const std::string& message) {
      context.add(message);
    }

    /*!
     * Check map value
     *
     * \param context the validation context
     * \param key the map key
     * \param check the check function
     */
    template<typename F>
    static void
    item(anch::val::ValidationContext& context, std::string_view key, F&& check) {
      context.enter(key);
      std::forward<F>(check)();
      context.leave();
    }

    /*!
     * Check collection item
     *
     * \param context the validation context
     * \param index the item index
     * \param check the check function
     */
    template<typename F>
    static void
    item(anch::val::ValidationContext& context, std::size_t index, F&& check) {
      context.enter(index);
      std::forward<F>(check)();
      context.leave();
    }

  };

  inline
  void
  ValidationContext::enter(std::string_view field) {
    std::string segment;
    segment.reserve(field.size());
    for(char c : field) {
      switch(c) {
      case '~':
	segment += "~0";
	break;
      case '/':
	segment += "~1";
	break;
      default:
	segment += c;
      }
    }
    _path.push_back(std::move(segment));
  }

  inline
  void
  ValidationContext::enter(std::size_t index) {
    _path.push_back(std::to_string(index));
  }

  inline
  void
  ValidationContext::leave() {
    _path.pop_back();
  }

  inline
  void
  ValidationContext::add(const std::string& message) {
    std::string path;
    for(const std::string& segment : _path) {
      path += '/';
      path += segment;
    }
    _violations.push_back(anch::val::Violation{std::move(path), message});
  }

  inline
  std::vector<anch::val::Violation>&
  ValidationContext::violations() {
    return _violations;
  }
  // Validation context -

  // String constraints +
  template<anch::val::StringLike V>
  bool
  MinLength::check(const V& str, std::string& message) const {
    std::size_t length = anch::val::detail::codePoints(std::string_view(str));
    if(length < value) {
      message = std::format("Length {} is lower than minLength {}", length, value);
      return false;
    }
    return true;
  }

  template<anch::val::StringLike V>
  bool
  MaxLength::check(const V& str, std::string& message) const {
    std::string_view view(str);
    if(view.size() <= value) { // fast path: there is at least one byte per code point
      return true;
    }
    std::size_t length = anch::val::detail::codePoints(view);
    if(length > value) {
      message = std::format("Length {} is greater than maxLength {}", length, value);
      return false;
    }
    return true;
  }
  template<anch::val::StringLike V>
  bool
  Pattern::check(const V& str, std::string& message) const {
    if(std::invoke(*matcher, std::string_view(str))) {
      return true;
    }
    message = "Value does not match pattern " + source;
    return false;
  }

  template<anch::val::StringLike V>
  bool
  Format::check(const V& str, std::string& message) const {
    if(anch::val::detail::matchesFormat(name, std::string_view(str))) {
      return true;
    }
    message = "Value is not a valid " + name + " (format)";
    return false;
  }
  // String constraints -

  // Number constraints +
  template<anch::val::Number N>
  template<anch::val::Number V>
  bool
  Minimum<N>::check(const V& number, std::string& message) const {
    bool invalid = exclusive ? !anch::val::detail::lessThan(value, number) : anch::val::detail::lessThan(number, value);
    if(invalid) {
      message = std::format("Value {} is {} {}", number, (exclusive ? "lower than or equal to exclusiveMinimum" : "lower than minimum"), value);
      return false;
    }
    return true;
  }

  template<anch::val::Number N>
  template<anch::val::Number V>
  bool
  Maximum<N>::check(const V& number, std::string& message) const {
    bool invalid = exclusive ? !anch::val::detail::lessThan(number, value) : anch::val::detail::lessThan(value, number);
    if(invalid) {
      message = std::format("Value {} is {} {}", number, (exclusive ? "greater than or equal to exclusiveMaximum" : "greater than maximum"), value);
      return false;
    }
    return true;
  }

  template<anch::val::Number N>
  template<anch::val::Number V>
  bool
  MultipleOf<N>::check(const V& number, std::string& message) const {
    bool valid;
    if constexpr (std::integral<V> && std::integral<N>) {
      uintmax_t magnitude;
      if constexpr (std::is_signed_v<V>) {
	magnitude = (number < 0) ? uintmax_t{0} - static_cast<uintmax_t>(number) : static_cast<uintmax_t>(number);
      } else {
	magnitude = static_cast<uintmax_t>(number);
      }
      valid = (magnitude % static_cast<uintmax_t>(value)) == 0;
    } else {
      long double quotient = static_cast<long double>(number) / static_cast<long double>(value);
      valid = std::isfinite(quotient)
	&& std::fabs(quotient - std::round(quotient)) <= 64.0L * std::numeric_limits<double>::epsilon() * std::max(1.0L, std::fabs(quotient));
    }
    if(!valid) {
      message = std::format("Value {} is not a multiple of {}", number, value);
    }
    return valid;
  }
  // Number constraints -

  // Containers constraints +
  template<anch::val::Collection V>
  bool
  MinItems::check(const V& items, std::string& message) const {
    std::size_t size = std::ranges::size(items);
    if(size < value) {
      message = std::format("Item count {} is lower than minItems {}", size, value);
      return false;
    }
    return true;
  }

  template<anch::val::Collection V>
  bool
  MaxItems::check(const V& items, std::string& message) const {
    std::size_t size = std::ranges::size(items);
    if(size > value) {
      message = std::format("Item count {} is greater than maxItems {}", size, value);
      return false;
    }
    return true;
  }

  template<anch::val::MapLike V>
  bool
  MinProperties::check(const V& map, std::string& message) const {
    std::size_t size = std::ranges::size(map);
    if(size < value) {
      message = std::format("Property count {} is lower than minProperties {}", size, value);
      return false;
    }
    return true;
  }

  template<anch::val::MapLike V>
  bool
  MaxProperties::check(const V& map, std::string& message) const {
    std::size_t size = std::ranges::size(map);
    if(size > value) {
      message = std::format("Property count {} is greater than maxProperties {}", size, value);
      return false;
    }
    return true;
  }

  template<anch::val::Collection V>
  requires std::equality_comparable<std::ranges::range_value_t<V>>
  bool
  UniqueItems::check(const V& items, std::string& message) const {
    using E = std::ranges::range_value_t<V>;
    static_assert(std::is_lvalue_reference_v<std::ranges::range_reference_t<const V>>,
		  "uniqueItems needs references to items (std::vector<bool> is not supported)");
    std::vector<std::pair<const E*, std::size_t>> refs;
    refs.reserve(std::ranges::size(items));
    std::size_t index = 0;
    for(const E& item : items) {
      refs.emplace_back(&item, index++);
    }
    auto duplicate = [&message](std::size_t first, std::size_t second) -> bool {
      message = std::format("Items {} and {} are equal (uniqueItems)", std::min(first, second), std::max(first, second));
      return false;
    };
    if constexpr (std::totally_ordered<E>) {
      std::ranges::stable_sort(refs, [](const auto& a, const auto& b) -> bool { return *a.first < *b.first; });
      for(std::size_t i = 1 ; i < refs.size() ; ++i) {
	if(*refs[i - 1].first == *refs[i].first) {
	  return duplicate(refs[i - 1].second, refs[i].second);
	}
      }
    } else {
      for(std::size_t i = 0 ; i < refs.size() ; ++i) {
	for(std::size_t j = i + 1 ; j < refs.size() ; ++j) {
	  if(*refs[i].first == *refs[j].first) {
	    return duplicate(i, j);
	  }
	}
      }
    }
    return true;
  }

  template<typename... C>
  template<typename V, typename X>
  void
  Items<C...>::apply(const V& items, X& context) const {
    auto check = [this, &context](const auto& item) -> void {
      std::apply([&item, &context](const auto&... cts) -> void {
	(anch::val::detail::applyConstraint(cts, item, context), ...);
      }, constraints);
    };
    if constexpr (anch::val::MapLike<V>) {
      static_assert(anch::val::StringLike<typename V::key_type>, "Map keys have to be strings");
      for(const auto& [key, item] : items) {
	anch::val::CheckContext<X>::item(context, std::string_view(key), [&check, &item]() { check(item); });
      }
    } else {
      std::size_t index = 0;
      for(const auto& item : items) {
	anch::val::CheckContext<X>::item(context, index++, [&check, &item]() { check(item); });
      }
    }
  }

  // Containers constraints -

  // Other constraints +
  template<typename A>
  template<typename V>
  requires std::equality_comparable_with<V, A>
  bool
  Allowed<A>::check(const V& value, std::string& message) const {
    for(const A& allowed : values) {
      if(anch::val::detail::equals(value, allowed)) {
	return true;
      }
    }
    std::string formatted = anch::val::detail::formatValue(value);
    message = formatted.empty() ? std::string("Value is not allowed") : "Value " + formatted + " is not allowed";
    // Allowed values are listed when they can be formatted
    std::string list;
    for(const A& allowed : values) {
      std::string item = anch::val::detail::formatValue(allowed);
      if(item.empty()) {
	return false;
      }
      list += list.empty() ? item : ", " + item;
    }
    if(!list.empty()) {
      message += " (allowed: " + list + ")";
    }
    return false;
  }

  template<typename V>
  bool
  Custom<V>::check(const V& value, std::string& error) const {
    if(std::invoke(predicate, value)) {
      return true;
    }
    error = message;
    return false;
  }
  // Other constraints -

  // Constraints factories +
  constexpr anch::val::MinLength
  minLength(std::size_t length) {
    return anch::val::MinLength{length};
  }

  constexpr anch::val::MaxLength
  maxLength(std::size_t length) {
    return anch::val::MaxLength{length};
  }

  template<anch::val::Number N>
  constexpr anch::val::Minimum<N>
  minimum(N value, bool exclusive) {
    return anch::val::Minimum<N>{value, exclusive};
  }

  template<anch::val::Number N>
  constexpr anch::val::Minimum<N>
  exclusiveMinimum(N value) {
    return anch::val::Minimum<N>{value, true};
  }

  template<anch::val::Number N>
  constexpr anch::val::Maximum<N>
  maximum(N value, bool exclusive) {
    return anch::val::Maximum<N>{value, exclusive};
  }

  template<anch::val::Number N>
  constexpr anch::val::Maximum<N>
  exclusiveMaximum(N value) {
    return anch::val::Maximum<N>{value, true};
  }

  template<anch::val::Number N>
  anch::val::MultipleOf<N>
  multipleOf(N value) {
    if(!(value > N{0})) {
      throw std::invalid_argument("multipleOf divisor has to be strictly positive");
    }
    return anch::val::MultipleOf<N>{value};
  }

  constexpr anch::val::MinItems
  minItems(std::size_t count) {
    return anch::val::MinItems{count};
  }

  constexpr anch::val::MaxItems
  maxItems(std::size_t count) {
    return anch::val::MaxItems{count};
  }

  constexpr anch::val::MinProperties
  minProperties(std::size_t count) {
    return anch::val::MinProperties{count};
  }

  constexpr anch::val::MaxProperties
  maxProperties(std::size_t count) {
    return anch::val::MaxProperties{count};
  }

  inline
  anch::val::Format
  format(std::string name) {
    return anch::val::Format{std::move(name)};
  }

  template<typename A>
  auto
  allowed(std::initializer_list<A> values) {
    if constexpr (std::same_as<std::decay_t<A>, const char*> || std::same_as<std::decay_t<A>, char*>) {
      return anch::val::Allowed<std::string>{std::vector<std::string>(values.begin(), values.end())};
    } else {
      return anch::val::Allowed<A>{std::vector<A>(values)};
    }
  }

  template<typename... C>
  anch::val::Items<std::decay_t<C>...>
  items(C&&... constraints) {
    return anch::val::Items<std::decay_t<C>...>{std::tuple<std::decay_t<C>...>(std::forward<C>(constraints)...)};
  }

  template<typename V>
  anch::val::Custom<V>
  custom(std::function<bool(const V&)> predicate, std::string message) {
    return anch::val::Custom<V>{std::move(predicate), std::move(message)};
  }
  // Constraints factories -

}  // anch::val
