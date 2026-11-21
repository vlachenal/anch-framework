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
using anch::oas::MediaTypeObject;
using anch::oas::Operation;
using anch::oas::Parameter;
using anch::oas::PathItem;
using anch::oas::RequestBody;
using anch::oas::ResponseObject;
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
struct anch::Mapping<Schema> {
  static void
  registerObject(anch::ObjectMapper<Schema> auto& mapper) {
    mapper
      .registerField("$ref", &Schema::ref)
      .registerField("type", &Schema::type)
      .registerField("format", &Schema::format)
      .registerField("title", &Schema::title)
      .registerField("description", &Schema::description)
      .registerField("nullable", &Schema::nullable)
      .registerField("properties", &Schema::properties)
      .registerField("required", &Schema::required)
      .registerField("additionalProperties", &Schema::additionalProperties)
      .registerField("items", &Schema::items)
      .registerField("enum", &Schema::enumeration)
      .registerField("allOf", &Schema::allOf)
      .registerField("oneOf", &Schema::oneOf)
      .registerField("anyOf", &Schema::anyOf)
      .registerField("minLength", &Schema::minLength)
      .registerField("maxLength", &Schema::maxLength)
      .registerField("pattern", &Schema::pattern)
      .registerField("minimum", &Schema::minimum)
      .registerField("maximum", &Schema::maximum)
      .registerField("exclusiveMinimum", &Schema::exclusiveMinimum)
      .registerField("exclusiveMaximum", &Schema::exclusiveMaximum)
      .registerField("multipleOf", &Schema::multipleOf)
      .registerField("minItems", &Schema::minItems)
      .registerField("maxItems", &Schema::maxItems)
      .registerField("uniqueItems", &Schema::uniqueItems)
      .registerField("minProperties", &Schema::minProperties)
      .registerField("maxProperties", &Schema::maxProperties)
      ;
  }
};

template<>
struct anch::Mapping<MediaTypeObject> {
  static void
  registerObject(anch::ObjectMapper<MediaTypeObject> auto& mapper) {
    mapper
      .registerField("schema", &MediaTypeObject::schema)
      ;
  }
};

template<>
struct anch::Mapping<Parameter> {
  static void
  registerObject(anch::ObjectMapper<Parameter> auto& mapper) {
    mapper
      .registerField("$ref", &Parameter::ref)
      .registerField("name", &Parameter::name)
      .registerField("in", &Parameter::in)
      .registerField("description", &Parameter::description)
      .registerField("required", &Parameter::required)
      .registerField("schema", &Parameter::schema)
      ;
  }
};

template<>
struct anch::Mapping<RequestBody> {
  static void
  registerObject(anch::ObjectMapper<RequestBody> auto& mapper) {
    mapper
      .registerField("$ref", &RequestBody::ref)
      .registerField("description", &RequestBody::description)
      .registerField("required", &RequestBody::required)
      .registerField("content", &RequestBody::content)
      ;
  }
};

template<>
struct anch::Mapping<ResponseObject> {
  static void
  registerObject(anch::ObjectMapper<ResponseObject> auto& mapper) {
    mapper
      .registerField("$ref", &ResponseObject::ref)
      .registerField("description", &ResponseObject::description)
      .registerField("content", &ResponseObject::content)
      ;
  }
};

template<>
struct anch::Mapping<Operation> {
  static void
  registerObject(anch::ObjectMapper<Operation> auto& mapper) {
    mapper
      .registerField("operationId", &Operation::operationId)
      .registerField("summary", &Operation::summary)
      .registerField("description", &Operation::description)
      .registerField("deprecated", &Operation::deprecated)
      .registerField("parameters", &Operation::parameters)
      .registerField("requestBody", &Operation::requestBody)
      .registerField("responses", &Operation::responses)
      ;
  }
};

template<>
struct anch::Mapping<PathItem> {
  static void
  registerObject(anch::ObjectMapper<PathItem> auto& mapper) {
    mapper
      .registerField("parameters", &PathItem::parameters)
      .registerField("get", &PathItem::get)
      .registerField("put", &PathItem::put)
      .registerField("post", &PathItem::post)
      .registerField("delete", &PathItem::del)
      .registerField("patch", &PathItem::patch)
      .registerField("head", &PathItem::head)
      .registerField("options", &PathItem::options)
      .registerField("trace", &PathItem::trace)
      ;
  }
};

template<>
struct anch::Mapping<Info> {
  static void
  registerObject(anch::ObjectMapper<Info> auto& mapper) {
    mapper
      .registerField("title", &Info::title)
      .registerField("version", &Info::version)
      ;
  }
};

template<>
struct anch::Mapping<Components> {
  static void
  registerObject(anch::ObjectMapper<Components> auto& mapper) {
    mapper
      .registerField("schemas", &Components::schemas)
      .registerField("parameters", &Components::parameters)
      .registerField("requestBodies", &Components::requestBodies)
      .registerField("responses", &Components::responses)
      ;
  }
};

template<>
struct anch::Mapping<OpenAPI> {
  static void
  registerObject(anch::ObjectMapper<OpenAPI> auto& mapper) {
    mapper
      .registerField("openapi", &OpenAPI::openapi)
      .registerField("info", &OpenAPI::info)
      .registerField("paths", &OpenAPI::paths)
      .registerField("components", &OpenAPI::components)
      ;
  }
};
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
