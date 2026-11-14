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
#include "rest/api.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>

#include "rest/httpException.hpp"

using anch::rest::Api;
using anch::rest::CorsOptions;
using anch::rest::Filter;
using anch::rest::HttpException;
using anch::rest::Method;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Route;
using anch::rest::Status;

// Routing tree node +
struct Api::Node {
  /*! Literal segment children */
  std::map<std::string, std::unique_ptr<Node>, std::less<>> literals;

  /*! Parameter segment child */
  std::unique_ptr<Node> parameter;

  /*! Routes ending on this node by method */
  std::map<Method, std::unique_ptr<Route>> routes;
};
// Routing tree node -

// Match +
struct Api::Match {
  /*! Matching node */
  Node* node;

  /*! Path parameter values, in pattern order */
  std::vector<std::string> parameters;
};
// Match -

namespace {

  /*! Split path on '/', empty segments are ignored */
  std::vector<std::string_view>
  splitPath(std::string_view path) {
    std::vector<std::string_view> segments;
    while(!path.empty()) {
      const std::size_t slash = path.find('/');
      std::string_view segment = path.substr(0, slash);
      if(!segment.empty()) {
	segments.push_back(segment);
      }
      path = (slash == std::string_view::npos) ? std::string_view() : path.substr(slash + 1);
    }
    return segments;
  }

  bool
  isParameter(std::string_view segment) noexcept {
    return segment.size() >= 2 && segment.front() == '{' && segment.back() == '}';
  }

  /*! Join methods with ", " */
  std::string
  formatMethods(const std::set<Method>& methods) {
    std::string res;
    for(Method method : methods) {
      if(!res.empty()) {
	res += ", ";
      }
      res += anch::rest::toString(method);
    }
    return res;
  }

  /*! Join values with ", " */
  std::string
  join(const std::vector<std::string>& values) {
    std::string res;
    for(const std::string& value : values) {
      if(!res.empty()) {
	res += ", ";
      }
      res += value;
    }
    return res;
  }

  /*! Compare strings ignoring case */
  bool
  equalsIgnoreCase(std::string_view left, std::string_view right) noexcept {
    const anch::rest::CaseInsensitiveLess less;
    return !less(left, right) && !less(right, left);
  }

  /*! Check if list contains "*" */
  bool
  hasWildcard(const std::vector<std::string>& values) noexcept {
    return std::find(values.begin(), values.end(), "*") != values.end();
  }

  /*! Check if origin is allowed */
  bool
  isAllowedOrigin(const CorsOptions& options, std::string_view origin) noexcept {
    return hasWildcard(options.origins)
      || std::find(options.origins.begin(), options.origins.end(), origin) != options.origins.end();
  }

  /*! Split comma separated header names, whitespaces are trimmed and empty names are ignored */
  std::vector<std::string_view>
  splitNames(std::string_view value) {
    std::vector<std::string_view> names;
    while(!value.empty()) {
      const std::size_t comma = value.find(',');
      std::string_view name = value.substr(0, comma);
      value = (comma == std::string_view::npos) ? std::string_view() : value.substr(comma + 1);
      const std::size_t first = name.find_first_not_of(" \t");
      if(first == std::string_view::npos) {
	continue;
      }
      name = name.substr(first, name.find_last_not_of(" \t") - first + 1);
      names.push_back(name);
    }
    return names;
  }

}

// Constructors +
Api::Api(): _root(std::make_unique<Node>()), _filters(), _cors() {
  // Nothing to do
}

Api::Api(Api&& other) noexcept: _root(std::move(other._root)), _filters(std::move(other._filters)),
				 _cors(std::move(other._cors)) {
  // Nothing to do
}
// Constructors -

// Destructor +
Api::~Api() {
  // Nothing to do
}
// Destructor -

// Methods +
Api&
Api::operator=(Api&& other) noexcept {
  _root = std::move(other._root);
  _filters = std::move(other._filters);
  _cors = std::move(other._cors);
  return *this;
}

std::vector<std::string>
Api::parsePattern(std::string_view pattern) {
  if(pattern.empty() || pattern.front() != '/') {
    throw std::invalid_argument("Invalid path pattern '" + std::string(pattern) + "': it has to start with '/'");
  }
  std::vector<std::string> names;
  for(std::string_view segment : splitPath(pattern)) {
    if(isParameter(segment)) {
      std::string name(segment.substr(1, segment.size() - 2));
      if(name.empty() || name.find_first_of("{}") != std::string::npos) {
	throw std::invalid_argument("Invalid path pattern '" + std::string(pattern) + "': invalid parameter name");
      }
      if(std::find(names.begin(), names.end(), name) != names.end()) {
	throw std::invalid_argument("Invalid path pattern '" + std::string(pattern) + "': duplicate parameter '" + name + "'");
      }
      names.push_back(std::move(name));
    } else if(segment.find_first_of("{}") != std::string_view::npos) {
      throw std::invalid_argument("Invalid path pattern '" + std::string(pattern) + "': a parameter has to be a whole segment");
    }
  }
  return names;
}

