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

#include <cstddef>
#include <format>
#include <ranges>
#include <type_traits>
#include <utility>

namespace anch::val::detail {

  /*!
   * Check if values of type \c P can contain described objects (directly or in \c std::optional , pointers,
   * collections or maps): these values are validated
   *
   * \tparam P the value type
   *
   * \return \c true when values can contain described objects
   */
  template<typename P>
  constexpr bool
  hasNestedObjects() {
    using V = std::remove_cvref_t<P>;
    if constexpr (anch::val::isNullable<V>()) {
      return hasNestedObjects<anch::val::detail::Unwrap<V>>();
    } else if constexpr (anch::val::StringLike<V>) {
      return false;
    } else if constexpr (anch::val::MapLike<V>) {
      return hasNestedObjects<typename V::mapped_type>();
    } else if constexpr (anch::val::Collection<V>) {
      return hasNestedObjects<std::ranges::range_value_t<V>>();
    } else {
      return anch::Described<V>;
    }
  }

  /*!
   * Validate nested values
   *
   * \tparam P the value type
   *
   * \param value the value
   * \param context the validation context
   */
  template<typename P>
  [[gnu::cold]] void
  validateNested(const P& value, anch::val::ValidationContext& context) {
    using V = std::remove_cvref_t<P>;
    if constexpr (!hasNestedObjects<V>()) {
      (void)value;
      (void)context;
    } else if constexpr (anch::val::isNullable<V>()) {
      if(value) {
	validateNested(*value, context);
      }
    } else if constexpr (anch::val::MapLike<V>) {
      for(const auto& [key, item] : value) {
	if constexpr (anch::val::StringLike<std::remove_cvref_t<decltype(key)>>) {
	  context.enter(std::string_view(key));
	} else {
	  context.enter(std::string_view(std::format("{}", key)));
	}
	validateNested(item, context);
	context.leave();
      }
    } else if constexpr (anch::val::Collection<V>) {
      std::size_t index = 0;
      for(const auto& item : value) {
	context.enter(index++);
	validateNested(item, context);
	context.leave();
      }
    } else {
      anch::val::ValidationMapper<V>::instance().validate(value, context);
    }
  }

}

namespace anch::val {

  template<typename T>
  ValidationMapper<T>::ValidationMapper(): _validators() {
    anch::describe<T>(*this);
    _validators.shrink_to_fit();
  }

  template<typename T>
  const ValidationMapper<T>&
  ValidationMapper<T>::instance() {
    static const ValidationMapper<T> mapper;
    return mapper;
  }

  template<typename T>
  void
  ValidationMapper<T>::validate(const T& value, anch::val::ValidationContext& context) const {
    for(const auto& validator : _validators) {
      validator(value, context);
    }
  }

  template<typename T>
  template<typename G, typename... C>
  void
  ValidationMapper<T>::add(const std::string& name, G get, C&&... constraints) {
    using P = std::remove_cvref_t<std::invoke_result_t<G, const T&>>;
    if constexpr (anch::val::hasValueConstraints<std::remove_cvref_t<C>...>() || anch::val::detail::hasNestedObjects<P>()) {
      _validators.push_back([name, get, ...cts = std::forward<C>(constraints)](const T& obj, anch::val::ValidationContext& context) -> void {
	context.enter(name);
	decltype(auto) value = std::invoke(get, obj);
	if constexpr (anch::val::hasValueConstraints<std::remove_cvref_t<C>...>()) {
	  anch::val::detail::checkConstraints(value, context, cts...);
	} else {
	  ((void)cts, ...);
	}
	anch::val::detail::validateNested(value, context);
	context.leave();
      });
    } else {
      (void)name;
      (void)get;
      ((void)constraints, ...);
    }
  }

  template<typename T>
  template<typename P, typename... C>
  ValidationMapper<T>&
  ValidationMapper<T>::registerField(const std::string& name, P T::* member, C&&... constraints) {
    add(name, [member](const T& obj) -> const P& { return obj.*member; }, std::forward<C>(constraints)...);
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  ValidationMapper<T>&
  ValidationMapper<T>::registerField(const std::string& name, std::function<P(const T&)> getter, C&&... constraints) {
    add(name, std::move(getter), std::forward<C>(constraints)...);
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  ValidationMapper<T>&
  ValidationMapper<T>::registerField([[maybe_unused]] const std::string& name, [[maybe_unused]] std::function<void(T&, const P&)> setter,
				     [[maybe_unused]] C&&... constraints) {
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  ValidationMapper<T>&
  ValidationMapper<T>::registerField(const std::string& name, std::function<P(const T&)> getter,
				     [[maybe_unused]] std::function<void(T&, const P&)> setter, C&&... constraints) {
    add(name, std::move(getter), std::forward<C>(constraints)...);
    return *this;
  }

  template<typename T>
  std::vector<anch::val::Violation>
  validate(const T& value) {
    anch::val::ValidationContext context;
    anch::val::detail::validateNested(value, context);
    return std::move(context.violations());
  }

  template<typename T>
  bool
  isValid(const T& value) {
    return anch::val::validate(value).empty();
  }

}
