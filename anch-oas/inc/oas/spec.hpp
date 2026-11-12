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
  struct AdditionalProperties {
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
  struct Scalar {
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

    /*! Value as text (string content without quotes, number, \c true or \c false ) */
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
  struct Schema {
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
  };

  /*!
   * \brief OpenAPI 3.0 info object
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct Info {
    /*! API title */
    std::string title;

    /*! API version */
    std::string version;
  };

  /*!
   * \brief OpenAPI 3.0 components object (schemas only)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct Components {
    /*! Schemas indexed by their names */
    std::map<std::string,anch::oas::Schema> schemas;
  };

  /*!
   * \brief OpenAPI 3.0 document (models part)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct OpenAPI {
    /*! OpenAPI specification version */
    std::string openapi;

    /*! API informations */
    anch::oas::Info info;

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
  anch::oas::OpenAPI parse(std::istream& input);

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
