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
#pragma once

#include <string>
#include <vector>
#include <regex>

namespace anch {
  namespace rest {

    // Verbs +
    /*!
     * \brief HTTP verbs
     *
     * This struct should be used in a static way
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    struct Verb {
      // HTTP specifications +
      /*! HTTP GET */
      static const std::string GET;
      /*! HTTP PUT */
      static const std::string PUT;
      /*! HTTP PACTH */
      static const std::string PATCH;
      /*! HTTP POST */
      static const std::string POST;
      /*! HTTP DELETE */
      static const std::string DELETE;
      /*! HTTP OPTIONS */
      static const std::string OPTIONS;
      /*! HTTP TRACE */
      static const std::string TRACE;
      /*! HTTP HEAD */
      static const std::string HEAD;
      /*! HTTP CONNECT */
      static const std::string CONNECT;
      // HTTP specifications -

      // WebDAV specifications +
      /*! HTTP COPY */
      static const std::string COPY;
      /*! HTTP LOCK */
      static const std::string LOCK;
      /*! HTTP MKCOL */
      static const std::string MKCOL;
      /*! HTTP MOVE */
      static const std::string MOVE;
      /*! HTTP PROPFIND */
      static const std::string PROPFIND;
      /*! HTTP PROPPATCH */
      static const std::string PROPPATCH;
      /*! HTTP UNLOCK */
      static const std::string UNLOCK;
      // WebDAV specifications -
    };
    // Verbs -

    // Status codes +
    /*!
     * \brief HTTP status codes enumeration class
     *
     * Only status codes which are part of RFCs are included.\n
     * Status code descriptions are imported from wikipedia.
     *
     * \see <a href="https://en.wikipedia.org/wiki/List_of_HTTP_status_codes">Wikipedia</a>
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    enum class StatusCode {
      // 1xx - Informations +
      /*! The server has received the request headers and the client should proceed to send the request body (in the case of a request for which a body needs to be sent; for example, a POST request). Sending a large request body to a server after a request has been rejected for inappropriate headers would be inefficient. To have a server check the request's headers, a client must send Expect: 100-continue as a header in its initial request and receive a 100 Continue status code in response before sending the body. If the client receives an error code such as 403 (Forbidden) or 405 (Method Not Allowed) then it shouldn't send the request's body. The response 417 Expectation Failed indicates that the request should be repeated without the Expect header as it indicates that the server doesn't support expectations (this is the case, for example, of HTTP/1.0 servers). */
      CONTINUE = 100,
      /*! The requester has asked the server to switch protocols and the server has agreed to do so. */
      SWITCHING_PROTOCOLES = 101,
      /*! A WebDAV request may contain many sub-requests involving file operations, requiring a long time to complete the request. This code indicates that the server has received and is processing the request, but no response is available yet. This prevents the client from timing out and assuming the request was lost. */
      PROCESSING = 102,
      /*! Used to return some response headers before final HTTP message. */
      EARLY_HINTS = 103,
      // 1xx - Informations -

      // 2xx - Success +
      /*! Standard response for successful HTTP requests. The actual response will depend on the request method used. In a GET request, the response will contain an entity corresponding to the requested resource. In a POST request, the response will contain an entity describing or containing the result of the action. */
      OK = 200,
      /*! The request has been fulfilled, resulting in the creation of a new resource. */
      CREATED = 201,
      /*! The request has been accepted for processing, but the processing has not been completed. The request might or might not be eventually acted upon, and may be disallowed when processing occurs. */
      ACCEPTED = 202,
      /*! The server is a transforming proxy (e.g. a Web accelerator) that received a 200 OK from its origin, but is returning a modified version of the origin's response. */
      NON_AUTH_INFORMATION = 203,
      /*! The server successfully processed the request, and is not returning any content. */
      NO_CONTENT = 204,
      /*! The server successfully processed the request, asks that the requester reset its document view, and is not returning any content. */
      RESET_CONTENT = 205,
      /*! The server is delivering only part of the resource (byte serving) due to a range header sent by the client. The range header is used by HTTP clients to enable resuming of interrupted downloads, or split a download into multiple simultaneous streams. */
      PARTIAL_CONTENT = 206,
      /*! The message body that follows is by default an XML message and can contain a number of separate response codes, depending on how many sub-requests were made. */
      MULTI_STATUS = 207,
      /*! The members of a DAV binding have already been enumerated in a preceding part of the (multistatus) response, and are not being included again. */
      ALREADY_REPORTED = 208,
      /*! The server has fulfilled a request for the resource, and the response is a representation of the result of one or more instance-manipulations applied to the current instance. */
      IM_USED = 209,
      // 2xx - Success -