void
Api::addRoute(std::unique_ptr<Route> route) {
  const std::string description = std::string(anch::rest::toString(route->getMethod())) + ' ' + route->getPattern();
  if(route->getPathParameterCount() != route->getParameterNames().size()) {
    throw std::invalid_argument(description + ": pattern has " + std::to_string(route->getParameterNames().size())
				+ " path parameter(s), endpoint function has " + std::to_string(route->getPathParameterCount()));
  }
  Node* node = _root.get();
  for(std::string_view segment : splitPath(route->getPattern())) {
    std::unique_ptr<Node>* child = nullptr;
    if(isParameter(segment)) {
      child = &node->parameter;
    } else {
      auto iter = node->literals.find(segment);
      if(iter == node->literals.end()) {
	iter = node->literals.emplace(std::string(segment), nullptr).first;
      }
      child = &iter->second;
    }
    if(!*child) {
      *child = std::make_unique<Node>();
    }
    node = child->get();
  }
  if(node->routes.contains(route->getMethod())) {
    throw std::invalid_argument(description + ": route already exists");
  }
  node->routes.emplace(route->getMethod(), std::move(route));
}

Api&
Api::filter(Filter filter) {
  _filters.push_back(std::move(filter));
  return *this;
}

Api&
Api::cors(CorsOptions options) {
  if(options.origins.empty()) {
    throw std::invalid_argument("CORS: at least one origin is required");
  }
  if(options.credentials && hasWildcard(options.origins)) {
    throw std::invalid_argument("CORS: credentials can not be allowed for every origin ('*')");
  }
  _cors = std::move(options);
  return *this;
}

void
Api::validate() const {
  std::string errors;
  std::vector<const Node*> nodes{_root.get()};
  while(!nodes.empty()) {
    const Node* node = nodes.back();
    nodes.pop_back();
    for(const auto& [method, route] : node->routes) {
      std::optional<std::string> error = route->check();
      if(error) {
	errors += std::string(anch::rest::toString(method)) + ' ' + route->getPattern() + ": " + *error + '\n';
      }
    }
    for(const auto& [segment, child] : node->literals) {
      nodes.push_back(child.get());
    }
    if(node->parameter) {
      nodes.push_back(node->parameter.get());
    }
  }
  if(!errors.empty()) {
    throw std::logic_error("Invalid endpoints:\n" + errors);
  }
}

Response
Api::handle(Request& request) noexcept {
  try {
    if(_cors && request.getMethod() == Method::OPTIONS
       && request.getHeaders().has(anch::rest::header::ORIGIN)
       && request.getHeaders().has(anch::rest::header::ACCESS_CONTROL_REQUEST_METHOD)) {
      return preflight(request);
    }
    Response response = next(request, 0);
    addCorsHeaders(request, response);
    return response;
  } catch(const HttpException& e) {
    return error(request, e.getStatus(), e.what());
  } catch(...) {
    return error(request, Status::INTERNAL_SERVER_ERROR, "");
  }
}

Response
Api::error(const Request& request, Status status, const char* message) const noexcept {
  try {
    Response response(status);
    if(*message != '\0') {
      response.contentType(anch::rest::Text::mediaType()).body(message);
    }
    addCorsHeaders(request, response);
    return response;
  } catch(...) {
    return Response(status);
  }
}

void
Api::addCorsHeaders(const Request& request, Response& response) const {
  if(!_cors) {
    return;
  }
  if(!hasWildcard(_cors->origins)) {
    // Response depends on origin
    response.getHeaders().add(anch::rest::header::VARY, anch::rest::header::ORIGIN);
  }
  std::optional<std::string_view> origin = request.getHeaders().get(anch::rest::header::ORIGIN);
  if(!origin || !isAllowedOrigin(*_cors, *origin)) {
    return;
  }
  response.header(anch::rest::header::ACCESS_CONTROL_ALLOW_ORIGIN, hasWildcard(_cors->origins) ? std::string_view("*") : *origin);
  if(_cors->credentials) {
    response.header(anch::rest::header::ACCESS_CONTROL_ALLOW_CREDENTIALS, "true");
  }
  if(!_cors->exposedHeaders.empty()) {
    response.header(anch::rest::header::ACCESS_CONTROL_EXPOSE_HEADERS, join(_cors->exposedHeaders));
  }
}

