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

#include <ranges>
#include <utility>

#include "json/mappingError.hpp"
#include "json/readerContext.hpp"
#include "json/writerContext.hpp"

namespace anch::json::detail {

  template<typename P>
  bool
  writeEmptyRequired(const std::string& key, const P& value, anch::json::WriterContext& context) {
    if constexpr (anch::val::isNullable<P>()) {
      if(!value) {
	context.writeField(key);
	context.write(std::string_view("null"));
	return true;
      }
      return false;
    } else if constexpr (anch::val::MapLike<P>) {
      if(std::ranges::empty(value)) {
	context.writeField(key);
	context.write(std::string_view("{}"));
	return true;
      }
      return false;
    } else if constexpr (anch::val::Collection<P>) {
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

}  // anch::json::detail

namespace anch::val {

  // Reader context +
  inline bool
  CheckContext<anch::json::ReaderContext>::enabled(const anch::json::ReaderContext& context) {
    return context.options.deserialize_check_constraints;
  }

  inline void
  CheckContext<anch::json::ReaderContext>::violation(anch::json::ReaderContext& context, const std::string& message) {
    anch::json::detail::violation(context, message);
  }

  template<typename F>
  void
  CheckContext<anch::json::ReaderContext>::item([[maybe_unused]] anch::json::ReaderContext& context, std::string_view key, F&& check) {
    try {
      std::forward<F>(check)();
    } catch(anch::json::MappingError& error) {
      error.prependField(key);
      throw;
    }
  }

  template<typename F>
  void
  CheckContext<anch::json::ReaderContext>::item([[maybe_unused]] anch::json::ReaderContext& context, std::size_t index, F&& check) {
    try {
      std::forward<F>(check)();
    } catch(anch::json::MappingError& error) {
      error.prependIndex(index);
      throw;
    }
  }
  // Reader context -

  // Writer context +
  inline bool
  CheckContext<anch::json::WriterContext>::enabled(const anch::json::WriterContext& context) {
    return context.options().serialize_check_constraints;
  }

  inline void
  CheckContext<anch::json::WriterContext>::violation(anch::json::WriterContext& context, const std::string& message) {
    anch::json::detail::violation(context, message);
  }

  template<typename F>
  void
  CheckContext<anch::json::WriterContext>::item([[maybe_unused]] anch::json::WriterContext& context, std::string_view key, F&& check) {
    try {
      std::forward<F>(check)();
    } catch(anch::json::MappingError& error) {
      error.prependField(key);
      throw;
    }
  }

  template<typename F>
  void
  CheckContext<anch::json::WriterContext>::item([[maybe_unused]] anch::json::WriterContext& context, std::size_t index, F&& check) {
    try {
      std::forward<F>(check)();
    } catch(anch::json::MappingError& error) {
      error.prependIndex(index);
      throw;
    }
  }
  // Writer context -

}  // anch::val
