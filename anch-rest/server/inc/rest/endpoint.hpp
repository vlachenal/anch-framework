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

#include "anch-rest-server-export.hpp"

#include <cstddef>
#include <functional>
#include <istream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "rest/body.hpp"
#include "rest/codec.hpp"
#include "rest/mediaType.hpp"
#include "rest/method.hpp"
#include "rest/paramConverter.hpp"
#include "rest/request.hpp"
#include "rest/response.hpp"

/*! \cond INTERNAL */
namespace anch::rest::detail {

  /*!
   * Function signature traits (functions, function pointers and function objects)
   *
   * \tparam F the function type
   */
  template<typename F>
  struct FunctionTraits: FunctionTraits<decltype(&F::operator())> {};

  /*! Function signature traits: functions */
  template<typename R, typename... A>
  struct FunctionTraits<R(A...)> {
    /*! Result type */
    using Result = R;
    /*! Argument types */
    using Args = std::tuple<A...>;
  };

  /*! Function signature traits: noexcept functions */
  template<typename R, typename... A>
  struct FunctionTraits<R(A...) noexcept>: FunctionTraits<R(A...)> {};

  /*! Function signature traits: function pointers */
  template<typename R, typename... A>
  struct FunctionTraits<R(*)(A...)>: FunctionTraits<R(A...)> {};

  /*! Function signature traits: noexcept function pointers */
  template<typename R, typename... A>
  struct FunctionTraits<R(*)(A...) noexcept>: FunctionTraits<R(A...)> {};

  /*! Function signature traits: function objects call operator */
  template<typename C, typename R, typename... A>
  struct FunctionTraits<R(C::*)(A...)>: FunctionTraits<R(A...)> {};

  /*! Function signature traits: const function objects call operator */
  template<typename C, typename R, typename... A>
  struct FunctionTraits<R(C::*)(A...) const>: FunctionTraits<R(A...)> {};

  /*! Function signature traits: noexcept function objects call operator */
  template<typename C, typename R, typename... A>
  struct FunctionTraits<R(C::*)(A...) noexcept>: FunctionTraits<R(A...)> {};

  /*! Function signature traits: const noexcept function objects call operator */
  template<typename C, typename R, typename... A>
  struct FunctionTraits<R(C::*)(A...) const noexcept>: FunctionTraits<R(A...)> {};

  /*!
   * Tuple element, \c void when index is out of range
   *
   * \tparam I the index
   * \tparam T the tuple type
   */
  template<std::size_t I, typename T, bool = (I < std::tuple_size_v<T>)>
  struct ElementOr {
    /*! Element type */
    using Type = void;
  };

  /*! Tuple element in range */
  template<std::size_t I, typename T>
  struct ElementOr<I, T, true> {
    /*! Element type */
    using Type = std::tuple_element_t<I, T>;
  };

  /*! \ref anch::rest::Body detection */
  template<typename T>
  struct IsBody: std::false_type {
    /*! Body value type */
    using Type = std::monostate;
  };

  /*! \ref anch::rest::Body detection */
  template<typename T>
  struct IsBody<anch::rest::Body<T>>: std::true_type {
    /*! Body value type */
    using Type = T;
  };

  /*! \c std::optional detection */
  template<typename T>
  struct IsOptional: std::false_type {
    /*! Value type */
    using Type = T;
  };

  /*! \c std::optional detection */
  template<typename T>
  struct IsOptional<std::optional<T>>: std::true_type {
    /*! Value type */
    using Type = T;
  };

  /*!
   * Endpoint function signature: <tt>([const Request&], path parameters..., [Body<T>])</tt>
   *
   * \tparam F the function type
   */
  template<typename F>
  struct Signature {
    /*! Argument types */
    using Args = typename FunctionTraits<F>::Args;

    /*! Result type */
    using Result = std::remove_cvref_t<typename FunctionTraits<F>::Result>;

    /*! Number of arguments */
    static constexpr std::size_t COUNT = std::tuple_size_v<Args>;

    /*! First argument is the request */
    static constexpr bool HAS_REQUEST =
      std::is_same_v<std::remove_cvref_t<typename ElementOr<0, Args>::Type>, anch::rest::Request>;

    /*! Last argument type */
    using Last = std::remove_cvref_t<typename ElementOr<COUNT - 1, Args>::Type>;

    /*! Last argument is the body */
    static constexpr bool HAS_BODY = COUNT > 0 && IsBody<Last>::value;