Response
Api::preflight(const Request& request) const {
  Response response(Status::NO_CONTENT);
  response.header(anch::rest::header::VARY, "Origin, Access-Control-Request-Method, Access-Control-Request-Headers");
  const Response forbidden = Response(Status::FORBIDDEN).header(anch::rest::header::VARY, "Origin, Access-Control-Request-Method, Access-Control-Request-Headers");

  // Origin
  const std::string_view origin = *request.getHeaders().get(anch::rest::header::ORIGIN);
  if(!isAllowedOrigin(*_cors, origin)) {
    return forbidden;
  }

  // Method
  std::vector<Match> matches = match(request);
  if(matches.empty()) {
    throw HttpException(Status::NOT_FOUND);
  }
  const std::set<Method> allowed = allowedMethods(matches);
  std::optional<Method> method = anch::rest::parseMethod(*request.getHeaders().get(anch::rest::header::ACCESS_CONTROL_REQUEST_METHOD));
  if(!method || !allowed.contains(*method)) {
    return forbidden;
  }

  // Headers
  const std::string requested = request.getHeaders().format(anch::rest::header::ACCESS_CONTROL_REQUEST_HEADERS).value_or("");
  const std::vector<std::string_view> names = splitNames(requested);
  if(!hasWildcard(_cors->headers)) {
    for(std::string_view name : names) {
      auto found = std::find_if(_cors->headers.begin(), _cors->headers.end(), [name](const std::string& header) {
	return equalsIgnoreCase(header, name);
      });
      if(found == _cors->headers.end()) {
	return forbidden;
      }
    }
  }

  response.header(anch::rest::header::ACCESS_CONTROL_ALLOW_ORIGIN, hasWildcard(_cors->origins) ? std::string_view("*") : origin);
  response.header(anch::rest::header::ACCESS_CONTROL_ALLOW_METHODS, formatMethods(allowed));
  if(!names.empty()) {
    std::string headers;
    for(std::string_view name : names) {
      if(!headers.empty()) {
	headers += ", ";
      }
      headers += name;
    }
    response.header(anch::rest::header::ACCESS_CONTROL_ALLOW_HEADERS, headers);
  }
  if(_cors->credentials) {
    response.header(anch::rest::header::ACCESS_CONTROL_ALLOW_CREDENTIALS, "true");
  }
  if(_cors->maxAge) {
    response.header(anch::rest::header::ACCESS_CONTROL_MAX_AGE, std::to_string(_cors->maxAge->count()));
  }
  return response;
}

Response
Api::next(Request& request, std::size_t index) {
  if(index == _filters.size()) {
    return dispatch(request);
  }
  return _filters[index](request, [this, index](Request& req) {
    return next(req, index + 1);
  });
}

std::vector<Api::Match>
Api::match(const Request& request) const {
  // Decode path segments
  std::vector<std::string> segments;
  for(std::string_view segment : splitPath(request.getPath())) {
    std::optional<std::string> decoded = anch::rest::percentDecode(segment);
    if(!decoded) {
      throw HttpException(Status::BAD_REQUEST, "Invalid path encoding");
    }
    segments.push_back(std::move(*decoded));
  }

  // Find matching nodes: literal segments first, then parameters (depth first)
  std::vector<Match> matches;
  std::vector<std::string> parameters;
  std::function<void(Node*, std::size_t)> explore = [&](Node* node, std::size_t depth) {
    if(depth == segments.size()) {
      if(!node->routes.empty()) {
	matches.push_back(Match{node, parameters});
      }
      return;
    }
    auto literal = node->literals.find(segments[depth]);
    if(literal != node->literals.end()) {
      explore(literal->second.get(), depth + 1);
    }
    if(node->parameter) {
      parameters.push_back(segments[depth]);
      explore(node->parameter.get(), depth + 1);
      parameters.pop_back();
    }
  };
  explore(_root.get(), 0);
  return matches;
}

std::set<Method>
Api::allowedMethods(const std::vector<Match>& matches) {
  std::set<Method> allowed{Method::OPTIONS};
  for(const Match& match : matches) {
    for(const auto& [method, route] : match.node->routes) {
      allowed.insert(method);
      if(method == Method::GET) {
	allowed.insert(Method::HEAD);
      }
    }
  }
  return allowed;
}

Response
Api::dispatch(Request& request) {
  const Method method = request.getMethod();
  if(method == Method::OPTIONS && request.getPath() == "*") {
    return Response(Status::NO_CONTENT);
  }
  std::vector<Match> matches = match(request);
  if(matches.empty()) {
    throw HttpException(Status::NOT_FOUND);
  }

  // Select route for method (HEAD falls back to GET)
  for(const bool fallback : {false, true}) {
    if(fallback && method != Method::HEAD) {
      break;
    }
    const Method wanted = fallback ? Method::GET : method;
    for(Match& match : matches) {
      auto iter = match.node->routes.find(wanted);
      if(iter == match.node->routes.end()) {
	continue;
      }
      Route& route = *iter->second;
      std::map<std::string, std::string, std::less<>> named;
      for(std::size_t i = 0 ; i < match.parameters.size() ; ++i) {
	named.emplace(route.getParameterNames()[i], match.parameters[i]);
      }
      request.setPathParameters(std::move(named));
      Response response = route.handle(request, match.parameters);
      if(fallback) {
	response.clearBody();
      }
      return response;
    }
  }

  // Path exists for other methods (OPTIONS without endpoint is answered with Allow)
  const std::string allow = formatMethods(allowedMethods(matches));
  if(method == Method::OPTIONS) {
    return Response(Status::NO_CONTENT).header(anch::rest::header::ALLOW, allow);
  }
  return Response(Status::METHOD_NOT_ALLOWED).header(anch::rest::header::ALLOW, allow);
}
// Methods -
