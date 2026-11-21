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

namespace anch::codec {

  template<typename... Cs>
  std::vector<anch::codec::MediaType>
  Codecs<Cs...>::mediaTypes() {
    return std::vector<anch::codec::MediaType>{anch::codec::detail::codecMediaType(Cs::mediaType())...};
  }

  template<typename... Cs>
  template<typename T>
  bool
  Codecs<Cs...>::deserialize(T& value, std::istream& input, const anch::codec::MediaType& contentType,
			     anch::codec::Side side) {
    static_assert(DESERIALIZES<T>, "No codec can deserialize this type");
    auto tryCodec = [&]<typename C>() {
      if constexpr (anch::codec::Deserializer<C, T>) {
	const anch::codec::MediaType codecType = anch::codec::detail::codecMediaType(C::mediaType());
	if(codecType.getType() == contentType.getType() && codecType.getSubtype() == contentType.getSubtype()) {
	  anch::codec::deserialize<C>(value, input, contentType, side);
	  return true;
	}
      }
      return false;
    };
    return (tryCodec.template operator()<Cs>() || ...);
  }

  template<typename... Cs>
  template<typename T>
  std::optional<anch::codec::MediaType>
  Codecs<Cs...>::serialize(const T& value, std::ostream& out, const std::vector<anch::codec::MediaType>& accept,
			   anch::codec::Side side) {
    static_assert(SERIALIZES<T>, "No codec can serialize this type");
    std::vector<anch::codec::MediaType> available;
    auto addCodec = [&]<typename C>() {
      if constexpr (anch::codec::Serializer<C, T>) {
	available.push_back(anch::codec::detail::codecMediaType(C::mediaType()));
      }
    };
    (addCodec.template operator()<Cs>(), ...);
    const std::optional<std::size_t> selected = anch::codec::negotiate(accept, available);
    if(!selected) {
      return std::nullopt;
    }
    std::optional<anch::codec::MediaType> res;
    std::size_t index = 0;
    auto useCodec = [&]<typename C>() {
      if constexpr (anch::codec::Serializer<C, T>) {
	if(index++ == *selected) {
	  res = anch::codec::contentTypeOf<C>(value);
	  anch::codec::serialize<C>(value, out, *res, side);
	  return true;
	}
      }
      return false;
    };
    (useCodec.template operator()<Cs>() || ...);
    return res;
  }

}
