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
#include "generator.hpp"

#include <initializer_list>
#include <optional>
#include <utility>

using anch::oas::GeneratorError;
using anch::oas::Operation;
using anch::oas::Parameter;
using anch::oas::PathItem;
using anch::oas::RequestBody;
using anch::oas::ResponseObject;
using anch::oas::Schema;
using anch::oas::detail::Container;
using anch::oas::detail::Field;
using anch::oas::detail::FormMapping;
using anch::oas::detail::Model;
using anch::oas::detail::Generator;
using anch::oas::detail::Op;
using anch::oas::detail::OpParameter;
using anch::oas::detail::ResultKind;
using anch::oas::detail::Type;
using anch::oas::detail::Unsupported;


namespace {

  /*! Parameters references prefix */
  const std::string PARAMETERS_REF = "#/components/parameters/";

  /*! Request bodies references prefix */
  const std::string REQUEST_BODIES_REF = "#/components/requestBodies/";

  /*! Responses references prefix */
  const std::string RESPONSES_REF = "#/components/responses/";

  /*! JSON codec */
  const std::string JSON_CODEC = "anch::rest::Json";

  /*! Text codec */
  const std::string TEXT_CODEC = "anch::rest::Text";

  /*! \c multipart/form-data codec */
  const std::string MULTIPART_CODEC = "anch::rest::Multipart";

  /*! \c application/x-www-form-urlencoded codec */
  const std::string FORM_CODEC = "anch::rest::FormUrlEncoded";

  /*! Types which can be path and query parameters ( \c anch::rest::ParamConverter ) */
  const std::set<std::string> PARAMETER_TYPES = {"std::string", "int32_t", "int64_t", "float", "double", "bool", "anch::UUID"};

  /*! Types passed by value */
  const std::set<std::string> VALUE_TYPES = {"int32_t", "int64_t", "float", "double", "bool"};

  /*! anch-rest success statuses */
  const std::map<int,std::string> SUCCESS_STATUSES = {
    {200, "OK"}, {201, "CREATED"}, {202, "ACCEPTED"}, {203, "NON_AUTHORITATIVE_INFORMATION"}, {204, "NO_CONTENT"},
    {205, "RESET_CONTENT"}, {206, "PARTIAL_CONTENT"}, {207, "MULTI_STATUS"}, {208, "ALREADY_REPORTED"}, {226, "IM_USED"}
  };

  /*!
   * Codec of media type
   *
   * \param mediaType the media type (parameters are ignored)
   *
   * \return the codec, empty when media type is not supported
   */
  std::string
  codecOf(const std::string& mediaType) {
    std::string type = mediaType.substr(0, mediaType.find(';'));
    while(!type.empty() && type.back() == ' ') {
      type.pop_back();
    }
    std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if(type == "application/json") {
      return JSON_CODEC;
    }
    if(type == "text/plain") {
      return TEXT_CODEC;
    }
    if(type == "multipart/form-data") {
      return MULTIPART_CODEC;
    }
    if(type == "application/x-www-form-urlencoded") {
      return FORM_CODEC;
    }
    return std::string();
  }

  /*!
   * Check if two schemas are the same: same object or same reference
   */
  bool
  sameSchema(const Schema* left, const Schema* right) {
    return left == right || (left != nullptr && right != nullptr && left->ref.has_value() && left->ref == right->ref);
  }

  /*!
   * Form body media type
   */
  struct FormContent {
    /*! Media type */
    std::string mediaType;
    /*! Codec */
    std::string codec;
    /*! Schema ( \c nullptr without schema) */
    const Schema* schema;
  };

  /*!
   * Success status code ( \c 2xx , \c 2XX is \c 200 )
   *
   * \param code the response code
   *
   * \return the status code, \c 0 when it is not a success status
   */
  int
  successCode(const std::string& code) {
    if(code == "2XX" || code == "2xx") {
      return 200;
    }
    if(code.size() != 3 || code[0] != '2' || !anch::oas::detail::isDigit(code[1]) || !anch::oas::detail::isDigit(code[2])) {
      return 0;
    }
    return std::stoi(code);
  }

  /*!
   * Name of a reference
   *
   * \param ref the reference
   * \param prefix the expected prefix
   *
   * \return the referenced name
   *
   * \throw Unsupported reference outside prefix
   */
  std::string
  referenceName(const std::string& ref, const std::string& prefix) {
    if(!ref.starts_with(prefix)) {
      throw Unsupported{"reference '" + ref + "' (only '" + prefix + "' references are supported)"};
    }
    return anch::oas::detail::decodePointer(ref.substr(prefix.size()));
  }

}