    /*! Body value type (\c std::monostate when there is no body) */
    using BodyType = typename IsBody<Last>::Type;

    /*! Last argument is the raw body stream ( \c std::istream& , whatever the \c Content-Type is) */
    static constexpr bool RAW_BODY = COUNT > 0 && std::is_same_v<typename ElementOr<COUNT - 1, Args>::Type, std::istream&>;

    /*! Number of path parameters */
    static constexpr std::size_t PATH_COUNT = COUNT - (HAS_REQUEST ? 1 : 0) - ((HAS_BODY || RAW_BODY) ? 1 : 0);

    /*!
     * Path parameter type
     *
     * \tparam I the path parameter index
     */
    template<std::size_t I>
    using PathArg = std::remove_cvref_t<std::tuple_element_t<I + (HAS_REQUEST ? 1 : 0), Args>>;

    /*! Result is a value to serialize (not \c void nor \ref anch::rest::Response ) */
    static constexpr bool SERIALIZED = !std::is_void_v<Result> && !std::is_same_v<Result, anch::rest::Response>;

    /*! Result is an optional value (empty means \c 404 ) */
    static constexpr bool OPTIONAL = IsOptional<Result>::value;

    /*! Serialized value type (\c std::monostate when result is not serialized) */
    using Value = std::conditional_t<SERIALIZED, typename IsOptional<Result>::Type, std::monostate>;
  };

}
/*! \endcond */

namespace anch::rest {

  /*!
   * Route: endpoint registered in \ref Api for a method and a path pattern.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ANCH_REST_SERVER_EXPORT Route {

    // Attributes +
  protected:
    /*! Method */
    anch::rest::Method _method;

    /*! Path pattern */
    std::string _pattern;

    /*! Path parameter names, in pattern order */
    std::vector<std::string> _parameterNames;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Route constructor
     *
     * \param method the method
     * \param pattern the path pattern
     * \param parameterNames the path parameter names
     */
    Route(anch::rest::Method method, std::string pattern, std::vector<std::string> parameterNames);

    /*! Prohibit \ref Route copy */
    Route(const Route&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Route destructor
     */
    virtual ~Route();
    // Destructor -

    // Methods +
  public:
    /*!
     * Execute endpoint
     *
     * \param request the request (path parameters are set)
     * \param parameters the decoded path parameters, in pattern order
     *
     * \return the response
     *
     * \throw anch::rest::HttpException on request error (\c 400 , \c 406 , \c 415 ) or endpoint error
     */
    virtual anch::rest::Response handle(anch::rest::Request& request, std::vector<std::string>& parameters) = 0;

    /*!
     * Get the number of path parameters expected by endpoint
     *
     * \return the number of path parameters
     */
    virtual std::size_t getPathParameterCount() const noexcept = 0;

    /*!
     * Check endpoint configuration (codecs)
     *
     * \return the error message, empty when endpoint is valid
     */
    virtual std::optional<std::string> check() const = 0;

    /*! Prohibit \ref Route copy */
    Route& operator=(const Route&) = delete;
    // Methods -

    // Accessors +
  public:
    /*!
     * Method getter
     *
     * \return the method
     */
    anch::rest::Method getMethod() const noexcept;

    /*!
     * Path pattern getter
     *
     * \return the path pattern
     */
    const std::string& getPattern() const noexcept;

    /*!
     * Path parameter names getter
     *
     * \return the path parameter names
     */
    const std::vector<std::string>& getParameterNames() const noexcept;
    // Accessors -

  };

  /*!
   * Endpoint function bound to a route.\n
   * Function arguments are, in order:
   * - <tt>const anch::rest::Request&</tt> (optional)
   * - path parameters, in pattern order (types supported by \ref ParamConverter , \c 400 when invalid)
   * - \ref Body (optional): deserialized with the codec matching \c Content-Type (\c 415 when not
   *   consumed, \c 400 when missing or invalid)
   * - or <tt>std::istream&</tt> (optional): raw body stream, whatever the \c Content-Type is (absent
   *   included, no codec: read it with <tt>const Request&</tt> ), empty without body
   *
   * Function result:
   * - \ref Response : sent as is
   * - \c void : \c 204
   * - \c std::optional<T> : \c 404 when empty, \c T otherwise
   * - \c T : serialized with the codec negotiated with \c Accept (\c 406 when not acceptable)
   *
   * \tparam F the function type
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  template<typename F>
  class Endpoint: public anch::rest::Route {

    /*! Function signature */
    using Sig = anch::rest::detail::Signature<F>;

