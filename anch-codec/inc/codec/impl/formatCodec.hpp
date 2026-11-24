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

  template<anch::codec::DataFormat F>
  std::string_view
  FormatCodec<F>::mediaType() noexcept {
    return F::MEDIA_TYPE;
  }

  template<anch::codec::DataFormat F>
  typename FormatCodec<F>::Options&
  FormatCodec<F>::options(anch::codec::Side side) {
    static Options server = []() {
      Options options = F::defaultOptions();
      anch::codec::FormatPolicy<F>::configure(options, anch::codec::Side::SERVER);
      return options;
    }();
    static Options client = []() {
      Options options = F::defaultOptions();
      anch::codec::FormatPolicy<F>::configure(options, anch::codec::Side::CLIENT);
      return options;
    }();
    return (side == anch::codec::Side::SERVER) ? server : client;
  }

  template<anch::codec::DataFormat F>
  template<typename T>
  requires anch::codec::FormatSerializer<F, T>
  void
  FormatCodec<F>::serialize(const T& value, std::ostream& out) {
    F::serialize(value, out, F::defaultOptions());
  }

  template<anch::codec::DataFormat F>
  template<typename T>
  requires anch::codec::FormatSerializer<F, T>
  void
  FormatCodec<F>::serialize(const T& value, std::ostream& out, anch::codec::Side side) {
    F::serialize(value, out, FormatCodec<F>::options(side));
  }

  template<anch::codec::DataFormat F>
  template<typename T>
  void
  FormatCodec<F>::read(T& value, std::istream& input, const Options& options) {
    try {
      F::deserialize(value, input, options);
    } catch(const std::exception& error) {
      anch::codec::FormatPolicy<F>::translate(error);
      throw;
    }
  }

  template<anch::codec::DataFormat F>
  template<typename T>
  requires anch::codec::FormatDeserializer<F, T>
  void
  FormatCodec<F>::deserialize(T& value, std::istream& input) {
    FormatCodec<F>::read(value, input, F::defaultOptions());
  }

  template<anch::codec::DataFormat F>
  template<typename T>
  requires anch::codec::FormatDeserializer<F, T>
  void
  FormatCodec<F>::deserialize(T& value, std::istream& input, anch::codec::Side side) {
    FormatCodec<F>::read(value, input, FormatCodec<F>::options(side));
  }

}
