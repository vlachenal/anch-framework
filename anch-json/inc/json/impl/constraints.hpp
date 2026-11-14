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

#include "json/mappingError.hpp"
#include "json/readerContext.hpp"
#include "json/writerContext.hpp"

namespace anch::json::detail {

  /*! Constraint is not an \ref anch::json::Items constraint */
  template<typename C>
  struct IsItems: std::false_type {};

  /*! Constraint is an \ref anch::json::Items constraint */
  template<typename... C>
  struct IsItems<anch::json::Items<C...>>: std::true_type {};

  /*!
   * Compare numbers: integers without conversion, floating point numbers as \c long \c double
   *
   * \return \c true when \c a is lower than \c b
   */
  template<anch::json::Number A, anch::json::Number B>
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
    if constexpr (anch::json::Number<A> && anch::json::Number<B> && std::integral<A> && std::integral<B>) {
      return std::cmp_equal(a, b);
    } else {
      return a == b;
    }
  }

  template<typename V>
  std::string
  formatValue(const V& value) {
    if constexpr (anch::json::StringLike<V>) {
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

  inline
  bool
  checkEnabled(const anch::json::ReaderContext& context) {
    return context.options.deserialize_check_constraints;
  }

  inline
  bool
  checkEnabled(const anch::json::WriterContext& context) {
    return context.options().serialize_check_constraints;
  }

  inline
  bool
  checkEnabled([[maybe_unused]] const anch::json::ValidationContext& context) {
    return true;
  }

  inline
  void
  violation(anch::json::ValidationContext& context, const std::string& message) {
    context.add(message);
  }

  template<typename P>
  bool
  writeEmptyRequired(const std::string& key, const P& value, anch::json::WriterContext& context) {
    if constexpr (anch::json::isNullable<P>()) {
      if(!value) {
	context.writeField(key);
	context.write(std::string_view("null"));
	return true;
      }
      return false;
    } else if constexpr (anch::json::MapLike<P>) {
      if(std::ranges::empty(value)) {
	context.writeField(key);
	context.write(std::string_view("{}"));
	return true;
      }
      return false;
    } else if constexpr (anch::json::Collection<P>) {
      if(std::ranges::empty(value)) {
	context.writeField(key);
	context.write(std::string_view("[]"));
	return true;
      }
      return false;
    } else {
      return false;
    }
  }

  template<typename C, typename P, typename X>
  void
  applyConstraint(const C& constraint, const P& value, X& context) {
    if constexpr (std::same_as<C, anch::json::Required>) {
      // Required is checked by object mapper
    } else if constexpr (anch::json::isNullable<P>()) {
      if(value) {
	applyConstraint(constraint, *value, context);
      }
    } else if constexpr (IsItems<C>::value) {
      constraint.apply(value, context);
    } else {
      std::string message;
      if(!constraint.check(value, message)) [[unlikely]] {
	anch::json::detail::violation(context, message);
      }
    }
  }

  template<typename P, typename X, typename... C>
  void
  checkConstraints(const P& value, X& context, const C&... constraints) {
    if(!anch::json::detail::checkEnabled(context)) {
      return;
    }
    (applyConstraint(constraints, value, context), ...);
  }

}  // anch::json::detail

namespace anch::json {

  // Validation context +
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
    _violations.push_back(anch::json::Violation{std::move(path), message});
  }

  inline
  std::vector<anch::json::Violation>&
  ValidationContext::violations() {
    return _violations;
  }
  // Validation context -

  // String constraints +
  template<anch::json::StringLike V>
  bool
  MinLength::check(const V& str, std::string& message) const {
    std::size_t length = anch::json::detail::codePoints(std::string_view(str));
    if(length < value) {
      message = std::format("Length {} is lower than minLength {}", length, value);
      return false;
    }
    return true;
  }

  template<anch::json::StringLike V>
  bool
  MaxLength::check(const V& str, std::string& message) const {
    std::string_view view(str);
    if(view.size() <= value) { // fast path: there is at least one byte per code point
      return true;
    }
    std::size_t length = anch::json::detail::codePoints(view);
    if(length > value) {
      message = std::format("Length {} is greater than maxLength {}", length, value);
      return false;
    }
    return true;
  }
  template<anch::json::StringLike V>
  bool
  Pattern::check(const V& str, std::string& message) const {
    if(std::invoke(*matcher, std::string_view(str))) {
      return true;
    }
    message = "Value does not match pattern " + source;
    return false;
  }

  template<anch::json::StringLike V>
  bool
  Format::check(const V& str, std::string& message) const {
    if(anch::json::detail::matchesFormat(name, std::string_view(str))) {
      return true;
    }
    message = "Value is not a valid " + name + " (format)";
    return false;
  }
  // String constraints -

  // Number constraints +
  template<anch::json::Number N>
  template<anch::json::Number V>
  bool
  Minimum<N>::check(const V& number, std::string& message) const {
    bool invalid = exclusive ? !anch::json::detail::lessThan(value, number) : anch::json::detail::lessThan(number, value);
    if(invalid) {
      message = std::format("Value {} is {} {}", number, (exclusive ? "lower than or equal to exclusiveMinimum" : "lower than minimum"), value);
      return false;
    }
    return true;
  }

  template<anch::json::Number N>
  template<anch::json::Number V>
  bool
  Maximum<N>::check(const V& number, std::string& message) const {
    bool invalid = exclusive ? !anch::json::detail::lessThan(number, value) : anch::json::detail::lessThan(value, number);
    if(invalid) {
      message = std::format("Value {} is {} {}", number, (exclusive ? "greater than or equal to exclusiveMaximum" : "greater than maximum"), value);
      return false;
    }
    return true;
  }

  template<anch::json::Number N>
  template<anch::json::Number V>
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
  template<anch::json::Collection V>
  bool
  MinItems::check(const V& items, std::string& message) const {
    std::size_t size = std::ranges::size(items);
    if(size < value) {
      message = std::format("Item count {} is lower than minItems {}", size, value);
      return false;
    }
    return true;
  }

  template<anch::json::Collection V>
  bool
  MaxItems::check(const V& items, std::string& message) const {
    std::size_t size = std::ranges::size(items);
    if(size > value) {
      message = std::format("Item count {} is greater than maxItems {}", size, value);
      return false;
    }
    return true;
  }

  template<anch::json::MapLike V>
  bool
  MinProperties::check(const V& map, std::string& message) const {
    std::size_t size = std::ranges::size(map);
    if(size < value) {
      message = std::format("Property count {} is lower than minProperties {}", size, value);
      return false;
    }
    return true;
  }

  template<anch::json::MapLike V>
  bool
  MaxProperties::check(const V& map, std::string& message) const {
    std::size_t size = std::ranges::size(map);
    if(size > value) {
      message = std::format("Property count {} is greater than maxProperties {}", size, value);
      return false;
    }
    return true;
  }

  template<anch::json::Collection V>
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
	(anch::json::detail::applyConstraint(cts, item, context), ...);
      }, constraints);
    };
    if constexpr (std::same_as<X, anch::json::ValidationContext>) { // validation goes on after violations
      if constexpr (anch::json::MapLike<V>) {
	for(const auto& [key, item] : items) {
	  context.enter(key);
	  check(item);
	  context.leave();
	}
      } else {
	std::size_t index = 0;
	for(const auto& item : items) {
	  context.enter(index++);
	  check(item);
	  context.leave();
	}
      }
    } else if constexpr (anch::json::MapLike<V>) {
      static_assert(anch::json::StringLike<typename V::key_type>, "Map keys have to be strings");
      for(const auto& [key, item] : items) {
	try {
	  check(item);
	} catch(anch::json::MappingError& error) {
	  error.prependField(key);
	  throw;
	}
      }
    } else {
      std::size_t index = 0;
      for(const auto& item : items) {
	try {
	  check(item);
	} catch(anch::json::MappingError& error) {
	  error.prependIndex(index);
	  throw;
	}
	++index;
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
      if(anch::json::detail::equals(value, allowed)) {
	return true;
      }
    }
    std::string formatted = anch::json::detail::formatValue(value);
    message = formatted.empty() ? std::string("Value is not allowed") : "Value " + formatted + " is not allowed";
    // Allowed values are listed when they can be formatted
    std::string list;
    for(const A& allowed : values) {
      std::string item = anch::json::detail::formatValue(allowed);
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
  constexpr anch::json::MinLength
  minLength(std::size_t length) {
    return anch::json::MinLength{length};
  }

  constexpr anch::json::MaxLength
  maxLength(std::size_t length) {
    return anch::json::MaxLength{length};
  }

  template<anch::json::Number N>
  constexpr anch::json::Minimum<N>
  minimum(N value, bool exclusive) {
    return anch::json::Minimum<N>{value, exclusive};
  }

  template<anch::json::Number N>
  constexpr anch::json::Minimum<N>
  exclusiveMinimum(N value) {
    return anch::json::Minimum<N>{value, true};
  }

  template<anch::json::Number N>
  constexpr anch::json::Maximum<N>
  maximum(N value, bool exclusive) {
    return anch::json::Maximum<N>{value, exclusive};
  }

  template<anch::json::Number N>
  constexpr anch::json::Maximum<N>
  exclusiveMaximum(N value) {
    return anch::json::Maximum<N>{value, true};
  }

  template<anch::json::Number N>
  anch::json::MultipleOf<N>
  multipleOf(N value) {
    if(!(value > N{0})) {
      throw std::invalid_argument("multipleOf divisor has to be strictly positive");
    }
    return anch::json::MultipleOf<N>{value};
  }

  constexpr anch::json::MinItems
  minItems(std::size_t count) {
    return anch::json::MinItems{count};
  }

  constexpr anch::json::MaxItems
  maxItems(std::size_t count) {
    return anch::json::MaxItems{count};
  }

  constexpr anch::json::MinProperties
  minProperties(std::size_t count) {
    return anch::json::MinProperties{count};
  }

  constexpr anch::json::MaxProperties
  maxProperties(std::size_t count) {
    return anch::json::MaxProperties{count};
  }

  inline
  anch::json::Format
  format(std::string name) {
    return anch::json::Format{std::move(name)};
  }

  template<typename A>
  auto
  allowed(std::initializer_list<A> values) {
    if constexpr (std::same_as<std::decay_t<A>, const char*> || std::same_as<std::decay_t<A>, char*>) {
      return anch::json::Allowed<std::string>{std::vector<std::string>(values.begin(), values.end())};
    } else {
      return anch::json::Allowed<A>{std::vector<A>(values)};
    }
  }

  template<typename... C>
  anch::json::Items<std::decay_t<C>...>
  items(C&&... constraints) {
    return anch::json::Items<std::decay_t<C>...>{std::tuple<std::decay_t<C>...>(std::forward<C>(constraints)...)};
  }

  template<typename V>
  anch::json::Custom<V>
  custom(std::function<bool(const V&)> predicate, std::string message) {
    return anch::json::Custom<V>{std::move(predicate), std::move(message)};
  }
  // Constraints factories -

}  // anch::json
