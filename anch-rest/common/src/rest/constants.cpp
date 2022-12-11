/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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
#include "rest/constants.hpp"

using anch::rest::Verb;
using anch::rest::MediaType;
using anch::rest::CommonHeaders;


// Verbs +
const std::string Verb::GET("GET");
const std::string Verb::POST("POST");
const std::string Verb::PUT("PUT");
const std::string Verb::PATCH("PATCH");
const std::string Verb::DELETE("DELETE");
const std::string Verb::OPTIONS("OPTIONS");
const std::string Verb::TRACE("TRACE");
const std::string Verb::HEAD("HEAD");
const std::string Verb::CONNECT("CONNECT");
const std::string Verb::COPY("COPY");
const std::string Verb::LOCK("LOCK");
const std::string Verb::MKCOL("MKCOL");
const std::string Verb::MOVE("MOVE");
const std::string Verb::PROPFIND("PROPFIND");
const std::string Verb::PROPPATCH("PROPPATCH");
const std::string Verb::UNLOCK("UNLOCK");
// Verbs -

// Media types +
const std::string MediaType::NONE("");
const std::string MediaType::ALL("*/*");
const std::string MediaType::APP_GRAPHQL("application/graphql");
const std::string MediaType::APP_JAVASCRIPT("application/javascript");
const std::string MediaType::APP_JSON("application/json");
const std::string MediaType::APP_OCTET_STREAM("application/octet-stream");
const std::string MediaType::APP_VND_PROTOBUF("application/vnd.google.protobuf");
const std::string MediaType::APP_VND_THRIFT_BINARY("application/vnd.apache.thrift.binary");
const std::string MediaType::APP_VND_THRIFT_COMPACT("application/vnd.apache.thrift.compact");
const std::string MediaType::APP_VND_THRIFT_JSON("application/vnd.apache.thrift.json");
const std::string MediaType::APP_X_PROTOBUF("application/x-protobuf");
const std::string MediaType::APP_X_WWW_FORM_URLENCODED("application/x-www-form-urlencoded");
const std::string MediaType::APP_XML("application/xml");
const std::string MediaType::MULTIPART_FORM_DATA("multipart/form-data");
const std::string MediaType::TXT_CSS("text/css");
const std::string MediaType::TXT_CSV("text/csv");
const std::string MediaType::TXT_HTML("text/html");
const std::string MediaType::TXT_PLAIN("text/plain");
const std::string MediaType::TXT_XML("text/xml");
// Media types -

// Common headers +
// Standard commons +
const std::string CommonHeaders::CACHE_CONTROL("cache-control");
const std::string CommonHeaders::CONNECTION("connection");
const std::string CommonHeaders::CONTENT_ENCODING("content-encoding");
const std::string CommonHeaders::CONTENT_LENGTH("content-length");
const std::string CommonHeaders::CONTENT_MD5("content-md5");
const std::string CommonHeaders::CONTENT_TYPE("content-type");
const std::string CommonHeaders::DATE("date");
const std::string CommonHeaders::PRAGMA("pragma");
const std::string CommonHeaders::TRAILER("trailer");
const std::string CommonHeaders::TRANSFER_ENCODING("transfer-encoding");
const std::string CommonHeaders::UPGRADE("upgrade");
const std::string CommonHeaders::VIA("via");
const std::string CommonHeaders::WARNING("warning");
// Standard commons -

// Commons +
const std::string CommonHeaders::X_REQUEST_ID("x-request-id");
const std::string CommonHeaders::X_CORRELATION_ID("x-correlation-id");
// Commons -

// Standard request +
const std::string CommonHeaders::A_IM("a-im");
const std::string CommonHeaders::ACCEPT("accept");
const std::string CommonHeaders::ACCEPT_CHARSET("accept-charset");
const std::string CommonHeaders::ACCEPT_DATETIME("accept-datetime");
const std::string CommonHeaders::ACCEPT_LANGUAGE("accept-language");
const std::string CommonHeaders::ACCESS_CONTROL_REQUEST_METHOD("access-control-request-method");
const std::string CommonHeaders::ACCESS_CONTROL_REQUEST_HEADERS("access-control-request-headers");
const std::string CommonHeaders::AUTHORIZATION("authorization");
const std::string CommonHeaders::COOKIE("cookie");
const std::string CommonHeaders::EXPECT("expect");
const std::string CommonHeaders::FORWARDED("forwarded");
const std::string CommonHeaders::FROM("from");
const std::string CommonHeaders::HOST("host");
const std::string CommonHeaders::HTTP2_SETTINGS("http2-settings");
const std::string CommonHeaders::IF_MATCH("if-match");
const std::string CommonHeaders::IF_MODIFIED_SINCE("if-modified-since");
const std::string CommonHeaders::IF_NONE_MATCH("if-none-match");
const std::string CommonHeaders::IF_RANGE("if-range");
const std::string CommonHeaders::IF_UNMODIFIED_SINCE("if-unmodified-since");
const std::string CommonHeaders::MAX_FORWARDS("max-forwards");
const std::string CommonHeaders::ORIGIN("origin");
const std::string CommonHeaders::PREFER("prefer");
const std::string CommonHeaders::PROXY_AUTHORIZATION("proxy-authorization");
const std::string CommonHeaders::RANGE("range");
const std::string CommonHeaders::REFERER("referer");
const std::string CommonHeaders::TE("te");
const std::string CommonHeaders::USER_AGENT("user-agent");
// Standard request -