const Parameter&
Generator::parameter(const Parameter& param) const {
  if(!param.ref.has_value()) {
    return param;
  }
  std::string name = referenceName(param.ref.value(), PARAMETERS_REF);
  auto iter = _api.components.parameters.find(name);
  if(iter == _api.components.parameters.end()) {
    throw GeneratorError("Unknown reference '" + param.ref.value() + "'");
  }
  return iter->second;
}

std::string
Generator::apiName() const {
  if(!_options.api.empty()) {
    return _options.api;
  }
  std::string name = pascal(_api.info.title);
  return (name.empty() || name == "_") ? std::string("Api") : name;
}

void
Generator::collectOperations() {
  for(const auto& [path, item] : _api.paths) {
    for(const auto& [method, operation] : std::initializer_list<std::pair<std::string,const std::optional<Operation>*>>{
	{"get", &item.get}, {"put", &item.put}, {"post", &item.post}, {"delete", &item.del}, {"patch", &item.patch},
	{"head", &item.head}, {"options", &item.options}, {"trace", &item.trace}}) {
      if(!operation->has_value()) {
	continue;
      }
      std::string where = "paths." + path + "." + method;
      if(method == "head" || method == "options" || method == "trace") {
	warn(where, "HEAD, OPTIONS and TRACE methods (HEAD is served by GET routes)");
	continue;
      }
      try {
	_operations.push_back(buildOperation(method, path, item, operation->value()));
	_operationNames.insert(_operations.back().name);
      } catch(const Unsupported& error) {
	warn(where, error.reason);
      } catch(const GeneratorError& error) {
	throw GeneratorError(where + ": " + error.what());
      }
    }
  }
}

