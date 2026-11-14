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
#include <type_traits>
#include <utility>

namespace anch::json::detail {

  template<typename P>
  constexpr bool
  hasNestedObjects() {
    if constexpr (anch::json::isNullable<P>()) {
      return hasNestedObjects<anch::json::detail::Unwrap<P>>();
    } else if constexpr (anch::json::StringLike<P> || anch::json::isPrimitive<P>() || std::is_enum_v<P> || !std::is_class_v<P>) {
      return false;
    } else if constexpr (anch::json::MapLike<P>) {
      return hasNestedObjects<std::remove_cvref_t<typename P::mapped_type>>();
    } else if constexpr (anch::json::Collection<P>) {
      return hasNestedObjects<std::remove_cvref_t<std::ranges::range_value_t<P>>>();
    } else {
      return true;
    }
  }

  template<typename P, typename... C>
  [[gnu::cold]] void
  validateField(const std::string& key, const P& value, anch::json::ValidationContext& context, const C&... constraints) {
    context.enter(key);
    if constexpr (anch::json::hasValueConstraints<C...>()) {
      anch::json::detail::checkConstraints(value, context, constraints...);
    } else {
      ((void)constraints, ...);
    }
    if constexpr (hasNestedObjects<P>()) {
      validateNested(value, context);
    }
    context.leave();
  }

  template<typename P>
  [[gnu::cold]] void
  validateNested(const P& value, anch::json::ValidationContext& context) {
    if constexpr (anch::json::isNullable<P>()) {
      if(value) {
	validateNested(*value, context);
      }
    } else if constexpr (anch::json::StringLike<P> || anch::json::isPrimitive<P>() || std::is_enum_v<P> || !std::is_class_v<P>) {
      // Primitive values have no nested value
    } else if constexpr (anch::json::MapLike<P>) {
      for(const auto& [key, item] : value) {
	context.enter(key);
	validateNested(item, context);
	context.leave();
      }
    } else if constexpr (anch::json::Collection<P>) {
      std::size_t index = 0;
      for(const auto& item : value) {
	context.enter(index++);
	validateNested(item, context);
	context.leave();
      }
    } else { // Object: fields constraints
      anch::json::Factory<P>::getInstance().validateValue(value, context);
    }
  }

}  // anch::json::detail

namespace anch::json {

  template<typename T>
  std::vector<anch::json::Violation>
  validate(const T& value) {
    anch::json::ValidationContext context;
    anch::json::detail::validateNested(value, context);
    return std::move(context.violations());
  }

  template<typename T>
  bool
  isValid(const T& value) {
    return anch::json::validate(value).empty();
  }

}  // anch::json