      // 3xx - Redirection +
      /*! Indicates multiple options for the resource from which the client may choose (via agent-driven content negotiation). For example, this code could be used to present multiple video format options, to list files with different filename extensions, or to suggest word-sense disambiguation. */
      MULTIPLE_CHOICES = 300,
      /*! This and all future requests should be directed to the given URI. */
      MOVE_PERMANENTLY = 301,
      /*! Tells the client to look at (browse to) another URL. 302 has been superseded by 303 and 307. This is an example of industry practice contradicting the standard. The HTTP/1.0 specification (RFC 1945) required the client to perform a temporary redirect (the original describing phrase was "Moved Temporarily"), but popular browsers implemented 302 with the functionality of a 303 See Other. Therefore, HTTP/1.1 added status codes 303 and 307 to distinguish between the two behaviours. However, some Web applications and frameworks use the 302 status code as if it were the 303. */
      FOUND = 302,
      /*! The response to the request can be found under another URI using the GET method. When received in response to a POST (or PUT/DELETE), the client should presume that the server has received the data and should issue a new GET request to the given URI. */
      SEE_OTHER = 303,
      /*! Indicates that the resource has not been modified since the version specified by the request headers If-Modified-Since or If-None-Match. In such case, there is no need to retransmit the resource since the client still has a previously-downloaded copy. */
      NOT_MODIFIED = 304,
      /*! The requested resource is available only through a proxy, the address for which is provided in the response. For security reasons, many HTTP clients (such as Mozilla Firefox and Internet Explorer) do not obey this status code. */
      USE_PROXY = 305,
      /*! No longer used. Originally meant "Subsequent requests should use the specified proxy." */
      SWITCH_PROXY = 306,
      /*! In this case, the request should be repeated with another URI; however, future requests should still use the original URI. In contrast to how 302 was historically implemented, the request method is not allowed to be changed when reissuing the original request. For example, a POST request should be repeated using another POST request. */
      TEMPORARY_REDIRECT = 307,
      /*! The request and all future requests should be repeated using another URI. 307 and 308 parallel the behaviors of 302 and 301, but do not allow the HTTP method to change. So, for example, submitting a form to a permanently redirected resource may continue smoothly. */
      PERMANENT_REDIRECT = 308,
      // 3xx - Redirection -

