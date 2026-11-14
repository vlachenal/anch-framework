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

#include <exception>
#include <memory>
#include <stdexcept>

#include "rest/httpException.hpp"

namespace anch::rest::detail {

  /*!
   * Convert path parameter
   *
   * \tparam T the parameter type
   *
   * \param value the parameter value
   *
   * \return the converted value
   *
   * \throw anch::rest::HttpException \c 400 when value is invalid
   */
  template<typename T>
  T
  convertPathParameter(const std::string& value) {
    std::optional<T> res = anch::rest::ParamConverter<T>::convert(value);
    if(!res) {
      throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, "Invalid path parameter '" + value + "'");
    }
    return std::move(*res);
  }

}

namespace anch::rest {

  template<typename F>
  template<std::size_t... I>
  consteval bool
  Endpoint<F>::pathParametersSupported(std::index_sequence<I...>) {
    return (anch::rest::PathParameter<typename Sig::template PathArg<I>> && ...);
  }

  template<typename F>
  Endpoint<F>::Endpoint(anch::rest::Method method,
			std::string pattern,
			std::vector<std::string> parameterNames,
			F function):
    anch::rest::Route(method, std::move(pattern), std::move(parameterNames)),
    _function(std::move(function)),
    _produced(),
    _serializers(),
    _consumed(),
    _deserializers(),
    _status() {
    // Nothing to do
  }

  template<typename F>
  Endpoint<F>&
  Endpoint<F>::status(anch::rest::Status status) {
    static_assert(Sig::SERIALIZED || std::is_void_v<typename Sig::Result>,
		  "status() needs an endpoint which returns a value or void (anch::rest::Response sets its status)");
    _status = status;
    return *this;
  }

  template<typename F>
  template<typename C>
  Endpoint<F>&
  Endpoint<F>::produces() {
    static_assert(Sig::SERIALIZED, "produces() needs an endpoint which returns a value (not void nor anch::rest::Response)");
    static_assert(anch::rest::Serializer<C, Value>, "Codec can not serialize endpoint result");
    _produced.push_back(anch::rest::detail::codecMediaType(C::mediaType()));
    _serializers.push_back(Writer{
	[](const Value& value) {
	  return anch::rest::contentTypeOf<C>(value);
	},
	[](const Value& value, std::ostream& out, const anch::rest::MediaType& contentType) {
	  anch::rest::serializeBody<C>(value, out, contentType, anch::rest::Side::SERVER);
	}
      });
    return *this;
  }

  template<typename F>
  template<typename C>
  Endpoint<F>&
  Endpoint<F>::consumes() {
    static_assert(Sig::HAS_BODY, "consumes() needs an endpoint with an anch::rest::Body<T> argument");
    static_assert(anch::rest::Deserializer<C, BodyType>, "Codec can not deserialize endpoint body");
    _consumed.push_back(anch::rest::detail::codecMediaType(C::mediaType()));
    _deserializers.emplace_back([](BodyType& value, std::istream& input, const anch::rest::MediaType& contentType) {
      anch::rest::deserializeBody<C>(value, input, contentType, anch::rest::Side::SERVER);
    });
    return *this;
  }

  template<typename F>
  anch::rest::Response
  Endpoint<F>::handle(anch::rest::Request& request, std::vector<std::string>& parameters) {
    // Negotiate response media type before endpoint execution
    std::size_t produced = 0;
    if constexpr (Sig::SERIALIZED) {
      std::optional<std::size_t> selected = anch::rest::negotiate(request.getAccept(), _produced);
      if(!selected) {
	throw anch::rest::HttpException(anch::rest::Status::NOT_ACCEPTABLE);
      }
      produced = *selected;
    }
    auto sequence = std::make_index_sequence<Sig::PATH_COUNT>();
    if constexpr (std::is_void_v<typename Sig::Result>) {
      invoke(request, parameters, sequence);
      return anch::rest::Response(_status.value_or(anch::rest::Status::NO_CONTENT));
    } else if constexpr (!Sig::SERIALIZED) {
      return invoke(request, parameters, sequence);
    } else if constexpr (Sig::OPTIONAL) {
      typename Sig::Result result = invoke(request, parameters, sequence);
      if(!result) {
	return anch::rest::Response(anch::rest::Status::NOT_FOUND);
      }
      return serialize(std::move(*result), produced);
    } else {
      return serialize(invoke(request, parameters, sequence), produced);
    }
  }

