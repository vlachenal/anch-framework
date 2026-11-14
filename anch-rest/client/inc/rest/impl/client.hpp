/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2012 Vincent Lachenal

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
#include <optional>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace anch::rest {

  // Call +
  template<typename... Codecs>
  Call<Codecs...>::Call(anch::rest::Client& client, anch::rest::Request request):
    _client(&client),
    _request(std::move(request)) {
    // Nothing to do
  }

  template<typename... Codecs>
  Call<Codecs...>&
  Call<Codecs...>::header(std::string_view name, std::string_view value) & {
    _request.getHeaders().set(name, value);
    return *this;
  }

  template<typename... Codecs>
  Call<Codecs...>&&
  Call<Codecs...>::header(std::string_view name, std::string_view value) && {
    return std::move(header(name, value));
  }

  template<typename... Codecs>
  template<anch::rest::ParameterValue T>
  Call<Codecs...>&
  Call<Codecs...>::query(std::string_view name, const T& value) & {
    _request.addQueryParameter(name, anch::rest::toParameterString(value));
    return *this;
  }

  template<typename... Codecs>
  template<anch::rest::ParameterValue T>
  Call<Codecs...>&&
  Call<Codecs...>::query(std::string_view name, const T& value) && {
    return std::move(query(name, value));
  }

  template<typename... Codecs>
  template<typename C>
  Call<Codecs..., C>
  Call<Codecs...>::accept() && {
    _request.getHeaders().add(anch::rest::header::ACCEPT, C::mediaType());
    return Call<Codecs..., C>(*_client, std::move(_request));
  }

  template<typename... Codecs>
  template<typename C, typename T>
  Call<Codecs...>&
  Call<Codecs...>::body(const T& value) & {
    static_assert(anch::rest::Serializer<C, T>, "Codec can not serialize body");
    const anch::rest::MediaType contentType = anch::rest::contentTypeOf<C>(value);
    if constexpr (requires { { C::stream(value, contentType, anch::rest::Side::CLIENT) } -> std::same_as<anch::rest::BodyStream>; }) {
      // Body is read by the transport: large bodies are not built in memory
      anch::rest::BodyStream stream = C::stream(value, contentType, anch::rest::Side::CLIENT);
      _request.getHeaders().set(anch::rest::header::CONTENT_TYPE, contentType.toString());
      if(stream.size) {
	_request.getHeaders().set(anch::rest::header::CONTENT_LENGTH, std::to_string(*stream.size));
      }
      _request.setBody(std::move(stream.input));
      return *this;
    } else {
      std::ostringstream out;
      anch::rest::serializeBody<C>(value, out, contentType, anch::rest::Side::CLIENT);
      return body(out.str(), contentType.toString());
    }
  }

  template<typename... Codecs>
  template<typename C, typename T>
  Call<Codecs...>&&
  Call<Codecs...>::body(const T& value) && {
    return std::move(body<C, T>(value));
  }

  template<typename... Codecs>
  Call<Codecs...>&
  Call<Codecs...>::body(std::string content, std::string_view contentType) & {
    _request.getHeaders().set(anch::rest::header::CONTENT_TYPE, contentType);
    _request.setBody(std::move(content));
    return *this;
  }

  template<typename... Codecs>
  Call<Codecs...>&&
  Call<Codecs...>::body(std::string content, std::string_view contentType) && {
    return std::move(body(std::move(content), contentType));
  }

  template<typename... Codecs>
  Call<Codecs...>&
  Call<Codecs...>::body(std::unique_ptr<std::istream> input, std::string_view contentType, std::optional<std::uint64_t> size) & {
    _request.getHeaders().set(anch::rest::header::CONTENT_TYPE, contentType);
    if(size) {
      _request.getHeaders().set(anch::rest::header::CONTENT_LENGTH, std::to_string(*size));
    } else {
      _request.getHeaders().remove(anch::rest::header::CONTENT_LENGTH);
    }
    _request.setBody(std::move(input));
    return *this;
  }

  template<typename... Codecs>
  Call<Codecs...>&&
  Call<Codecs...>::body(std::unique_ptr<std::istream> input, std::string_view contentType, std::optional<std::uint64_t> size) && {
    return std::move(body(std::move(input), contentType, size));
  }

  template<typename... Codecs>
  Call<Codecs...>&
  Call<Codecs...>::body(anch::rest::RawBody raw) & {
    return body(std::move(raw.input), raw.contentType, raw.size);
  }

  template<typename... Codecs>
  Call<Codecs...>&&
  Call<Codecs...>::body(anch::rest::RawBody raw) && {
    return std::move(body(std::move(raw)));
  }

  template<typename... Codecs>
  anch::rest::Response
  Call<Codecs...>::send() && {
    return _client->send(_request);
  }

  template<typename... Codecs>
  void
  Call<Codecs...>::execute() && {
    anch::rest::Response response = std::move(*this).send();
    if(anch::rest::code(response.getStatus()) >= 400) {
      throw anch::rest::HttpError(std::move(response));
    }
  }

  template<typename... Codecs>
  template<typename T>
  T
  Call<Codecs...>::as() && {
    static_assert(sizeof...(Codecs) > 0, "as<T>() needs an accepted codec (accept<Codec>())");
    static_assert((anch::rest::Deserializer<Codecs, T> || ...), "No accepted codec can deserialize this type");
    static_assert(std::is_default_constructible_v<T>, "Result type has to be default constructible");
    anch::rest::Response response = std::move(*this).send();
    if(anch::rest::code(response.getStatus()) >= 400) {
      throw anch::rest::HttpError(std::move(response));
    }
    std::optional<std::string_view> header = response.getHeaders().get(anch::rest::header::CONTENT_TYPE);
    std::optional<anch::rest::MediaType> type = anch::rest::MediaType::parse(header.value_or(""));
    std::optional<T> result;
    auto tryCodec = [&]<typename C>() {
      if constexpr (anch::rest::Deserializer<C, T>) {
	std::optional<anch::rest::MediaType> codecType = anch::rest::MediaType::parse(C::mediaType());
	if(type && codecType && type->getType() == codecType->getType() && type->getSubtype() == codecType->getSubtype()) {
	  T value{};
	  std::istringstream input(response.content());
	  anch::rest::deserializeBody<C>(value, input, *type, anch::rest::Side::CLIENT);
	  result = std::move(value);
	  return true;
	}
      }
      return false;
    };
    (tryCodec.template operator()<Codecs>() || ...);
    if(!result) {
      throw std::runtime_error("Response content type '" + std::string(header.value_or("")) + "' is not accepted");
    }
    return std::move(*result);
  }

  template<typename... Codecs>
  anch::rest::Request&
  Call<Codecs...>::getRequest() noexcept {
    return _request;
  }
  // Call -

  // Client +
  template<anch::rest::ParameterValue... P>
  anch::rest::Call<>
  Client::request(anch::rest::Method method, std::string_view path, const P&... parameters) {
    std::vector<std::string> values{anch::rest::toParameterString(parameters)...};
    return anch::rest::Call<>(*this, anch::rest::Request(method, formatPath(path, values)));
  }

  template<anch::rest::ParameterValue... P>
  anch::rest::Call<>
  Client::get(std::string_view path, const P&... parameters) {
    return request(anch::rest::Method::GET, path, parameters...);
  }

  template<anch::rest::ParameterValue... P>
  anch::rest::Call<>
  Client::post(std::string_view path, const P&... parameters) {
    return request(anch::rest::Method::POST, path, parameters...);
  }

  template<anch::rest::ParameterValue... P>
  anch::rest::Call<>
  Client::put(std::string_view path, const P&... parameters) {
    return request(anch::rest::Method::PUT, path, parameters...);
  }

  template<anch::rest::ParameterValue... P>
  anch::rest::Call<>
  Client::patch(std::string_view path, const P&... parameters) {
    return request(anch::rest::Method::PATCH, path, parameters...);
  }

  template<anch::rest::ParameterValue... P>
  anch::rest::Call<>
  Client::del(std::string_view path, const P&... parameters) {
    return request(anch::rest::Method::DELETE, path, parameters...);
  }

  template<anch::rest::ParameterValue... P>
  anch::rest::Call<>
  Client::head(std::string_view path, const P&... parameters) {
    return request(anch::rest::Method::HEAD, path, parameters...);
  }
  // Client -

}
