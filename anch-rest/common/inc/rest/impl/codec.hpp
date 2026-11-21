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

#include <iterator>
#include <optional>
#include <stdexcept>
#include <utility>

namespace anch::rest::detail {

  /*!
   * Parse codec media type
   *
   * \param mediaType the codec media type
   *
   * \return the media type
   *
   * \throw std::invalid_argument when media type is invalid
   */
  inline anch::rest::MediaType
  codecMediaType(std::string_view mediaType) {
    std::optional<anch::rest::MediaType> res = anch::rest::MediaType::parse(mediaType);
    if(!res) {
      throw std::invalid_argument("Invalid codec media type '" + std::string(mediaType) + "'");
    }
    return std::move(*res);
  }

}

namespace anch::rest {

  inline std::string
  readContent(std::istream& input, std::size_t maxSize) {
    std::string res;
    char buffer[8192];
    while(input) {
      input.read(buffer, sizeof(buffer));
      const auto count = static_cast<std::size_t>(input.gcount());
      if(count == 0) {
	break;
      }
      if(count > maxSize - res.size()) {
	throw anch::rest::ContentTooLarge("Content is larger than " + std::to_string(maxSize) + " bytes");
      }
      res.append(buffer, count);
    }
    return res;
  }

  inline std::string
  readBody(std::istream& input, anch::rest::Side side) {
    const std::size_t maxSize = anch::rest::maxBodySize(side);
    return anch::rest::readContent(input, maxSize == 0 ? std::numeric_limits<std::size_t>::max() : maxSize);
  }

  template<typename C, typename T>
  requires anch::rest::Serializer<C, T>
  anch::rest::MediaType
  contentTypeOf(const T& value) {
    if constexpr (requires { { C::contentType(value) } -> std::convertible_to<anch::rest::MediaType>; }) {
      return C::contentType(value);
    } else {
      return anch::rest::detail::codecMediaType(C::mediaType());
    }
  }

  template<typename C, typename T>
  requires anch::rest::Serializer<C, T>
  void
  serializeBody(const T& value, std::ostream& out, [[maybe_unused]] const anch::rest::MediaType& contentType,
		[[maybe_unused]] anch::rest::Side side) {
    if constexpr (requires { C::serialize(value, out, contentType, side); }) {
      C::serialize(value, out, contentType, side);
    } else if constexpr (requires { C::serialize(value, out, side); }) {
      C::serialize(value, out, side);
    } else {
      C::serialize(value, out);
    }
  }

  template<typename C, typename T>
  requires anch::rest::Deserializer<C, T>
  void
  deserializeBody(T& value, std::istream& input, [[maybe_unused]] const anch::rest::MediaType& contentType,
		  [[maybe_unused]] anch::rest::Side side) {
    if constexpr (requires { C::deserialize(value, input, contentType, side); }) {
      C::deserialize(value, input, contentType, side);
    } else if constexpr (requires { C::deserialize(value, input, side); }) {
      C::deserialize(value, input, side);
    } else {
      C::deserialize(value, input);
    }
  }

  inline std::string_view
  Text::mediaType() noexcept {
    return "text/plain";
  }

  template<typename T>
  requires requires(std::ostream& out, const T& value) { out << value; }
  void
  Text::serialize(const T& value, std::ostream& out) {
    out << value;
  }

  inline void
  Text::deserialize(std::string& value, std::istream& input) {
    value.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
  }

}