    /*! Serialized value type */
    using Value = typename Sig::Value;

    /*! Body value type */
    using BodyType = typename Sig::BodyType;

    /*!
     * Check path parameter types
     *
     * \return \c true when every path parameter type is supported
     */
    template<std::size_t... I>
    static consteval bool pathParametersSupported(std::index_sequence<I...>);

    static_assert(pathParametersSupported(std::make_index_sequence<Sig::PATH_COUNT>()),
		  "Endpoint arguments are ([const anch::rest::Request&], path parameters..., [anch::rest::Body<T>]) "
		  "and path parameters have to be supported by anch::rest::ParamConverter");
    static_assert(!Sig::HAS_BODY || std::is_default_constructible_v<BodyType>,
		  "Body type has to be default constructible");

    // Attributes +
  private:
    /*! Endpoint function */
    F _function;

    /*! Produced media types */
    std::vector<anch::rest::MediaType> _produced;

    /*! Result writer of a produced media type */
    struct Writer {
      /*! Get result \c Content-Type (codec \c contentType() or \c mediaType() ) */
      std::function<anch::rest::MediaType(const Value&)> contentType;

      /*! Serialize result with its \c Content-Type */
      std::function<void(const Value&, std::ostream&, const anch::rest::MediaType&)> write;
    };

    /*! Serializers (one per produced media type) */
    std::vector<Writer> _serializers;

    /*! Consumed media types */
    std::vector<anch::rest::MediaType> _consumed;

    /*! Deserializers (one per consumed media type) */
    std::vector<std::function<void(BodyType&, std::istream&, const anch::rest::MediaType&)>> _deserializers;

    /*! Success status of serialized results and of \c void endpoints (default: \c 200 and \c 204 ) */
    std::optional<anch::rest::Status> _status;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Endpoint constructor
     *
     * \param method the method
     * \param pattern the path pattern
     * \param parameterNames the path parameter names
     * \param function the endpoint function
     */
    Endpoint(anch::rest::Method method, std::string pattern, std::vector<std::string> parameterNames, F function);
    // Constructors -

    // Methods +
  public:
    /*!
     * Add a produced media type: result is serialized with codec \c C when it is negotiated
     *
     * \tparam C the codec (see \ref Serializer )
     *
     * \return \c this
     */
    template<typename C>
    Endpoint& produces();

    /*!
     * Add a consumed media type: body is deserialized with codec \c C when request \c Content-Type
     * is the codec media type (parameters such as \c charset are ignored)
     *
     * \tparam C the codec (see \ref Deserializer )
     *
     * \return \c this
     */
    template<typename C>
    Endpoint& consumes();

    /*!
     * Set success status: status of responses with serialized result ( \c 200 by default) or of \c void
     * endpoints ( \c 204 by default), such as \c 201 or \c 202 . Endpoints which return
     * \ref anch::rest::Response set their status themselves, and empty \c std::optional results are still
     * \c 404 .
     *
     * \param status the success status
     *
     * \return \c this
     *
     * \since 0.2
     */
    Endpoint& status(anch::rest::Status status);

    /*!
     * Execute endpoint
     *
     * \param request the request
     * \param parameters the decoded path parameters
     *
     * \return the response
     */
    virtual anch::rest::Response handle(anch::rest::Request& request, std::vector<std::string>& parameters) override;

    /*!
     * Get the number of path parameters expected by endpoint
     *
     * \return the number of path parameters
     */
    virtual std::size_t getPathParameterCount() const noexcept override;

    /*!
     * Check endpoint configuration: a serialized result needs a produced media type, a body needs a
     * consumed media type
     *
     * \return the error message, empty when endpoint is valid
     */
    virtual std::optional<std::string> check() const override;

  private:
    /*!
     * Convert path parameters and call endpoint function
     *
     * \param request the request
     * \param parameters the path parameters
     *
     * \return the function result
     */
    template<std::size_t... I>
    typename Sig::Result invoke(anch::rest::Request& request,
				std::vector<std::string>& parameters,
				std::index_sequence<I...>);

    /*!
     * Deserialize request body
     *
     * \param request the request
     *
     * \return the body
     */
    anch::rest::Body<BodyType> readBody(anch::rest::Request& request);

    /*!
     * Serialize result
     *
     * \param value the value
     * \param index the produced media type index
     *
     * \return the response
     */
    anch::rest::Response serialize(Value value, std::size_t index) const;
    // Methods -

  };

}

#include "rest/impl/endpoint.hpp"
