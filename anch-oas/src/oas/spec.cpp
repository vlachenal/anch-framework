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
#include "oas/spec.hpp"

#include <stdexcept>

#include "json/json.hpp"
#include "json/lexer.hpp"
#include "json/numberScanner.hpp"
#include "json/primitiveMapper.hpp"

using anch::oas::AdditionalProperties;
using anch::oas::Components;
using anch::oas::Info;
using anch::oas::OpenAPI;
using anch::oas::Scalar;
using anch::oas::Schema;


namespace anch::json {

  // AdditionalProperties mapper: boolean or schema +
  template<>
  PrimitiveMapper<AdditionalProperties>::PrimitiveMapper():
    anch::json::GenericMapper<PrimitiveMapper<AdditionalProperties>,AdditionalProperties>() {
    // Nothing to do
  }

  template<>
  PrimitiveMapper<AdditionalProperties>::~PrimitiveMapper() {
    // Nothing to do
  }

  template<>
  void
  PrimitiveMapper<AdditionalProperties>::serializeValue(const AdditionalProperties& value,
							anch::json::WriterContext& context) const {
    if(value.schema) {
      anch::json::Factory<Schema>::getInstance().serialize(*value.schema, context);
    } else {
      anch::json::Factory<bool>::getInstance().serialize(value.allowed, context);
    }
  }

  template<>
  bool
  PrimitiveMapper<AdditionalProperties>::deserializeValue(AdditionalProperties& value,
							  anch::json::ReaderContext& context) const {
    char current = context.discard();
    context.back();
    if(current == 't' || current == 'f') {
      return anch::json::lexBoolean(value.allowed, context);
    }
    value.schema = std::make_shared<Schema>();
    return anch::json::Factory<Schema>::getInstance().deserialize(*value.schema, context);
  }

  template class PrimitiveMapper<AdditionalProperties>;
  // AdditionalProperties mapper: boolean or schema -

  // Scalar mapper: any JSON value +
  template<>
  PrimitiveMapper<Scalar>::PrimitiveMapper():
    anch::json::GenericMapper<PrimitiveMapper<Scalar>,Scalar>() {
    // Nothing to do
  }

  template<>
  PrimitiveMapper<Scalar>::~PrimitiveMapper() {
    // Nothing to do
  }

  template<>
  void
  PrimitiveMapper<Scalar>::serializeValue(const Scalar& value, anch::json::WriterContext& context) const {
    if(value.kind == Scalar::Kind::STRING) {
      anch::json::Factory<std::string>::getInstance().serialize(value.text, context);
    } else {
      context.write(value.kind == Scalar::Kind::NUL ? std::string_view("null") : std::string_view(value.text));
    }
  }

  template<>
  bool
  PrimitiveMapper<Scalar>::deserializeValue(Scalar& value, anch::json::ReaderContext& context) const {
    char current = context.discard();
    switch(current) {
    case 'n': // lexNull expects 'n' to be consumed
      anch::json::lexNull(context);
      value.kind = Scalar::Kind::NUL;
      value.text = "null";
      return true;
    case anch::json::STRING_DELIMITER:
      context.back();
      value.kind = Scalar::Kind::STRING;
      return anch::json::lexString(value.text, context);
    case 't':
    case 'f':
      {
	context.back();
	bool boolean = false;
	value.kind = Scalar::Kind::BOOLEAN;
	bool res = anch::json::lexBoolean(boolean, context);
	value.text = boolean ? "true" : "false";
	return res;
      }
    case anch::json::OBJECT_BEGIN:
    case anch::json::ARRAY_BEGIN:
      context.back();
      value.kind = Scalar::Kind::OTHER;
      anch::json::lexUnknown(context);
      return true;
    default:
      {
	// Number is kept as written in the document (no conversion: integers bounds can be greater than 2^53)
	std::string_view chars = anch::json::scanNumber(context, anch::json::isNumberChar);
	if(!anch::json::isJsonNumber(chars)) {
	  throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context);
	}
	value.kind = Scalar::Kind::NUMBER;
	value.text = chars;
	return true;
      }
    }
  }

  template class PrimitiveMapper<Scalar>;
  // Scalar mapper: any JSON value -

}  // anch::json

// Objects mappers +
template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Schema>& mapper) {
  mapper
    .registerField<std::string>("$ref", &Schema::ref)
    .registerField<std::string>("type", &Schema::type)
    .registerField<std::string>("format", &Schema::format)
    .registerField<std::string>("title", &Schema::title)
    .registerField<std::string>("description", &Schema::description)
    .registerField<bool>("nullable", &Schema::nullable)
    .registerField<Schema>("properties", &Schema::properties)
    .registerField<std::string>("required", &Schema::required)
    .registerField<AdditionalProperties>("additionalProperties", &Schema::additionalProperties)
    .registerField<Schema>("items", &Schema::items)
    .registerField<Scalar>("enum", &Schema::enumeration)
    .registerField<Schema>("allOf", &Schema::allOf)
    .registerField<Schema>("oneOf", &Schema::oneOf)
    .registerField<Schema>("anyOf", &Schema::anyOf)
    .registerField<uint64_t>("minLength", &Schema::minLength)
    .registerField<uint64_t>("maxLength", &Schema::maxLength)
    .registerField<std::string>("pattern", &Schema::pattern)
    .registerField<Scalar>("minimum", &Schema::minimum)
    .registerField<Scalar>("maximum", &Schema::maximum)
    .registerField<bool>("exclusiveMinimum", &Schema::exclusiveMinimum)
    .registerField<bool>("exclusiveMaximum", &Schema::exclusiveMaximum)
    .registerField<Scalar>("multipleOf", &Schema::multipleOf)
    .registerField<uint64_t>("minItems", &Schema::minItems)
    .registerField<uint64_t>("maxItems", &Schema::maxItems)
    .registerField<bool>("uniqueItems", &Schema::uniqueItems)
    .registerField<uint64_t>("minProperties", &Schema::minProperties)
    .registerField<uint64_t>("maxProperties", &Schema::maxProperties)
    ;
}

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Info>& mapper) {
  mapper
    .registerField("title", &Info::title)
    .registerField("version", &Info::version)
    ;
}

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<Components>& mapper) {
  mapper
    .registerField<Schema>("schemas", &Components::schemas)
    ;
}

template<>
void
anch::json::registerObject(anch::json::ObjectMapper<OpenAPI>& mapper) {
  mapper
    .registerField("openapi", &OpenAPI::openapi)
    .registerField("info", &OpenAPI::info)
    .registerField("components", &OpenAPI::components)
    ;
}
// Objects mappers -

OpenAPI
anch::oas::parse(std::istream& input) {
  anch::json::MappingOptions options;
  options.deserialize_ignore_unknown_field = true; // only models keywords are mapped
  options.deserialize_max_discard_char = -1;       // documents are usually indented
  OpenAPI api = anch::json::deserialize<OpenAPI>(input, options);
  if(api.openapi.empty()) {
    throw std::invalid_argument("Not an OpenAPI 3.0 document: 'openapi' field not found (Swagger 2.0 documents are not supported)");
  }
  if(!api.openapi.starts_with("3.0.")) {
    throw std::invalid_argument("Unsupported OpenAPI version " + api.openapi + ": only 3.0.x documents are supported");
  }
  return api;
}