      // 4xx - Clients errors +
      /*! The server cannot or will not process the request due to an apparent client error (e.g., malformed request syntax, size too large, invalid request message framing, or deceptive request routing). */
      BAD_REQUEST = 400,
      /*!
       * Similar to 403 Forbidden, but specifically for use when authentication is required and has failed or has not yet been provided. The response must include a WWW-Authenticate header field containing a challenge applicable to the requested resource. See Basic access authentication and Digest access authentication. 401 semantically means "unauthorised", the user does not have valid authentication credentials for the target resource.\n
       * Note: Some sites incorrectly issue HTTP 401 when an IP address is banned from the website (usually the website domain) and that specific address is refused permission to access a website.
       */
      UNAUTHORIZED = 401,
      /*! Reserved for future use. The original intention was that this code might be used as part of some form of digital cash or micropayment scheme, as proposed, for example, by GNU Taler, but that has not yet happened, and this code is not widely used. Google Developers API uses this status if a particular developer has exceeded the daily limit on requests. Sipgate uses this code if an account does not have sufficient funds to start a call. Shopify uses this code when the store has not paid their fees and is temporarily disabled. Stripe uses this code for failed payments where parameters were correct, for example blocked fraudulent payments. */
      PAYMENT_REQUIRED = 402,
      /*! The request contained valid data and was understood by the server, but the server is refusing action. This may be due to the user not having the necessary permissions for a resource or needing an account of some sort, or attempting a prohibited action (e.g. creating a duplicate record where only one is allowed). This code is also typically used if the request provided authentication by answering the WWW-Authenticate header field challenge, but the server did not accept that authentication. The request should not be repeated. */
      FORBIDDEN = 403,
      /*! The requested resource could not be found but may be available in the future. Subsequent requests by the client are permissible. */
      NOT_FOUND = 404,
      /*! A request method is not supported for the requested resource; for example, a GET request on a form that requires data to be presented via POST, or a PUT request on a read-only resource. */
      METHOD_NOT_ALLOWED = 405,
      /*! The requested resource is capable of generating only content not acceptable according to the Accept headers sent in the request. See Content negotiation. */
      NOT_ACCEPTABLE = 406,
      /*! The client must first authenticate itself with the proxy. */
      PROXY_AUTHENTICATION_REQUIRED = 407,
      /*! The server timed out waiting for the request. According to HTTP specifications: "The client did not produce a request within the time that the server was prepared to wait. The client MAY repeat the request without modifications at any later time." */
      REQUEST_TIMEOUT = 408,
      /*! Indicates that the request could not be processed because of conflict in the current state of the resource, such as an edit conflict between multiple simultaneous updates. */
      CONFLICT = 409,
      /*! Indicates that the resource requested is no longer available and will not be available again. This should be used when a resource has been intentionally removed and the resource should be purged. Upon receiving a 410 status code, the client should not request the resource in the future. Clients such as search engines should remove the resource from their indices. Most use cases do not require clients and search engines to purge the resource, and a "404 Not Found" may be used instead. */
      GONE = 410,
      /*! The request did not specify the length of its content, which is required by the requested resource. */
      LENGTH_REQUIRED = 411,
      /*! The server does not meet one of the preconditions that the requester put on the request header fields. */
      PRECONDITION_FAILED = 412,
      /*! The request is larger than the server is willing or able to process. Previously called "Request Entity Too Large". */
      PAYLOAD_TOO_LARGE = 413,
      /*! The URI provided was too long for the server to process. Often the result of too much data being encoded as a query-string of a GET request, in which case it should be converted to a POST request. Called "Request-URI Too Long" previously. */
      URI_TOO_LONG = 414,
      /*! The request entity has a media type which the server or resource does not support. For example, the client uploads an image as image/svg+xml, but the server requires that images use a different format. */
      UNSUPPORTED_MEDIA_TYPE = 415,
      /*! The client has asked for a portion of the file (byte serving), but the server cannot supply that portion. For example, if the client asked for a part of the file that lies beyond the end of the file. Called "Requested Range Not Satisfiable" previously. */
      RANGE_NOT_SATISFIABLE = 416,
      /*! The server cannot meet the requirements of the Expect request-header field. */
      EXPECTATION_FAILED = 417,
      /*! This code was defined in 1998 as one of the traditional IETF April Fools' jokes, in RFC 2324, Hyper Text Coffee Pot Control Protocol, and is not expected to be implemented by actual HTTP servers. The RFC specifies this code should be returned by teapots requested to brew coffee. This HTTP status is used as an Easter egg in some websites, such as Google.com's I'm a teapot easter egg. */
      IM_A_TEAPOT = 418,
      /*! The request was directed at a server that is not able to produce a response (for example because of connection reuse). */
      MISREDIRECT_REQUEST = 421,
      /*! The request was well-formed but was unable to be followed due to semantic errors. */
      UNPROCESSABLE_ENTITY = 422,
      /* The resource that is being accessed is locked. */
      LOCKED = 423,
      /*! The request failed because it depended on another request and that request failed (e.g., a PROPPATCH). */
      FAILED_DEPENDENCY = 424,
      /*! Indicates that the server is unwilling to risk processing a request that might be replayed. */
      TOO_EARLY = 425,
      /*! The client should switch to a different protocol such as TLS/1.0, given in the Upgrade header field. */
      UPGRADE_REQUIRED = 426,
      /*! The origin server requires the request to be conditional. Intended to prevent the 'lost update' problem, where a client GETs a resource's state, modifies it, and PUTs it back to the server, when meanwhile a third party has modified the state on the server, leading to a conflict. */
      PRECONDITION_REQUIRED = 428,
      /*! The user has sent too many requests in a given amount of time. Intended for use with rate-limiting schemes. */
      TOO_MANY_REQUESTS = 429,
      /*! The server is unwilling to process the request because either an individual header field, or all the header fields collectively, are too large. */
      REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
      /*! A server operator has received a legal demand to deny access to a resource or to a set of resources that includes the requested resource. The code 451 was chosen as a reference to the novel Fahrenheit 451 (see the Acknowledgements in the RFC). */
      UNAVAILABLE_FOR_LEGAL_REASONS = 451,
      // 4xx - Clients errors -