Op
Generator::buildOperation(const std::string& method, const std::string& path, const PathItem& item, const Operation& operation) {
  Op op;
  op.method = (method == "delete") ? "del" : method;
  op.path = path;
  op.deprecated = operation.deprecated.value_or(false);
  // Path pattern +
  std::vector<std::string> pathNames;
  std::string derived = method; // name derived from method and path when operationId is missing
  std::size_t begin = 0;
  while(begin < path.size()) {
    std::size_t end = path.find('/', begin + 1);
    std::string segment = path.substr(begin + 1, (end == std::string::npos ? path.size() : end) - begin - 1);
    if(segment.starts_with('{') && segment.ends_with('}') && segment.size() > 2
       && segment.find_first_of("{}", 1) == segment.size() - 1) {
      pathNames.push_back(segment.substr(1, segment.size() - 2));
      derived += "By" + pascal(pathNames.back());
    } else if(segment.find_first_of("{}") != std::string::npos) {
      throw Unsupported{"path segment '" + segment + "' (anch-rest parameters are whole segments)"};
    } else if(!segment.empty()) {
      derived += pascal(segment);
    }
    if(end == std::string::npos) {
      break;
    }
    begin = end;
  }
  // Path pattern -
  // Name and documentation +
  // camelCase name ( \c get-pet and \c get_pet → \c getPet ), C++ keywords and macros get a \c _ suffix
  std::string name = pascal(operation.operationId.value_or(derived));
  if(!name.empty() && isUpper(name[0])) {
    name[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(name[0])));
  }
  name = memberName(name);
  while(_operationNames.contains(name)) {
    name += '_';
  }
  op.name = name;
  if(!operation.operationId.has_value()) {
    _warnings.push_back("paths." + path + "." + method + ": operationId is missing, method is named " + name);
  }
  op.description = document(operation.summary.value_or(""), operation.description.value_or(""));
  // Name and documentation -
  // Parameters (operation parameters override path item parameters) +
  std::vector<const Parameter*> parameters;
  auto addParameter = [&](const Parameter& declared) {
    const Parameter& param = parameter(declared);
    for(const Parameter*& found : parameters) {
      if(found->name == param.name && found->in == param.in) {
	found = &param;
	return;
      }
    }
    parameters.push_back(&param);
  };
  for(const Parameter& param : item.parameters) {
    addParameter(param);
  }
  for(const Parameter& param : operation.parameters) {
    addParameter(param);
  }
  std::set<std::string> args = {"request", "service", "call", "body"};
  auto argName = [&args](const std::string& base) {
    std::string res = memberName(base);
    while(args.contains(res)) {
      res += '_';
    }
    args.insert(res);
    return res;
  };
  auto parameterType = [&](const Parameter& param) {
    if(!param.schema.has_value()) {
      throw Unsupported{param.in + " parameter '" + param.name + "' without schema"};
    }
    Type type = resolve(param.schema.value(), pascal(name) + pascal(param.name));
    if(type.container != Container::NONE || type.model || type.enumeration || !PARAMETER_TYPES.contains(type.cpp)) {
      throw Unsupported{param.in + " parameter '" + param.name + "' of type " + cppType(type, false, false)
			+ " (anch-rest parameters are strings, numbers, booleans and UUID)"};
    }
    return type;
  };
  for(const std::string& pathName : pathNames) {
    auto found = std::find_if(parameters.begin(), parameters.end(), [&pathName](const Parameter* param) {
      return param->in == "path" && param->name == pathName;
    });
    if(found == parameters.end()) {
      throw Unsupported{"path parameter '" + pathName + "' is not declared"};
    }
    op.pathParameters.push_back(OpParameter{pathName, argName(pathName), parameterType(**found), true, (*found)->description.value_or("")});
  }
  std::vector<OpParameter> optionalQuery;
  for(const Parameter* param : parameters) {
    if(param->in == "query") {
      OpParameter query{param->name, argName(param->name), parameterType(*param), param->required.value_or(false), param->description.value_or("")};
      (query.required ? op.queryParameters : optionalQuery).push_back(std::move(query));
    } else if(param->in == "header" || param->in == "cookie") {
      constraintWarning("paths." + path + "." + method, param->in + " parameter '" + param->name + "' is not generated");
    } else if(param->in == "path" && std::find(pathNames.begin(), pathNames.end(), param->name) == pathNames.end()) {
      throw Unsupported{"path parameter '" + param->name + "' is not in path"};
    }
  }
  op.queryParameters.insert(op.queryParameters.end(), optionalQuery.begin(), optionalQuery.end());
  // Parameters -
  // Request body +
  if(operation.requestBody.has_value()) {
    const RequestBody* body = &operation.requestBody.value();
    if(body->ref.has_value()) {
      std::string bodyName = referenceName(body->ref.value(), REQUEST_BODIES_REF);
      auto found = _api.components.requestBodies.find(bodyName);
      if(found == _api.components.requestBodies.end()) {
	throw GeneratorError("Unknown reference '" + body->ref.value() + "'");
      }
      body = &found->second;
    }
    const Schema* schema = nullptr;
    bool text = false;
    std::vector<FormContent> forms; // multipart/form-data first
    for(const auto& [mediaType, content] : body->content) {
      std::string codec = codecOf(mediaType);
      if(codec == JSON_CODEC && content.schema.has_value()) {
	schema = &content.schema.value();
	op.consumes.insert(op.consumes.begin(), JSON_CODEC);
      } else if(codec == TEXT_CODEC) {
	text = true;
	if(schema == nullptr && content.schema.has_value()) {
	  schema = &content.schema.value();
	}
      } else if(codec == MULTIPART_CODEC || codec == FORM_CODEC) {
	FormContent form{mediaType, codec, content.schema.has_value() ? &content.schema.value() : nullptr};
	forms.insert(codec == MULTIPART_CODEC ? forms.begin() : forms.end(), std::move(form));
      }
    }
    const std::string where = "paths." + path + "." + method;
    Type type;
    if(schema == nullptr && !forms.empty()) {
      // Form body: model of the first form media type with schema, anch::rest::Form without schema
      for(const FormContent& form : forms) {
	if(form.schema != nullptr) {
	  schema = form.schema;
	  break;
	}
      }
      if(schema == nullptr) {
	type.cpp = "anch::rest::Form";
	_rawForm = true;
      } else {
	type = resolve(*schema, pascal(name) + "Request");
	if(!type.model || type.container != Container::NONE) {
	  throw Unsupported{"form request body which is not an object"};
	}
      }
    } else {
      if(schema == nullptr) {
	throw Unsupported{"request body media types (only application/json, text/plain, multipart/form-data and "
			  "application/x-www-form-urlencoded are supported)"};
      }
      type = resolve(*schema, pascal(name) + "Request");
      if(text && type.cpp == "std::string" && type.container == Container::NONE && !type.model && !type.enumeration) {
	op.consumes.push_back(TEXT_CODEC);
      }
      if(op.consumes.empty()) {
	throw Unsupported{"text/plain request body which is not a string"};
      }
    }
    // Form codecs with the same body type
    for(const FormContent& form : forms) {
      const bool mapped = type.model && type.container == Container::NONE;
      if(form.schema == nullptr ? (type.cpp == "anch::rest::Form" || mapped) : (mapped && sameSchema(form.schema, schema))) {
	op.consumes.push_back(form.codec);
	_formCodecs.insert(form.codec);
	if(mapped) {
	  FormMapping& mapping = _forms[type.cpp];
	  mapping.multipart = mapping.multipart || form.codec == MULTIPART_CODEC;
	}
      } else {
	_warnings.push_back(where + ": " + form.mediaType + " request body is ignored (its schema is not the one of the other media types)");
      }
    }
    op.body = type;
    op.bodyArg = "body";
    op.bodyDescription = body->description.value_or("");
  }
  // Request body -
  // Responses +
  std::vector<std::pair<int,const ResponseObject*>> successes;
  bool notFound = false;
  for(const auto& [code, declared] : operation.responses) {
    const ResponseObject* response = &declared;
    if(response->ref.has_value()) {
      std::string responseName = referenceName(response->ref.value(), RESPONSES_REF);
      auto found = _api.components.responses.find(responseName);
      if(found == _api.components.responses.end()) {
	throw GeneratorError("Unknown reference '" + response->ref.value() + "'");
      }
      response = &found->second;
    }
    if(code == "404") {
      notFound = true;
    }
    int success = successCode(code);
    if(success != 0) {
      successes.emplace_back(success, response);
    }
  }
  std::sort(successes.begin(), successes.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
  std::size_t withContent = static_cast<std::size_t>(std::count_if(successes.begin(), successes.end(), [](const auto& success) {
    return !success.second->content.empty();
  }));
  if(withContent == 0) { // no content: void
    op.result = ResultKind::VOID;
    if(!successes.empty() && successes.front().first != 204) {
      op.status = SUCCESS_STATUSES.at(successes.front().first);
    }
  } else if(successes.size() > 1) { // several success responses: raw response
    op.result = ResultKind::RESPONSE;
  } else {
    const auto& [code, response] = successes.front();
    const Schema* schema = nullptr;
    bool text = false;
    for(const auto& [mediaType, content] : response->content) {
      std::string codec = codecOf(mediaType);
      if(codec == JSON_CODEC && content.schema.has_value()) {
	schema = &content.schema.value();
	op.produces.insert(op.produces.begin(), JSON_CODEC);
      } else if(codec == TEXT_CODEC) {
	text = true;
	if(schema == nullptr && content.schema.has_value()) {
	  schema = &content.schema.value();
	}
      }
    }
    if(schema == nullptr) { // other media types: raw response
      op.result = ResultKind::RESPONSE;
      op.produces.clear();
    } else {
      Type type = resolve(*schema, pascal(name) + "Response");
      if(text && type.cpp == "std::string" && type.container == Container::NONE && !type.model && !type.enumeration) {
	op.produces.push_back(TEXT_CODEC);
      }
      if(op.produces.empty()) {
	op.result = ResultKind::RESPONSE;
      } else {
	op.result = notFound ? ResultKind::OPTIONAL : ResultKind::VALUE;
	op.resultType = type;
	if(code != 200) {
	  op.status = SUCCESS_STATUSES.at(code);
	}
      }
    }
  }
  // Responses -
  return op;
}

