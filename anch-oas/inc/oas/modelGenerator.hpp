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

#include "anch-oas-export.hpp"

#include <stdexcept>
#include <string>
#include <vector>

#include "oas/spec.hpp"

namespace anch::oas {

  /*!
   * \brief Models generation options
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT ModelOptions {
    /*! Namespace of generated models (\c :: separated identifiers) */
    std::string ns = "model";

    /*! Generated files base name: \<name\>.hpp and \<name\>.cpp */
    std::string name = "models";

    /*! Map \c uuid string format to \c anch::UUID (needs \c ANCH_UUID in anch-json), \c std::string otherwise */
    bool uuid = true;

    /*! Map \c date-time string format to \c anch::date::Date and \c date format to \c std::chrono::year_month_day
     * (RFC 3339, needs \c ANCH_DATE in anch-json), \c std::string otherwise */
    bool date = true;

    /*! Raise an error on unsupported schema instead of skipping it with a warning */
    bool strict = false;

    /*! Generate value constraints ( \c minLength , \c minimum , \c minItems , ...) in mappers. Required properties
     * are always checked.
     *
     * \since 0.2
     */
    bool constraints = true;

    /*! Generate endpoints server side ( \c paths ): service interface and its registration in \c anch::rest::Api
     * (\<name\>-server.hpp and \<name\>-server.cpp)
     *
     * \since 0.2
     */
    bool server = false;

    /*! Generate endpoints client side ( \c paths ): typed client based on \c anch::rest::Client
     * (\<name\>-client.hpp and \<name\>-client.cpp)
     *
     * \since 0.2
     */
    bool client = false;

    /*! API name: service is \<api\>Service and client is \<api\>Client (default: document title in PascalCase)
     *
     * \since 0.2
     */
    std::string api;
  };

  /*!
   * \brief Generated models
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT GeneratedModels {
    /*! Header content: models declaration */
    std::string header;

    /*! Source content: anch-json mappers */
    std::string source;

    /*! Server header content: service interface (when \c server option is set)
     *
     * \since 0.2
     */
    std::string serverHeader;

    /*! Server source content: endpoints registration (when \c server option is set)
     *
     * \since 0.2
     */
    std::string serverSource;

    /*! Client header content: typed client declaration (when \c client option is set)
     *
     * \since 0.2
     */
    std::string clientHeader;

    /*! Client source content: typed client implementation (when \c client option is set)
     *
     * \since 0.2
     */
    std::string clientSource;

    /*! Forms header content: \c anch::rest::FormMapper of form bodies models (when \c server or \c client option
     * is set)
     *
     * \since 0.2
     */
    std::string formsHeader;

    /*! Forms source content: \c anch::rest::FormMapper implementations (when \c server or \c client option is set)
     *
     * \since 0.2
     */
    std::string formsSource;

    /*! Warnings: schemas, properties and operations which have not been generated, constraints which have been
     * ignored */
    std::vector<std::string> warnings;
  };

  /*!
   * \brief Models generation error
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class ANCH_OAS_EXPORT GeneratorError: public std::runtime_error {
  public:
    /*!
     * \ref GeneratorError constructor
     *
     * \param message the error message
     */
    explicit GeneratorError(const std::string& message);
  };

  /*!
   * Generate C++ models and their anch-json mappers from the document schemas ( \c components.schemas ).\n
   * \n
   * Mapping:
   *  - \c string → \c std::string ( \c anch::UUID for \c uuid format, \c anch::date::Date for \c date-time format,
   *    \c std::chrono::year_month_day for \c date format), \c integer → \c int32_t / \c int64_t ,
   *    \c number → \c float / \c double , \c boolean → \c bool ;
   *  - objects with properties and \c allOf compositions → structures, inline objects get a name built from their
   *    path (\<Model\>\<Property\>) ;
   *  - string enumerations → <tt>enum class</tt> (unknown values are rejected on deserialization) ;
   *  - \c array → \c std::vector , objects with \c additionalProperties schema only → <tt>std::map<std::string,T></tt> ;
   *  - optional and \c nullable properties → \c std::optional , arrays and maps are empty when absent ;
   *  - required properties are registered with \c anch::json::required (absent or not nullable \c null value is
   *    refused on deserialization) ;
   *  - \c minLength , \c maxLength , \c minimum , \c maximum (exclusive or not), \c multipleOf , \c minItems ,
   *    \c maxItems , \c uniqueItems , \c minProperties , \c maxProperties and non string \c enum → anch-json value
   *    constraints (array items and map values constraints with \c anch::json::items ) ;
   *  - \c pattern → \c anch::json::pattern (invalid ECMAScript patterns are ignored with a warning), string
   *    \c format (which is not mapped to a type) → \c anch::json::format ;
   *  - models have a default \c operator== ;
   *  - recursive references → \c std::shared_ptr ;
   *  - other schemas (primitives, arrays, maps) → aliases ( \c using ).
   *
   * Not supported (warning, or error in strict mode): \c oneOf / \c anyOf , free-form objects, nested containers
   * (array of arrays, ...), references outside \c #/components/schemas .
   *
   * With \c server and \c client options, anch-rest endpoints are generated from the document paths: service interface,
   * its registration in \c anch::rest::Api and typed client (see \ref oas_endpoints_sec "endpoints generation").
   *
   * \param api the OpenAPI document
   * \param options the generation options
   *
   * \return the generated header, source and warnings
   *
   * \throw anch::oas::GeneratorError invalid options, unknown reference or unsupported schema in strict mode
   */
  ANCH_OAS_EXPORT anch::oas::GeneratedModels generateModels(const anch::oas::OpenAPI& api, const anch::oas::ModelOptions& options);

}  // anch::oas
