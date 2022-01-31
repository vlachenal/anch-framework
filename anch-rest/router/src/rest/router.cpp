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
#include "rest/router.hpp"

#include "rest/constants.hpp"

#include <iostream> // \todo remove


using anch::rest::Router;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::EndPoint;
using anch::rest::EndpointFunc;
using anch::rest::StatusCode;
using anch::rest::Verb;
using anch::rest::MediaType;


// Constants +
const uint16_t MASK_NOT_FOUND = 0x01;
const uint16_t MASK_METHOD_NOT_ALLOWED = 0x02;
const uint16_t MASK_UNSUPPORTED_MEDIA_TYPE = 0x04;
const uint16_t MASK_NOT_ACCEPTABLE = 0x08;
// Constants -


// Router +
// Constructors +
Router::Router(): _routes() {
  // Nothing to do
}
// Constructors -

// Destructor +
Router::~Router() {
  for(auto iter = _routes.begin() ; iter != _routes.end() ; ++iter) {
    delete iter->second;
  }
}
// Destructor -

// Internal functions +
/*!
 * Check if request is eligible for router according to its path
 *
 * \param request the request
 * \param endpoint the endpoint to match
 *
 * \return \c 0 if match, \c MASK_NOT_FOUND otherwise
 */
uint16_t
checkEligiblePath(const Request& request, const EndPoint& endpoint) {
  return endpoint.match(request) ? 0 : MASK_NOT_FOUND;
}

/*!
 * Check if request is eligible for router according to its verb
 *
 * \param request the request
 * \param endpoint the endpoint to match
 *
 * \return \c 0 if match, \c MASK_METHOD_NOT_ALLOWED otherwise
 */
uint16_t
checkEligibleVerb(const Request& request, const EndPoint& endpoint) {
  return request.getVerb() == endpoint.verb ? 0 : MASK_METHOD_NOT_ALLOWED;
}

/*!
 * Check if request is eligible for router according to its content media type
 *
 * \param request the request
 * \param endpoint the endpoint to match
 *
 * \return \c 0 if match, \c MASK_UNSUPPORTED_MEDIA_TYPE otherwise
 */
uint16_t
checkEligibleContentType(const Request& request, const EndPoint& endpoint) {
  uint16_t res = 0;
  if(endpoint.contentType == MediaType::ALL) { // if endpoint consumes any media type, it has to be kept
    return res;
  }
  if(endpoint.contentType != request.getContentType()) {
    res = MASK_UNSUPPORTED_MEDIA_TYPE;
  }
  return res;
}

/*!
 * Check if request is eligible for router according to its readable media type
 *
 * \param request the request
 * \param endpoint the endpoint to match
 *
 * \return \c 0 if match, \c MASK_NOT_ACCEPTABLE otherwise
 */
uint16_t
checkEligibleAcceptable(const Request& request, const EndPoint& endpoint) {
  if(endpoint.accept == MediaType::ALL) { // if endpoint produces any media type, it has to be kept
    return 0;
  }
  bool accept = false;
  auto header = request.getAccept();
  if(header.has_value()) {
    for(auto iter = header.value().begin() ; iter != header.value().end() ; ++iter) {
      std::cout << *iter << std::endl;
      if(*iter == endpoint.accept) {
	accept = true;
	break;
      }
      if(accept) {
	break;
      }
    }
  }
  return accept ? 0 : MASK_NOT_ACCEPTABLE;
}

/*!
 * Get eligible endpoint according to request
 *
 * \param request the request
 * \param endpoints the declared endpoints
 *
 * \return the selected endpoint
 *
 * \throw StatusCode the error status code (404, 405, 406 or 415)
 */
std::pair<EndPoint, EndpointFunc*>
getEligibleEndpoint(const Request& request, const std::map<EndPoint, EndpointFunc*>& endpoints) {
  std::map<EndPoint, EndpointFunc*> eligibles;
  uint16_t checks = 0;
  // Check accept all
  auto acceptHeader = request.getAccept();
  bool acceptAll = !acceptHeader.has_value();
  if(!acceptAll) {
    for(auto iter = acceptHeader.value().begin() ; iter != acceptHeader.value().end() ; ++iter) {
      if(*iter == MediaType::ALL) {
	acceptAll = true;
	break;
      }
    }
  }
  for(auto iter = endpoints.begin() ; iter != endpoints.end() ; ++iter) {
    const EndPoint& endpoint = iter->first;
    // Check path +
    uint16_t res = checkEligiblePath(request, endpoint);
    if(res != 0) {
      checks |= res;
      continue;
    }
    // Check path -
    // Check verb +
    res |= checkEligibleVerb(request, endpoint);
    if(res != 0) {
      checks |= res;
      continue;
    }
    // Check verb -
    // Check content type +
    res |= checkEligibleContentType(request, endpoint);
    if(res != 0) {
      checks |= res;
      continue;
    }
    // Check content type -
    // Check acceptables +
    if(!acceptAll) { // if client accepts every media type, eligible endpoints are kept
      res |= checkEligibleAcceptable(request, endpoint);
      if(res != 0) {
	checks |= res;
	continue;
      }
    }
    // Check acceptables -
    std::cout << "Found matching endpoint" << std::endl;
    eligibles.insert(*iter);
  }
  // Compute error +
  if(eligibles.empty()) {
    if(checks & MASK_NOT_ACCEPTABLE) {
      throw StatusCode::NOT_ACCEPTABLE;
    } else if(checks & MASK_UNSUPPORTED_MEDIA_TYPE) {
      throw StatusCode::UNSUPPORTED_MEDIA_TYPE;
    } else if(checks & MASK_METHOD_NOT_ALLOWED) {
      throw StatusCode::METHOD_NOT_ALLOWED;
    } else if(checks & MASK_NOT_FOUND) {
      throw StatusCode::NOT_FOUND;
    } else { // should not happen
      throw StatusCode::INTERNAL_SERVER_ERROR;
    }
  }
  // Compute error -
  return *(eligibles.begin());
}
// Internal functions -

// Methods +
Response
Router::execute(Request& request) noexcept {
  Response response;
  try {
    std::pair<EndPoint, EndpointFunc*> endpoint = getEligibleEndpoint(request, _routes);

    // \todo manage interceptors before endpoint execution => it can change request or put data in thread local ?

    std::vector<std::string> parameters;
    std::smatch matches;
    if(std::regex_match(request.getPath(), matches, endpoint.first.getPathRegex())) {
      auto iter = matches.begin();
      ++iter;
      for(; iter != matches.end() ; ++iter) {
	parameters.push_back(*iter);
      }
    }
    request.setParameters(parameters);
    response = endpoint.second->execute(request);

  } catch(const StatusCode& code) {
    response.setStatus(static_cast<uint16_t>(code));

  } catch(const int& code) {
    response.setStatus(static_cast<uint16_t>(code));

  } catch(const std::exception& e) { // status will be 500 and body the exception message
    response.setBody(e.what());

  } catch(...) {
    // Nothing to do ... reponse is already initialized with status code 500
  }

  // \todo manage interceptors after endpoint execution => it can change response
  return response;
}
// Methods -
// Router -
