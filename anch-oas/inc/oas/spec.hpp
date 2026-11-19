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

#include <cstdint>
#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "json/factory.hpp"

namespace anch::oas {

  struct Schema;

  /*!
   * \brief Schema \c additionalProperties value
   *
   * \c additionalProperties is either a boolean or a schema.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT AdditionalProperties {
    /*! Additional properties are allowed (\c false when \c additionalProperties is \c false ) */
    bool allowed = true;

    /*! Additional properties schema (when \c additionalProperties is a schema) */
    std::shared_ptr<anch::oas::Schema> schema;
  };

  /*!
   * \brief JSON scalar value (enumeration value)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT Scalar {
    /*! Value kind */
    enum class Kind {
      /*! \c null */
      NUL,
      /*! Character string */
      STRING,
      /*! Number */
      NUMBER,
      /*! Boolean */
      BOOLEAN,
      /*! Object or array */
      OTHER
    };

    /*! Value kind */
    Kind kind = Kind::NUL;

    /*! Value as text (string content without quotes, number as written in the document, \c true or \c false ) */
    std::string text;
  };

  /*!
   * \brief OpenAPI 3.0 schema object
   *
   * Only the keywords used to generate models are kept: other keywords are ignored.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT Schema {
    /*! Reference ( \c $ref ) */
    std::optional<std::string> ref;

    /*! Type ( \c string , \c integer , \c number , \c boolean , \c array or \c object ) */
    std::optional<std::string> type;

    /*! Type format ( \c int32 , \c int64 , \c float , \c double , \c uuid , \c date-time , ...) */
    std::optional<std::string> format;

    /*! Title */
    std::optional<std::string> title;

    /*! Description */
    std::optional<std::string> description;

    /*! \c null is allowed */
    std::optional<bool> nullable;

    /*! Object properties */
    std::map<std::string,anch::oas::Schema> properties;

    /*! Required object properties */
    std::vector<std::string> required;

    /*! Object additional properties */
    std::optional<anch::oas::AdditionalProperties> additionalProperties;

    /*! Array items */
    std::shared_ptr<anch::oas::Schema> items;

    /*! Allowed values */
    std::vector<anch::oas::Scalar> enumeration;

    /*! Composition: every schema */
    std::vector<anch::oas::Schema> allOf;

    /*! Composition: exactly one schema */
    std::vector<anch::oas::Schema> oneOf;

    /*! Composition: at least one schema */
    std::vector<anch::oas::Schema> anyOf;

    // Constraints +
    /*! String minimum length
     *
     * \since 0.2
     */
    std::optional<uint64_t> minLength;

    /*! String maximum length
     *
     * \since 0.2
     */
    std::optional<uint64_t> maxLength;

    /*! String pattern (documented, not checked)
     *
     * \since 0.2
     */
    std::optional<std::string> pattern;

    /*! Number minimum (as written in the document)
     *
     * \since 0.2
     */
    std::optional<anch::oas::Scalar> minimum;

    /*! Number maximum (as written in the document)
     *
     * \since 0.2
     */
    std::optional<anch::oas::Scalar> maximum;

    /*! \c minimum is exclusive
     *
     * \since 0.2
     */
    std::optional<bool> exclusiveMinimum;

    /*! \c maximum is exclusive
     *
     * \since 0.2
     */
    std::optional<bool> exclusiveMaximum;

    /*! Number divisor (as written in the document)
     *
     * \since 0.2
     */
    std::optional<anch::oas::Scalar> multipleOf;

    /*! Array minimum number of items
     *
     * \since 0.2
     */
    std::optional<uint64_t> minItems;

    /*! Array maximum number of items
     *
     * \since 0.2
     */
    std::optional<uint64_t> maxItems;

    /*! Array items are unique
     *
     * \since 0.2
     */
    std::optional<bool> uniqueItems;

    /*! Object minimum number of properties
     *
     * \since 0.2
     */
    std::optional<uint64_t> minProperties;

    /*! Object maximum number of properties
     *
     * \since 0.2
     */
    std::optional<uint64_t> maxProperties;
    // Constraints -
  };

  /*!
   * \brief OpenAPI 3.0 info object
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT Info {
    /*! API title */
    std::string title;

    /*! API version */
    std::string version;
  };

  /*!
   * \brief OpenAPI 3.0 media type object
   *
   * \since 0.2
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT MediaTypeObject {
    /*! Content schema */
    std::optional<anch::oas::Schema> schema;
  };

  /*!
   * \brief OpenAPI 3.0 parameter object
   *
   * \since 0.2
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT Parameter {
    /*! Reference ( \c $ref to \c #/components/parameters/ ) */
    std::optional<std::string> ref;

    /*! Name */
    std::string name;

    /*! Location ( \c path , \c query , \c header or \c cookie ) */
    std::string in;

    /*! Description */
    std::optional<std::string> description;

    /*! Parameter is required (always for path parameters) */
    std::optional<bool> required;

    /*! Schema */
    std::optional<anch::oas::Schema> schema;
  };

  /*!
   * \brief OpenAPI 3.0 request body object
   *
   * \since 0.2
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT RequestBody {
    /*! Reference ( \c $ref to \c #/components/requestBodies/ ) */
    std::optional<std::string> ref;

    /*! Description */
    std::optional<std::string> description;

    /*! Body is required */
    std::optional<bool> required;

    /*! Content by media type */
    std::map<std::string,anch::oas::MediaTypeObject> content;
  };

  /*!
   * \brief OpenAPI 3.0 response object
   *
   * \since 0.2
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT ResponseObject {
    /*! Reference ( \c $ref to \c #/components/responses/ ) */
    std::optional<std::string> ref;

    /*! Description */
    std::optional<std::string> description;

    /*! Content by media type */
    std::map<std::string,anch::oas::MediaTypeObject> content;
  };

  /*!
   * \brief OpenAPI 3.0 operation object
   *
   * \since 0.2
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT Operation {
    /*! Operation identifier */
    std::optional<std::string> operationId;

    /*! Summary */
    std::optional<std::string> summary;

    /*! Description */
    std::optional<std::string> description;

    /*! Operation is deprecated */
    std::optional<bool> deprecated;

    /*! Parameters */
    std::vector<anch::oas::Parameter> parameters;

    /*! Request body */
    std::optional<anch::oas::RequestBody> requestBody;

    /*! Responses by status code ( \c 200 , \c 2XX , \c default , ...) */
    std::map<std::string,anch::oas::ResponseObject> responses;
  };

  /*!
   * \brief OpenAPI 3.0 path item object
   *
   * \since 0.2
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT PathItem {
    /*! Parameters shared by operations */
    std::vector<anch::oas::Parameter> parameters;

    /*! \c GET operation */
    std::optional<anch::oas::Operation> get;

    /*! \c PUT operation */
    std::optional<anch::oas::Operation> put;

    /*! \c POST operation */
    std::optional<anch::oas::Operation> post;

    /*! \c DELETE operation */
    std::optional<anch::oas::Operation> del;

    /*! \c PATCH operation */
    std::optional<anch::oas::Operation> patch;

    /*! \c HEAD operation (not generated) */
    std::optional<anch::oas::Operation> head;

    /*! \c OPTIONS operation (not generated) */
    std::optional<anch::oas::Operation> options;

    /*! \c TRACE operation (not generated) */
    std::optional<anch::oas::Operation> trace;
  };

  /*!
   * \brief OpenAPI 3.0 components object (schemas, parameters, request bodies and responses)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT Components {
    /*! Schemas indexed by their names */
    std::map<std::string,anch::oas::Schema> schemas;

    /*! Parameters indexed by their names
     *
     * \since 0.2
     */
    std::map<std::string,anch::oas::Parameter> parameters;

    /*! Request bodies indexed by their names
     *
     * \since 0.2
     */
    std::map<std::string,anch::oas::RequestBody> requestBodies;

    /*! Responses indexed by their names
     *
     * \since 0.2
     */
    std::map<std::string,anch::oas::ResponseObject> responses;
  };

  /*!
   * \brief OpenAPI 3.0 document (models and endpoints parts)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ANCH_OAS_EXPORT OpenAPI {
    /*! OpenAPI specification version */
    std::string openapi;

    /*! API informations */
    anch::oas::Info info;

    /*! Paths
     *
     * \since 0.2
     */
    std::map<std::string,anch::oas::PathItem> paths;

    /*! Components */
    anch::oas::Components components;
  };

  /*!
   * Parse OpenAPI 3.0 document (JSON format). Unknown keywords are ignored.
   *
   * \param input the input stream to read
   *
   * \return the document
   *
   * \throw anch::json::MappingError invalid JSON or document structure
   * \throw std::invalid_argument the document is not an OpenAPI 3.0 document
   */
  ANCH_OAS_EXPORT anch::oas::OpenAPI parse(std::istream& input);

}  // anch::oas

namespace anch::json {

  /*!
   * \ref anch::oas::AdditionalProperties is deserialized with a dedicated mapper (boolean or schema)
   *
   * \return \c true
   */
  template<>
  constexpr bool isPrimitive<anch::oas::AdditionalProperties>() {
    return true;
  }

  /*!
   * \ref anch::oas::Scalar is deserialized with a dedicated mapper (any JSON value)
   *
   * \return \c true
   */
  template<>
  constexpr bool isPrimitive<anch::oas::Scalar>() {
    return true;
  }

}  // anch::json