  template<typename F>
  template<std::size_t... I>
  typename Endpoint<F>::Sig::Result
  Endpoint<F>::invoke(anch::rest::Request& request,
		      [[maybe_unused]] std::vector<std::string>& parameters,
		      std::index_sequence<I...>) {
    // Braced initialization: path parameters are converted in order
    std::tuple<typename Sig::template PathArg<I>...> values{
      anch::rest::detail::convertPathParameter<typename Sig::template PathArg<I>>(parameters[I])...
    };
    auto prefix = [&request]() {
      if constexpr (Sig::HAS_REQUEST) {
	return std::forward_as_tuple(request);
      } else {
	return std::tuple<>();
      }
    }();
    if constexpr (Sig::HAS_BODY) {
      return std::apply(_function, std::tuple_cat(prefix, std::move(values), std::make_tuple(readBody(request))));
    } else {
      return std::apply(_function, std::tuple_cat(prefix, std::move(values)));
    }
  }

  template<typename F>
  anch::rest::Body<typename Endpoint<F>::BodyType>
  Endpoint<F>::readBody(anch::rest::Request& request) {
    std::optional<anch::rest::MediaType> contentType = request.getContentType();
    if(!contentType) {
      throw anch::rest::HttpException(anch::rest::Status::UNSUPPORTED_MEDIA_TYPE);
    }
    for(std::size_t i = 0 ; i < _consumed.size() ; ++i) {
      if(_consumed[i].getType() != contentType->getType() || _consumed[i].getSubtype() != contentType->getSubtype()) {
	continue;
      }
      if(!request.hasBody()) {
	throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, "Missing request body");
      }
      BodyType value{};
      try {
	_deserializers[i](value, request.getBody(), *contentType);
      } catch(const std::exception& e) {
	throw anch::rest::HttpException(anch::rest::Status::BAD_REQUEST, e.what());
      }
      return anch::rest::Body<BodyType>(std::move(value));
    }
    throw anch::rest::HttpException(anch::rest::Status::UNSUPPORTED_MEDIA_TYPE);
  }

  template<typename F>
  anch::rest::Response
  Endpoint<F>::serialize(Value value, std::size_t index) const {
    // Body writer has to be copyable: value is shared
    // Content type is computed before writing: headers are sent before the body is streamed
    auto shared = std::make_shared<const Value>(std::move(value));
    auto contentType = std::make_shared<const anch::rest::MediaType>(_serializers[index].contentType(*shared));
    std::function<void(const Value&, std::ostream&, const anch::rest::MediaType&)> write = _serializers[index].write;
    return anch::rest::Response(_status.value_or(anch::rest::Status::OK))
      .contentType(contentType->toString())
      .stream([shared, contentType, write](std::ostream& out) {
	write(*shared, out, *contentType);
      });
  }

  template<typename F>
  std::size_t
  Endpoint<F>::getPathParameterCount() const noexcept {
    return Sig::PATH_COUNT;
  }

  template<typename F>
  std::optional<std::string>
  Endpoint<F>::check() const {
    if(Sig::SERIALIZED && _produced.empty()) {
      return std::string("no produced media type (produces<Codec>())");
    }
    if(Sig::HAS_BODY && _consumed.empty()) {
      return std::string("no consumed media type (consumes<Codec>())");
    }
    return std::nullopt;
  }

}