std::string
Generator::argumentType(const Type& type, bool optional) const {
  std::string cpp = cppType(type, false, false);
  if(optional) {
    return "const std::optional<" + cpp + ">&";
  }
  if(type.container == Container::NONE && VALUE_TYPES.contains(type.cpp)) {
    return cpp;
  }
  return "const " + cpp + "&";
}

std::string
Generator::signature(const Op& op, bool defaults) const {
  std::vector<std::string> args;
  for(const OpParameter& param : op.pathParameters) {
    args.push_back(argumentType(param.type, false) + " " + param.arg);
  }
  if(op.body.has_value()) {
    args.push_back(argumentType(op.body.value(), false) + " " + op.bodyArg);
  }
  for(const OpParameter& param : op.queryParameters) {
    args.push_back(argumentType(param.type, !param.required) + " " + param.arg + ((defaults && !param.required) ? " = std::nullopt" : ""));
  }
  return op.name + "(" + join(args, ", ") + ")";
}

std::string
Generator::resultType(const Op& op) const {
  switch(op.result) {
  case ResultKind::VOID:
    return "void";
  case ResultKind::VALUE:
    return cppType(op.resultType, false, false);
  case ResultKind::OPTIONAL:
    return "std::optional<" + cppType(op.resultType, false, false) + ">";
  case ResultKind::RESPONSE:
    break;
  }
  return "anch::rest::Response";
}

