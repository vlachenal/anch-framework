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

#include <charconv>
#include <sstream>
#include <string_view>
#include <stdexcept>
#include <system_error>
#include <utility>

#include "rest/httpException.hpp"

namespace anch::rest {

  template<typename T>
  requires (std::integral<T> && !std::same_as<T, bool>)
  std::optional<T>
  ParamConverter<T>::convert(std::string_view value) noexcept {
    T res{};
    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), res);
    if(value.empty() || ec != std::errc() || ptr != value.data() + value.size()) {
      return std::nullopt;
    }
    return res;
  }

  template<std::floating_point T>
  std::optional<T>
  ParamConverter<T>::convert(std::string_view value) noexcept {
    T res{};
    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), res);
    if(value.empty() || ec != std::errc() || ptr != value.data() + value.size()) {
      return std::nullopt;
    }
    return res;
  }

  inline std::optional<bool>
  ParamConverter<bool>::convert(std::string_view value) noexcept {
    if(value == "true") {
      return true;
    }
    if(value == "false") {
      return false;
    }
    return std::nullopt;
  }

  inline std::optional<std::string>
  ParamConverter<std::string>::convert(std::string_view value) {
    return std::string(value);
  }

  inline std::optional<anch::UUID>
  ParamConverter<anch::UUID>::convert(std::string_view value) {
    try {
      return anch::UUID(std::string(value));
    } catch(const std::invalid_argument&) {
      return std::nullopt;
    }
  }

  template<anch::rest::PathParameter T>
  std::optional<T>
  queryParameter(const anch::rest::Request& request, std::string_view name) {
    std::optional<std::string_view> value = request.getQueryParameter(name);
    if(!value) {
      return std::nullopt;
    }
    std::optional<T> res = anch::rest::ParamConverter<T>::convert(*value);
    if(!res) {
      throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST,
				      "Invalid query parameter '" + std::string(name) + "': '" + std::string(*value) + "'");
    }
    return res;
  }

  template<anch::rest::PathParameter T>
  T
  requiredQueryParameter(const anch::rest::Request& request, std::string_view name) {
    std::optional<T> res = anch::rest::queryParameter<T>(request, name);
    if(!res) {
      throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, "Missing query parameter '" + std::string(name) + "'");
    }
    return std::move(*res);
  }

  template<anch::rest::PathParameter T>
  std::optional<T>
  formField(const anch::rest::Form& form, std::string_view name) {
    std::optional<std::string_view> value = form.field(name);
    if(!value) {
      return std::nullopt;
    }
    std::optional<T> res = anch::rest::ParamConverter<T>::convert(*value);
    if(!res) {
      throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST,
				      "Invalid form field '" + std::string(name) + "': '" + std::string(*value) + "'");
    }
    return res;
  }

  template<anch::rest::PathParameter T>
  T
  requiredFormField(const anch::rest::Form& form, std::string_view name) {
    std::optional<T> res = anch::rest::formField<T>(form, name);
    if(!res) {
      throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, "Missing form field '" + std::string(name) + "'");
    }
    return std::move(*res);
  }

  template<anch::rest::ParameterValue T>
  std::string
  toParameterString(const T& value) {
    if constexpr (std::is_same_v<T, bool>) {
      return value ? "true" : "false";
    } else if constexpr (std::convertible_to<const T&, std::string_view>) {
      return std::string(std::string_view(value));
    } else if constexpr (std::is_arithmetic_v<T>) {
      char buffer[64];
      auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);
      return std::string(buffer, ptr);
    } else {
      std::ostringstream out;
      out << value;
      return out.str();
    }
  }

}