// Commons request +
const std::string CommonHeaders::UPGRADE_INSECURE_REQUESTS("upgrade-insecure-requests");
const std::string CommonHeaders::X_REQUESTED_WITH("x-requested-with");
const std::string CommonHeaders::DNT("dnt");
const std::string CommonHeaders::X_FORWARDED_FOR("x-forwarded-for");
const std::string CommonHeaders::X_FORWARDED_HOST("x-forwarded-host");
const std::string CommonHeaders::X_FORWARDED_PROTO("x-forwarded-proto");
const std::string CommonHeaders::FRONT_END_HTTPS("front-end-https");
const std::string CommonHeaders::X_HTTP_METHOD_OVERRIDE("x-http-method-override");
const std::string CommonHeaders::X_ATT_DEVICEID("x-att-deviceid");
const std::string CommonHeaders::X_WAP_PROFILE("x-wap-profile");
const std::string CommonHeaders::PROXY_CONNECTION("proxy-connection");
const std::string CommonHeaders::X_UIDH("x-uidh");
const std::string CommonHeaders::X_CSRF_TOKEN("x-csrf-token");
const std::string CommonHeaders::SAVE_DATA("save-data");
// Commons request -

// Standard response +
const std::string CommonHeaders::ACCEPT_CH("accept-ch");
const std::string CommonHeaders::ACCESS_CONTROL_ALLOW_ORIGIN("access-control-allow-origin");
const std::string CommonHeaders::ACCESS_CONTROL_ALLOW_CREDENTIALS("access-control-allow-credentials");
const std::string CommonHeaders::ACCESS_CONTROL_EXPOSE_HEADERS("access-control-expose-headers");
const std::string CommonHeaders::ACCESS_CONTROL_MAX_AGE("access-control-max-age");
const std::string CommonHeaders::ACCESS_CONTROL_ALLOW_METHODS("access-control-allow-methods");
const std::string CommonHeaders::ACCESS_CONTROL_ALLOW_HEADERS("access-control-allow-headers");
const std::string CommonHeaders::ACCEPT_PATCH("accept-patch");
const std::string CommonHeaders::ACCEPT_RANGES("accept-ranges");
const std::string CommonHeaders::AGE("age");
const std::string CommonHeaders::ALLOW("allow");
const std::string CommonHeaders::ALT_SVC("alt-svc");
const std::string CommonHeaders::CONTENT_DISPOSITION("content-disposition");
const std::string CommonHeaders::CONTENT_LANGUAGE("content-language");
const std::string CommonHeaders::CONTENT_LOCATION("content-location");
const std::string CommonHeaders::CONTENT_RANGE("content-range");
const std::string CommonHeaders::DELTA_BASE("delta-base");
const std::string CommonHeaders::ETAG("etag");
const std::string CommonHeaders::EXPIRES("expires");
const std::string CommonHeaders::IM("im");
const std::string CommonHeaders::LAST_MODIFIED("last-modified");
const std::string CommonHeaders::LINK("link");
const std::string CommonHeaders::LOCATION("location");
const std::string CommonHeaders::P3P("p3p");
const std::string CommonHeaders::PREFERENCE_APPLIED("preference-applied");
const std::string CommonHeaders::PROXY_AUTHENTICATE("proxy-authenticate");
const std::string CommonHeaders::PUBLIC_KEY_PINS("public-key-pins");
const std::string CommonHeaders::RETRY_AFTER("retry-after");
const std::string CommonHeaders::SERVER("server");
const std::string CommonHeaders::SET_COOKIE("set-cookie");
const std::string CommonHeaders::STRICT_TRANSPORT_SECURITY("strict-transport-security");
const std::string CommonHeaders::TK("tk");
const std::string CommonHeaders::VARY("vary");
const std::string CommonHeaders::WWW_AUTHENTICATE("www-authenticate");
const std::string CommonHeaders::X_FRAME_OPTIONS("x-frame-options");
// Standard response -

// Common response +
const std::string CommonHeaders::CONTENT_SECURITY_POLICY("content-security-policy");
const std::string CommonHeaders::X_CONTENT_SECURITY_POLICY("x-content-security-policy");
const std::string CommonHeaders::X_WEBKIT_CSP("x-webkit-csp");
const std::string CommonHeaders::EXPECT_CT("expect-ct");
const std::string CommonHeaders::NEL("nel");
const std::string CommonHeaders::PERMISSIONS_POLICY("permissions-policy");
const std::string CommonHeaders::REFRESH("refresh");
const std::string CommonHeaders::REPORT_TO("report-to");
const std::string CommonHeaders::STATUS("status");
const std::string CommonHeaders::TIMING_ALLOW_ORIGIN("timing-allow-origin");
const std::string CommonHeaders::X_CONTENT_DURATION("x-content-duration");
const std::string CommonHeaders::X_CONTENT_TYPE_OPTIONS("x-content-type-options");
const std::string CommonHeaders::X_POWERED_BY("x-powered-by");
const std::string CommonHeaders::X_REDIRECT_BY("x-redirect-by");
const std::string CommonHeaders::X_UA_COMPATIBLE("x-ua-compatible");
const std::string CommonHeaders::X_XSS_PROTECTION("x-xss-protection");
// Common response -
// Common headers -

const std::string anch::rest::EMPTY_VALUE;
const std::vector<std::string> anch::rest::EMPTY_VEC;
const std::regex anch::rest::PATH_PATTERN("\\{\\}");