void
Generator::writeOperationDoc(std::ostream& out, const Op& op, const std::string& indent) const {
  std::string doc = document(op.description, op.method == "del" ? "DELETE " + op.path : [&op]() {
    std::string method = op.method;
    std::transform(method.begin(), method.end(), method.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return method + " " + op.path;
  }());
  if(op.deprecated) {
    doc += "\n\n\\deprecated";
  }
  std::vector<std::string> params;
  for(const OpParameter& param : op.pathParameters) {
    params.push_back("\\param " + param.arg + " " + (param.description.empty() ? "path parameter " + param.name : param.description));
  }
  if(op.body.has_value()) {
    params.push_back("\\param " + op.bodyArg + " " + (op.bodyDescription.empty() ? std::string("the request body") : op.bodyDescription));
  }
  for(const OpParameter& param : op.queryParameters) {
    params.push_back("\\param " + param.arg + " " + (param.description.empty() ? "query parameter " + param.name : param.description)
		     + (param.required ? "" : " (optional)"));
  }
  if(!params.empty()) {
    doc += "\n\n" + join(params, "\n");
  }
  switch(op.result) {
  case ResultKind::VALUE:
    doc += "\n\n\\return the result";
    break;
  case ResultKind::OPTIONAL:
    doc += "\n\n\\return the result, empty for \\c 404";
    break;
  case ResultKind::RESPONSE:
    doc += "\n\n\\return the response";
    break;
  case ResultKind::VOID:
    break;
  }
  writeDoc(out, indent, doc);
}

std::string
Generator::formCodecInclude(bool multipart) const {
  if(multipart) {
    return _formCodecs.contains(MULTIPART_CODEC) ? "#include \"rest/multipartCodec.hpp\"\n" : "";
  }
  return _formCodecs.contains(FORM_CODEC) ? "#include \"rest/formCodec.hpp\"\n" : "";
}

void
Generator::buildFormMappings() {
  for(auto& [name, mapping] : _forms) {
    auto found = _models.find(name);
    if(found == _models.end()) {
      continue;
    }
    const Model& model = found->second;
    for(const Field& field : model.fields) {
      const std::string key = literal(field.json);
      const std::string member = "value." + field.member;
      Type element = field.type;
      element.cpp = qualified(field.type);
      const std::string cpp = cppType(element, false, field.pointer);
      const bool scalar = field.type.container != Container::MAP && !field.type.model && !field.type.enumeration
	&& PARAMETER_TYPES.contains(field.type.cpp);
      if(field.pointer) {
	warn(name + "." + field.json, "recursive model in form body");
	continue;
      }
      if(field.part) {
	mapping.write.push_back("anch::rest::addFormFile(form, " + key + ", " + member + ");");
	if(field.type.container == Container::VECTOR) {
	  mapping.read.push_back(member + " = anch::rest::formFiles(form, " + key + ");");
	} else {
	  mapping.read.push_back(member + " = anch::rest::" + (field.optional ? "formFile" : "requiredFormFile") + "(form, " + key + ");");
	}
      } else if(scalar) {
	mapping.write.push_back("anch::rest::addFormField(form, " + key + ", " + member + ");");
	if(field.type.container == Container::VECTOR) {
	  mapping.read.push_back(member + " = anch::rest::formFields<" + field.type.cpp + ">(form, " + key + ");");
	} else {
	  mapping.read.push_back(member + " = anch::rest::" + (field.optional ? "formField" : "requiredFormField") + "<" + field.type.cpp
				 + ">(form, " + key + ");");
	}
      } else if(mapping.multipart && (field.type.model || field.type.container != Container::NONE)) {
	// Objects and arrays: application/json parts
	mapping.side = true;
	if(field.optional) {
	  mapping.write.push_back("if(" + member + ") {\n  anch::rest::addJsonFormField(form, " + key + ", *" + member + ", side);\n}");
	  mapping.read.push_back(member + " = anch::rest::jsonFormField<" + cpp + ">(form, " + key + ", side);");
	} else {
	  mapping.write.push_back("anch::rest::addJsonFormField(form, " + key + ", " + member + ", side);");
	  if(field.required) {
	    mapping.read.push_back(member + " = anch::rest::requiredJsonFormField<" + cpp + ">(form, " + key + ", side);");
	  } else {
	    mapping.read.push_back(member + " = anch::rest::jsonFormField<" + cpp + ">(form, " + key + ", side).value_or(" + cpp + "());");
	  }
	}
      } else {
	warn(name + "." + field.json, "form field of type " + cppType(field.type, false, false) + " (form fields are strings, numbers, "
	     "booleans, UUID and files, multipart/form-data objects and arrays are JSON parts)");
      }
    }
    if(_options.constraints) {
      mapping.read.push_back("anch::rest::checkFormConstraints(value, side);");
      mapping.side = true;
    }
  }
}

void
Generator::writeFormsHeader(std::ostream& out) const {
  writeBanner(out);
  out << "#pragma once\n\n"
      << "#include \"" << _options.name << ".hpp\"\n\n"
      << "#include \"rest/codec.hpp\"\n"
      << "#include \"rest/form.hpp\"\n"
      << "#include \"rest/formMapper.hpp\"\n";
  if(_forms.empty()) {
    out << "\n// No form body model\n";
    return;
  }
  out << "\nnamespace anch::rest {\n";
  for(const auto& [name, mapping] : _forms) {
    if(!_models.contains(name)) {
      continue;
    }
    const std::string type = "::" + _options.ns + "::" + name;
    out << '\n';
    writeDoc(out, "  ", "\\c " + type + " form mapping (" + (mapping.multipart ? "multipart/form-data" : "application/x-www-form-urlencoded")
	     + " body)");
    out << "  template<>\n"
	<< "  struct FormMapper<" << type << "> {\n"
	<< "    /*!\n"
	<< "     * Write value in form\n"
	<< "     *\n"
	<< "     * \\param value the value\n"
	<< "     * \\param form the form\n"
	<< "     * \\param side the side which writes form\n"
	<< "     */\n"
	<< "    static void toForm(const " << type << "& value, anch::rest::Form& form, anch::rest::Side side);\n\n"
	<< "    /*!\n"
	<< "     * Read value from form\n"
	<< "     *\n"
	<< "     * \\param value the value to fill\n"
	<< "     * \\param form the form\n"
	<< "     * \\param side the side which reads form\n"
	<< "     *\n"
	<< "     * \\throw anch::rest::HttpException \\c 400 when a required field is missing or a value is invalid\n"
	<< "     */\n"
	<< "    static void fromForm(" << type << "& value, const anch::rest::Form& form, anch::rest::Side side);\n"
	<< "  };\n";
  }
  out << "\n}  // anch::rest\n";
}

void
Generator::writeFormsSource(std::ostream& out) const {
  writeBanner(out);
  out << "#include \"" << _options.name << "-forms.hpp\"\n";
  if(_forms.empty()) {
    out << "\n// No form body model\n";
    return;
  }
  out << "\n#include <string>\n"
      << "#include <vector>\n\n"
      << "#include \"rest/httpException.hpp\"\n"
      << "#include \"rest/jsonCodec.hpp\"\n"
      << "#include \"rest/paramConverter.hpp\"\n\n"
      << "namespace anch::rest {\n";
  auto writeBody = [&out](const std::vector<std::string>& statements) {
    for(const std::string& statement : statements) {
      std::istringstream lines(statement);
      std::string line;
      while(std::getline(lines, line)) {
	out << "    " << line << '\n';
      }
    }
  };
  auto usesSide = [](const std::vector<std::string>& statements) {
    return std::any_of(statements.begin(), statements.end(), [](const std::string& statement) {
      return statement.find("side)") != std::string::npos;
    });
  };
  for(const auto& [name, mapping] : _forms) {
    if(!_models.contains(name)) {
      continue;
    }
    const std::string type = "::" + _options.ns + "::" + name;
    const std::string unusedWrite = mapping.write.empty() ? "[[maybe_unused]] " : "";
    const std::string unusedRead = mapping.read.empty() ? "[[maybe_unused]] " : "";
    out << "\n  // " << name << " +\n"
	<< "  void\n"
	<< "  FormMapper<" << type << ">::toForm(" << unusedWrite << "const " << type << "& value, " << unusedWrite
	<< "anch::rest::Form& form, " << (usesSide(mapping.write) ? "" : "[[maybe_unused]] ") << "anch::rest::Side side) {\n";
    writeBody(mapping.write);
    out << "  }\n\n"
	<< "  void\n"
	<< "  FormMapper<" << type << ">::fromForm(" << unusedRead << type << "& value, " << unusedRead
	<< "const anch::rest::Form& form, " << (usesSide(mapping.read) ? "" : "[[maybe_unused]] ") << "anch::rest::Side side) {\n";
    writeBody(mapping.read);
    out << "  }\n"
	<< "  // " << name << " -\n";
  }
  out << "\n}  // anch::rest\n";
}

void
Generator::writeServerHeader(std::ostream& out) const {
  std::string service = apiName() + "Service";
  writeBanner(out);
  out << "#pragma once\n\n"
      << "#include <cstdint>\n"
      << "#include <map>\n"
      << "#include <optional>\n"
      << "#include <string>\n"
      << "#include <vector>\n\n"
      << "#include \"" << _options.name << ".hpp\"\n"
      << "#include \"" << _options.name << "-forms.hpp\"\n\n"
      << "#include \"rest/api.hpp\"\n"
      << "#include \"rest/response.hpp\"\n\n"
      << "namespace " << _options.ns << " {\n\n";
  writeDoc(out, "  ", commentSafe(_api.info.title) + " service: endpoints are registered in anch-rest API by \\ref registerEndpoints");
  out << "  class " << service << " {\n"
      << "  public:\n"
      << "    /*! \\ref " << service << " destructor */\n"
      << "    virtual ~" << service << "() = default;\n";
  for(const Op& op : _operations) {
    out << '\n';
    writeOperationDoc(out, op, "    ");
    out << "    virtual " << resultType(op) << ' ' << signature(op, false) << " = 0;\n";
  }
  out << "  };\n\n";
  writeDoc(out, "  ", "Register " + service + " endpoints\n\n\\param api the anch-rest API\n\\param service the service (it has to "
	   "outlive the API)");
  out << "  void registerEndpoints(anch::rest::Api& api, " << service << "& service);\n\n"
      << "}  // " << _options.ns << "\n";
}

void
Generator::writeServerSource(std::ostream& out) const {
  std::string service = apiName() + "Service";
  writeBanner(out);
  out << "#include \"" << _options.name << "-server.hpp\"\n\n"
      << "#include \"rest/body.hpp\"\n"
      << "#include \"rest/codec.hpp\"\n"
      << formCodecInclude(false)
      << "#include \"rest/jsonCodec.hpp\"\n"
      << formCodecInclude(true)
      << "#include \"rest/paramConverter.hpp\"\n"
      << "#include \"rest/request.hpp\"\n"
      << "#include \"rest/status.hpp\"\n\n"
      << "namespace " << _options.ns << " {\n\n"
      << "  void\n"
      << "  registerEndpoints(" << (_operations.empty() ? "[[maybe_unused]] " : "") << "anch::rest::Api& api, "
      << (_operations.empty() ? "[[maybe_unused]] " : "") << service << "& service) {\n";
  for(const Op& op : _operations) {
    std::vector<std::string> params;
    std::vector<std::string> args;
    if(!op.queryParameters.empty()) {
      params.push_back("const anch::rest::Request& request");
    }
    for(const OpParameter& param : op.pathParameters) {
      params.push_back(cppType(param.type, false, false) + " " + param.arg);
      args.push_back(param.arg);
    }
    if(op.body.has_value()) {
      params.push_back("anch::rest::Body<" + cppType(op.body.value(), false, false) + "> " + op.bodyArg);
      args.push_back("*" + op.bodyArg);
    }
    for(const OpParameter& param : op.queryParameters) {
      args.push_back(std::string("anch::rest::") + (param.required ? "requiredQueryParameter" : "queryParameter") + "<"
		     + cppType(param.type, false, false) + ">(request, " + literal(param.name) + ")");
    }
    out << "    // " << op.name << " +\n"
	<< "    api." << op.method << '(' << literal(op.path) << ", [&service](" << join(params, ", ") << ") -> " << resultType(op) << " {\n"
	<< "      " << (op.result == ResultKind::VOID ? "" : "return ") << "service." << op.name << '(' << join(args, ", ") << ");\n"
	<< "    })";
    for(const std::string& codec : op.consumes) {
      out << ".consumes<" << codec << ">()";
    }
    for(const std::string& codec : op.produces) {
      out << ".produces<" << codec << ">()";
    }
    if(!op.status.empty()) {
      out << ".status(anch::rest::Status::" << op.status << ")";
    }
    out << ";\n"
	<< "    // " << op.name << " -\n";
  }
  out << "  }\n\n"
      << "}  // " << _options.ns << "\n";
}

void
Generator::writeClientHeader(std::ostream& out) const {
  std::string client = apiName() + "Client";
  writeBanner(out);
  out << "#pragma once\n\n"
      << "#include <cstdint>\n"
      << "#include <map>\n"
      << "#include <optional>\n"
      << "#include <string>\n"
      << "#include <vector>\n\n"
      << "#include \"" << _options.name << ".hpp\"\n"
      << "#include \"" << _options.name << "-forms.hpp\"\n\n"
      << "#include \"rest/client.hpp\"\n"
      << "#include \"rest/response.hpp\"\n\n"
      << "namespace " << _options.ns << " {\n\n";
  writeDoc(out, "  ", commentSafe(_api.info.title) + " client: anch-rest client (transport, base URL, filters) is configured by "
	   "the caller.\nResponses with status greater or equal to 400 raise anch::rest::HttpError (except 404 for optional "
	   "results).");
  out << "  class " << client << " {\n"
      << "  private:\n"
      << "    /*! anch-rest client */\n"
      << "    anch::rest::Client& _client;\n\n"
      << "  public:\n"
      << "    /*!\n"
      << "     * \\ref " << client << " constructor\n"
      << "     *\n"
      << "     * \\param client the anch-rest client (it has to outlive this client)\n"
      << "     */\n"
      << "    explicit " << client << "(anch::rest::Client& client);\n";
  for(const Op& op : _operations) {
    out << '\n';
    writeOperationDoc(out, op, "    ");
    out << "    " << resultType(op) << ' ' << signature(op, true) << ";\n";
  }
  out << "  };\n\n"
      << "}  // " << _options.ns << "\n";
}

void
Generator::writeClientSource(std::ostream& out) const {
  std::string client = apiName() + "Client";
  writeBanner(out);
  out << "#include \"" << _options.name << "-client.hpp\"\n\n"
      << "#include <utility>\n\n"
      << "#include \"rest/codec.hpp\"\n"
      << formCodecInclude(false)
      << "#include \"rest/httpError.hpp\"\n"
      << "#include \"rest/jsonCodec.hpp\"\n"
      << formCodecInclude(true)
      << "#include \"rest/status.hpp\"\n\n"
      << "namespace " << _options.ns << " {\n\n"
      << "  " << client << "::" << client << "(anch::rest::Client& client): _client(client) {\n"
      << "    // Nothing to do\n"
      << "  }\n";
  for(const Op& op : _operations) {
    std::vector<std::string> pathArgs = {literal(op.path)};
    for(const OpParameter& param : op.pathParameters) {
      pathArgs.push_back(param.arg);
    }
    out << "\n  " << resultType(op) << "\n"
	<< "  " << client << "::" << signature(op, false) << " {\n"
	<< "    anch::rest::Call<> call = _client." << op.method << '(' << join(pathArgs, ", ") << ");\n";
    for(const OpParameter& param : op.queryParameters) {
      if(param.required) {
	out << "    call.query(" << literal(param.name) << ", " << param.arg << ");\n";
      } else {
	out << "    if(" << param.arg << ") {\n"
	    << "      call.query(" << literal(param.name) << ", *" << param.arg << ");\n"
	    << "    }\n";
      }
    }
    if(op.body.has_value()) {
      out << "    call.body<" << op.consumes.front() << ">(" << op.bodyArg << ");\n";
    }
    std::string accept;
    for(const std::string& codec : op.produces) {
      accept += ".accept<" + codec + ">()";
    }
    switch(op.result) {
    case ResultKind::VOID:
      out << "    std::move(call).execute();\n";
      break;
    case ResultKind::VALUE:
      out << "    return std::move(call)" << accept << ".as<" << cppType(op.resultType, false, false) << ">();\n";
      break;
    case ResultKind::OPTIONAL:
      out << "    try {\n"
	  << "      return std::move(call)" << accept << ".as<" << cppType(op.resultType, false, false) << ">();\n"
	  << "    } catch(const anch::rest::HttpError& error) {\n"
	  << "      if(error.getStatus() == anch::rest::Status::NOT_FOUND) {\n"
	  << "        return std::nullopt;\n"
	  << "      }\n"
	  << "      throw;\n"
	  << "    }\n";
      break;
    case ResultKind::RESPONSE:
      out << "    return std::move(call).send();\n";
      break;
    }
    out << "  }\n";
  }
  out << "\n}  // " << _options.ns << "\n";
}
