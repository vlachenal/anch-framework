/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "rest/status.hpp"

using anch::rest::Status;

std::string_view
anch::rest::reasonPhrase(Status status) noexcept {
  switch(status) {
  case Status::CONTINUE: return "Continue";
  case Status::SWITCHING_PROTOCOLS: return "Switching Protocols";
  case Status::PROCESSING: return "Processing";
  case Status::EARLY_HINTS: return "Early Hints";
  case Status::OK: return "OK";
  case Status::CREATED: return "Created";
  case Status::ACCEPTED: return "Accepted";
  case Status::NON_AUTHORITATIVE_INFORMATION: return "Non-Authoritative Information";
  case Status::NO_CONTENT: return "No Content";
  case Status::RESET_CONTENT: return "Reset Content";
  case Status::PARTIAL_CONTENT: return "Partial Content";
  case Status::MULTI_STATUS: return "Multi-Status";
  case Status::ALREADY_REPORTED: return "Already Reported";
  case Status::IM_USED: return "IM Used";
  case Status::MULTIPLE_CHOICES: return "Multiple Choices";
  case Status::MOVED_PERMANENTLY: return "Moved Permanently";
  case Status::FOUND: return "Found";
  case Status::SEE_OTHER: return "See Other";
  case Status::NOT_MODIFIED: return "Not Modified";
  case Status::TEMPORARY_REDIRECT: return "Temporary Redirect";
  case Status::PERMANENT_REDIRECT: return "Permanent Redirect";
  case Status::BAD_REQUEST: return "Bad Request";
  case Status::UNAUTHORIZED: return "Unauthorized";
  case Status::PAYMENT_REQUIRED: return "Payment Required";
  case Status::FORBIDDEN: return "Forbidden";
  case Status::NOT_FOUND: return "Not Found";
  case Status::METHOD_NOT_ALLOWED: return "Method Not Allowed";
  case Status::NOT_ACCEPTABLE: return "Not Acceptable";
  case Status::PROXY_AUTHENTICATION_REQUIRED: return "Proxy Authentication Required";
  case Status::REQUEST_TIMEOUT: return "Request Timeout";
  case Status::CONFLICT: return "Conflict";
  case Status::GONE: return "Gone";
  case Status::LENGTH_REQUIRED: return "Length Required";
  case Status::PRECONDITION_FAILED: return "Precondition Failed";
  case Status::CONTENT_TOO_LARGE: return "Content Too Large";
  case Status::URI_TOO_LONG: return "URI Too Long";
  case Status::UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
  case Status::RANGE_NOT_SATISFIABLE: return "Range Not Satisfiable";
  case Status::EXPECTATION_FAILED: return "Expectation Failed";
  case Status::IM_A_TEAPOT: return "I'm a teapot";
  case Status::MISDIRECTED_REQUEST: return "Misdirected Request";
  case Status::UNPROCESSABLE_CONTENT: return "Unprocessable Content";
  case Status::LOCKED: return "Locked";
  case Status::FAILED_DEPENDENCY: return "Failed Dependency";
  case Status::TOO_EARLY: return "Too Early";
  case Status::UPGRADE_REQUIRED: return "Upgrade Required";
  case Status::PRECONDITION_REQUIRED: return "Precondition Required";
  case Status::TOO_MANY_REQUESTS: return "Too Many Requests";
  case Status::REQUEST_HEADER_FIELDS_TOO_LARGE: return "Request Header Fields Too Large";
  case Status::UNAVAILABLE_FOR_LEGAL_REASONS: return "Unavailable For Legal Reasons";
  case Status::INTERNAL_SERVER_ERROR: return "Internal Server Error";
  case Status::NOT_IMPLEMENTED: return "Not Implemented";
  case Status::BAD_GATEWAY: return "Bad Gateway";
  case Status::SERVICE_UNAVAILABLE: return "Service Unavailable";
  case Status::GATEWAY_TIMEOUT: return "Gateway Timeout";
  case Status::HTTP_VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported";
  case Status::VARIANT_ALSO_NEGOTIATES: return "Variant Also Negotiates";
  case Status::INSUFFICIENT_STORAGE: return "Insufficient Storage";
  case Status::LOOP_DETECTED: return "Loop Detected";
  case Status::NETWORK_AUTHENTICATION_REQUIRED: return "Network Authentication Required";
  }
  return std::string_view();
}