      // 5xx - Server errors +
      /*! A generic error message, given when an unexpected condition was encountered and no more specific message is suitable. */
      INTERNAL_SERVER_ERROR = 500,
      /*! The server either does not recognize the request method, or it lacks the ability to fulfil the request. Usually this implies future availability (e.g., a new feature of a web-service API). */
      NOT_IMPLEMENTED = 501,
      /*! The server was acting as a gateway or proxy and received an invalid response from the upstream server. */
      BAD_GATEWAY = 502,
      /*! The server cannot handle the request (because it is overloaded or down for maintenance). Generally, this is a temporary state. */
      SERVICE_UNAVAILABLE = 503,
      /*! The server was acting as a gateway or proxy and did not receive a timely response from the upstream server. */
      GATEWAY_TIMEOUT = 504,
      /*! The server does not support the HTTP protocol version used in the request. */
      HTTP_VERSION_NOT_SUPPORTED = 505,
      /*! Transparent content negotiation for the request results in a circular reference. */
      VARIANT_ALSO_NEGOTIATES = 506,
      /*! The server is unable to store the representation needed to complete the request. */
      INSUFFICIENT_STORAGE = 507,
      /*! The server detected an infinite loop while processing the request (sent instead of 208 Already Reported). */
      LOOP_DETECTED = 508,
      /*! Further extensions to the request are required for the server to fulfil it. */
      NOT_EXTENDED = 510,
      /*! The client needs to authenticate to gain network access. Intended for use by intercepting proxies used to control access to the network (e.g., "captive portals" used to require agreement to Terms of Service before granting full Internet access via a Wi-Fi hotspot). */
      NETWORK_AUTHENTICATION_REQUIRED = 511
      // 5xx - Server errors -
    };
    // Status codes -

    // Common media types +
    /*!
     * \brief Media type
     *
     * Common media types. This struct should be used in a static way.
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    struct MediaType {
      /*! No content to parse (empty string) */
      static const std::string NONE;
      /*! Any media types (*\/\*) */
      static const std::string ALL;
      /*! application/graphql */
      static const std::string APP_GRAPHQL;
      /*! application/javascript */
      static const std::string APP_JAVASCRIPT;
      /*! application/json */
      static const std::string APP_JSON;
      /*! application/octet-stream */
      static const std::string APP_OCTET_STREAM;
      /*! application/vnd.google.protobuf */
      static const std::string APP_VND_PROTOBUF;
      /*! application/vnd.apache.thrift.binary */
      static const std::string APP_VND_THRIFT_BINARY;
      /*! application/vnd.apache.thrift.compact */
      static const std::string APP_VND_THRIFT_COMPACT;
      /*! application/vnd.apache.thrift.json */
      static const std::string APP_VND_THRIFT_JSON;
      /*! application/x-protobuf */
      static const std::string APP_X_PROTOBUF;
      /*! application/x-www-form-urlencoded */
      static const std::string APP_X_WWW_FORM_URLENCODED;
      /*! application/xml */
      static const std::string APP_XML;
      /*! multipart/form-data */
      static const std::string MULTIPART_FORM_DATA;
      /*! text/css */
      static const std::string TXT_CSS;
      /*! text/csv */
      static const std::string TXT_CSV;
      /*! text/html */
      static const std::string TXT_HTML;
      /*! text/plain */
      static const std::string TXT_PLAIN;
      /*! text/xml */
      static const std::string TXT_XML;
    };
    // Common media types -

    // Common headers +
    /*!
     * \brief Common HTTP headers
     *
     * \author Vincent Lachenal
     *
     * \since 0.1
     */
    struct CommonHeaders {

      // Standard commons +
      /*! Cache-Control header */
      static const std::string CACHE_CONTROL;
      /*! Connection header */
      static const std::string CONNECTION;
      /*! Content-Encoding header */
      static const std::string CONTENT_ENCODING;
      /*! Content-Length header */
      static const std::string CONTENT_LENGTH;
      /*! Content-MD5 header */
      static const std::string CONTENT_MD5;
      /*! Content-Type header */
      static const std::string CONTENT_TYPE;
      /*! Date header */
      static const std::string DATE;
      /*! Pragma header */
      static const std::string PRAGMA;
      /*! Trailer header */
      static const std::string TRAILER;
      /*! Transfer-Encoding header */
      static const std::string TRANSFER_ENCODING;
      /*! Upgrade header */
      static const std::string UPGRADE;
      /*! Via header */
      static const std::string VIA;
      /*! Warning header */
      static const std::string WARNING;
      // Standard commons -

      // Commons +
      /*! X-Request-ID header */
      static const std::string X_REQUEST_ID;
      /*! X-Correlation-ID header */
      static const std::string X_CORRELATION_ID;
      // Commons -

      // Standard request +
      /*! A-IM header */
      static const std::string A_IM;
      /*! Accept header */
      static const std::string ACCEPT;
      /*! Accept-Charset header */
      static const std::string ACCEPT_CHARSET;
      /*! Accept-Datetime header */
      static const std::string ACCEPT_DATETIME;
      /*! Accept-Language header */
      static const std::string ACCEPT_LANGUAGE;
      /*! Access-Control-Request-Method header */
      static const std::string ACCESS_CONTROL_REQUEST_METHOD;
      /*! Access-Control-Request-Headers header */
      static const std::string ACCESS_CONTROL_REQUEST_HEADERS;
      /*! Authorization header */
      static const std::string AUTHORIZATION;
      /*! Cookie header */
      static const std::string COOKIE;
      /*! Expect header */
      static const std::string EXPECT;
      /*! Forwarded header */
      static const std::string FORWARDED;
      /*! From header */
      static const std::string FROM;
      /*! Host header */
      static const std::string HOST;
      /*! HTTP2-Settings header */
      static const std::string HTTP2_SETTINGS;
      /*! If-Match header */
      static const std::string IF_MATCH;
      /*! If-Modified-Since header */
      static const std::string IF_MODIFIED_SINCE;
      /*! If-None-Match header */
      static const std::string IF_NONE_MATCH;
      /*! If-Range header */
      static const std::string IF_RANGE;
      /*! If-Unmodified-Since header */
      static const std::string IF_UNMODIFIED_SINCE;
      /*! Max-Forwards header */
      static const std::string MAX_FORWARDS;
      /*! Origin header */
      static const std::string ORIGIN;
      /*! Prefer header */
      static const std::string PREFER;
      /*! Proxy-Authorization header */
      static const std::string PROXY_AUTHORIZATION;
      /*! Range header */
      static const std::string RANGE;
      /*! Referer header */
      static const std::string REFERER;
      /*! TE header */
      static const std::string TE;
      /*! User-Agent header */
      static const std::string USER_AGENT;
      // Standard request -

      // Commons request +
      /*! Upgrade-Insecure-Requests header */
      static const std::string UPGRADE_INSECURE_REQUESTS;
      /*! X-Requested-With header */
      static const std::string X_REQUESTED_WITH;
      /*! DNT header */
      static const std::string DNT;
      /*! X-Forwarded-For header */
      static const std::string X_FORWARDED_FOR;
      /*! X-Forwarded-Host header */
      static const std::string X_FORWARDED_HOST;
      /*! X-Forwarded-Proto header */
      static const std::string X_FORWARDED_PROTO;
      /*! Front-End-Https header */
      static const std::string FRONT_END_HTTPS;
      /*! X-Http-Method-Override header */
      static const std::string X_HTTP_METHOD_OVERRIDE;
      /*! X-ATT-DeviceId header */
      static const std::string X_ATT_DEVICEID;
      /*! X-Wap-Profile header */
      static const std::string X_WAP_PROFILE;
      /*! Proxy-Connection header */
      static const std::string PROXY_CONNECTION;
      /*! X-UIDH header */
      static const std::string X_UIDH;
      /*! X-Csrf-Token header */
      static const std::string X_CSRF_TOKEN;
      /*! Save-Data header */
      static const std::string SAVE_DATA;
      // Commons request -

      // Standard response +
      /*! Accept-CH header */
      static const std::string ACCEPT_CH;
      /*! Access-Control-Allow-Origin header */
      static const std::string ACCESS_CONTROL_ALLOW_ORIGIN;
      /*! Access-Control-Allow-Credentials header */
      static const std::string ACCESS_CONTROL_ALLOW_CREDENTIALS;
      /*! Access-Control-Expose-Headers header */
      static const std::string ACCESS_CONTROL_EXPOSE_HEADERS;
      /*! Access-Control-Max-Age header */
      static const std::string ACCESS_CONTROL_MAX_AGE;
      /*! Access-Control-Allow-Methods header */
      static const std::string ACCESS_CONTROL_ALLOW_METHODS;
      /*! Access-Control-Allow-Headers header */
      static const std::string ACCESS_CONTROL_ALLOW_HEADERS;
      /*! Accept-Patch header */
      static const std::string ACCEPT_PATCH;
      /*! Accept-Ranges header */
      static const std::string ACCEPT_RANGES;
      /*! Age header */
      static const std::string AGE;
      /*! Allow header */
      static const std::string ALLOW;
      /*! Alt-Svc header */
      static const std::string ALT_SVC;
      /*! Content-Disposition header */
      static const std::string CONTENT_DISPOSITION;
      /*! Content-Language header */
      static const std::string CONTENT_LANGUAGE;
      /*! Content-Location header */
      static const std::string CONTENT_LOCATION;
      /*! Content-Range header */
      static const std::string CONTENT_RANGE;
      /*! Delta-Base header */
      static const std::string DELTA_BASE;
      /*! ETag header */
      static const std::string ETAG;
      /*! Expires header */
      static const std::string EXPIRES;
      /*! IM header */
      static const std::string IM;
      /*! Last-Modified header */
      static const std::string LAST_MODIFIED;
      /*! Link header */
      static const std::string LINK;
      /*! Location header */
      static const std::string LOCATION;
      /*! P3P header */
      static const std::string P3P;
      /*! Preference-Applied header */
      static const std::string PREFERENCE_APPLIED;
      /*! Proxy-Authenticate header */
      static const std::string PROXY_AUTHENTICATE;
      /*! Public-Key-Pins header */
      static const std::string PUBLIC_KEY_PINS;
      /*! Retry-After header */
      static const std::string RETRY_AFTER;
      /*! Server header */
      static const std::string SERVER;
      /*! Set-Cookie header */
      static const std::string SET_COOKIE;
      /*! Strict-Transport-Security header */
      static const std::string STRICT_TRANSPORT_SECURITY;
      /*! Tk header */
      static const std::string TK;
      /*! Vary header */
      static const std::string VARY;
      /*! WWW-Authenticate header */
      static const std::string WWW_AUTHENTICATE;
      /*! X-Frame-Options header */
      static const std::string X_FRAME_OPTIONS;
      // Standard response -

      // Common response +
      /*! Content-Security-Policy header */
      static const std::string CONTENT_SECURITY_POLICY;
      /*! X-Content-Security-Policy header */
      static const std::string X_CONTENT_SECURITY_POLICY;
      /*! X-WebKit-CSP header */
      static const std::string X_WEBKIT_CSP;
      /*! Expect-CT header */
      static const std::string EXPECT_CT;
      /*! NEL header */
      static const std::string NEL;
      /*! Permissions-Policy header */
      static const std::string PERMISSIONS_POLICY;
      /*! Refresh header */
      static const std::string REFRESH;
      /*! Report-To header */
      static const std::string REPORT_TO;
      /*! Status header */
      static const std::string STATUS;
      /*! Timing-Allow-Origin header */
      static const std::string TIMING_ALLOW_ORIGIN;
      /*! X-Content-Duration header */
      static const std::string X_CONTENT_DURATION;
      /*! X-Content-Type-Options header */
      static const std::string X_CONTENT_TYPE_OPTIONS;
      /*! X-Powered-By header */
      static const std::string X_POWERED_BY;
      /*! X-Redirect-By header */
      static const std::string X_REDIRECT_BY;
      /*! X-UA-Compatible header */
      static const std::string X_UA_COMPATIBLE;
      /*! X-XSS-Protection header */
      static const std::string X_XSS_PROTECTION;
      // Common response -

    };
    // Common headers -

    /*! Empty value */
    extern const std::string EMPTY_VALUE;

    /*! Empty vector value ... */
    extern const std::vector<std::string> EMPTY_VEC; // \todo check if used

    /*! Path pattern placeholder */
    extern const std::regex PATH_PATTERN;

  }  // rest
}  // anch
