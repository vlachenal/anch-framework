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
  struct ModelOptions {
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
  };

  /*!
   * \brief Generated models
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct GeneratedModels {
    /*! Header content: models declaration */
    std::string header;

    /*! Source content: anch-json mappers */
    std::string source;

    /*! Warnings: schemas and properties which have not been generated */
    std::vector<std::string> warnings;
  };

  /*!
   * \brief Models generation error
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class GeneratorError: public std::runtime_error {
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
   *  - recursive references → \c std::shared_ptr ;
   *  - other schemas (primitives, arrays, maps) → aliases ( \c using ).
   *
   * Not supported (warning, or error in strict mode): \c oneOf / \c anyOf , free-form objects, nested containers
   * (array of arrays, ...), references outside \c #/components/schemas .
   *
   * \param api the OpenAPI document
   * \param options the generation options
   *
   * \return the generated header, source and warnings
   *
   * \throw anch::oas::GeneratorError invalid options, unknown reference or unsupported schema in strict mode
   */
  anch::oas::GeneratedModels generateModels(const anch::oas::OpenAPI& api, const anch::oas::ModelOptions& options);

}  // anch::oas
