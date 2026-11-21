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

// Internal header: models and endpoints generator (not installed)

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <map>
#include <ostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "oas/modelGenerator.hpp"
#include "oas/spec.hpp"

namespace anch::oas::detail {

  /*! Supported references prefix */
  inline const std::string SCHEMAS_REF = "#/components/schemas/";

  /*! C++ keywords */
  inline const std::set<std::string> KEYWORDS = {
    "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char",
    "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const", "consteval", "constexpr", "constinit",
    "const_cast", "continue", "co_await", "co_return", "co_yield", "decltype", "default", "delete", "do", "double",
    "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if",
    "inline", "int", "long", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or",
    "or_eq", "private", "protected", "public", "register", "reinterpret_cast", "requires", "return", "short", "signed",
    "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "template", "this", "thread_local", "throw",
    "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile",
    "wchar_t", "while", "xor", "xor_eq"
  };

  /*! Object-like macros which can not be used as identifiers */
  inline const std::set<std::string> MACROS = {
    "NULL", "EOF", "TRUE", "FALSE", "EDOM", "ERANGE", "EILSEQ", "errno", "stdin", "stdout", "stderr", "linux", "unix",
    "major", "minor"
  };

  /*!
   * Unsupported schema: the model or the property is not generated
   */
  struct Unsupported {
    /*! Why the schema is not supported */
    std::string reason;
  };

  /*!
   * Property which is not generated (warning already reported)
   */
  struct Skipped {
    /*! Why the property is not generated */
    std::string reason;
  };

  /*! Container of a type */
  enum class Container {
    /*! Single value */
    NONE,
    /*! \c std::vector */
    VECTOR,
    /*! \c std::map with \c std::string keys */
    MAP
  };

  /*!
   * Resolved C++ type
   */
  struct Type {
    /*! Element C++ type (model, enumeration or primitive type) */
    std::string cpp;
    /*! Element is a generated model */
    bool model = false;
    /*! Element is a generated enumeration */
    bool enumeration = false;
    /*! Container */
    Container container = Container::NONE;
    /*! \c null is allowed */
    bool nullable = false;
    /*! Remark for documentation (format, allowed values) */
    std::string remark;
  };

  /*!
   * Generated type (model or enumeration)
   *
   * \param cpp the type name
   * \param model \c true for model, \c false for enumeration
   * \param nullable \c null is allowed
   *
   * \return the type
   */
  inline
  Type
  generatedType(const std::string& cpp, bool model, bool nullable) {
    Type type;
    type.cpp = cpp;
    type.model = model;
    type.enumeration = !model;
    type.nullable = nullable;
    return type;
  }

  /*!
   * Model property with the model which declares it (\c allOf inheritance)
   */
  struct Property {
    /*! Property schema */
    const Schema* schema = nullptr;
    /*! Name of the model which declares the property */
    std::string owner;
  };

  /*!
   * Model properties after \c allOf merge
   */
  struct Composite {
    /*! Properties indexed by their JSON names */
    std::map<std::string,Property> properties;
    /*! Required properties */
    std::set<std::string> required;
    /*! Additional properties */
    const anch::oas::AdditionalProperties* additional = nullptr;
  };

  /*!
   * Model field
   */
  struct Field {
    /*! JSON property name */
    std::string json;
    /*! C++ member name */
    std::string member;
    /*! Type */
    Type type;
    /*! \c std::optional field */
    bool optional = false;
    /*! \c std::shared_ptr field (recursive model) */
    bool pointer = false;
    /*! File ( \c anch::rest::Part ) of a \c multipart/form-data body model: not mapped in JSON */
    bool part = false;
    /*! Required property ( \c anch::val::required ) */
    bool required = false;
    /*! Value constraints (C++ expressions) */
    std::vector<std::string> constraints;
    /*! Description */
    std::string description;
  };

  /*!
   * Model (structure)
   */
  struct Model {
    /*! Name */
    std::string name;
    /*! Description */
    std::string description;
    /*! Fields */
    std::vector<Field> fields;
    /*! Properties which have not been generated (with reason) */
    std::vector<std::string> skipped;
  };

  /*!
   * Enumeration
   */
  struct Enumeration {
    /*! Name */
    std::string name;
    /*! Description */
    std::string description;
    /*! Values: JSON value and C++ identifier */
    std::vector<std::pair<std::string,std::string>> values;
  };

  /*!
   * Alias ( \c using )
   */
  struct Alias {
    /*! Name */
    std::string name;
    /*! Description */
    std::string description;
    /*! Type */
    Type type;
  };

  inline
  bool
  isAlnum(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
  }

  inline
  bool
  isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
  }

  inline
  bool
  isUpper(char c) {
    return std::isupper(static_cast<unsigned char>(c)) != 0;
  }

  inline
  bool
  isLower(char c) {
    return std::islower(static_cast<unsigned char>(c)) != 0;
  }

  inline
  char
  toUpper(char c) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
  }

  /*!
   * Type name: split on non alphanumeric characters and capitalize each word ( \c pet-store → \c PetStore )
   *
   * \param name the schema or property name
   *
   * \return the type name
   */
  inline
  std::string
  pascal(const std::string& name) {
    std::string res;
    bool upper = true;
    for(char c : name) {
      if(!isAlnum(c)) {
	upper = true;
	continue;
      }
      res += upper ? toUpper(c) : c;
      upper = false;
    }
    if(res.empty()) {
      res = "Model";
    } else if(isDigit(res[0])) {
      res = "M" + res;
    }
    return res;
  }

  /*!
   * Member name: invalid characters are replaced by \c _ , keywords and macros get a \c _ suffix
   *
   * \param name the property name
   *
   * \return the member name
   */
  inline
  std::string
  memberName(const std::string& name) {
    std::string res;
    for(char c : name) {
      res += (isAlnum(c) || c == '_') ? c : '_';
    }
    if(res.empty() || isDigit(res[0])) {
      res = "_" + res;
    } else if(res.size() > 1 && res[0] == '_' && (res[1] == '_' || isUpper(res[1]))) { // reserved identifiers
      res = "m" + res;
    }
    if(KEYWORDS.contains(res) || MACROS.contains(res)) {
      res += '_';
    }
    return res;
  }

  /*!
   * Enumeration value name: upper case words separated by \c _ ( \c in-stock and \c inStock → \c IN_STOCK )
   *
   * \param value the enumeration value
   *
   * \return the enumeration value name
   */
  inline
  std::string
  enumValueName(const std::string& value) {
    std::string res;
    char previous = '\0';
    for(char c : value) {
      if(isAlnum(c)) {
	if(isUpper(c) && (isLower(previous) || isDigit(previous))) {
	  res += '_';
	}
	res += toUpper(c);
	previous = c;
      } else {
	if(!res.empty() && res.back() != '_') {
	  res += '_';
	}
	previous = '\0';
      }
    }
    while(!res.empty() && res.back() == '_') {
      res.pop_back();
    }
    if(res.empty()) {
      res = "EMPTY";
    } else if(isDigit(res[0])) {
      res = "_" + res;
    }
    if(MACROS.contains(res)) {
      res += '_';
    }
    return res;
  }

  /*!
   * Check C++ identifier
   *
   * \param identifier the identifier to check
   *
   * \return \c true when valid, \c false otherwise
   */
  inline
  bool
  isIdentifier(const std::string& identifier) {
    if(identifier.empty() || isDigit(identifier[0]) || KEYWORDS.contains(identifier)) {
      return false;
    }
    for(char c : identifier) {
      if(!isAlnum(c) && c != '_') {
	return false;
      }
    }
    return true;
  }

  /*!
   * C++ string literal
   *
   * \param value the value
   *
   * \return the escaped and quoted value
   */
  inline
  std::string
  literal(const std::string& value) {
    std::ostringstream oss;
    oss << '"';
    char previous = '\0';
    for(char c : value) {
      switch(c) {
      case '"':
	oss << "\\\"";
	break;
      case '\\':
	oss << "\\\\";
	break;
      case '\n':
	oss << "\\n";
	break;
      case '\r':
	oss << "\\r";
	break;
      case '\t':
	oss << "\\t";
	break;
      case '?': // ?? could start a trigraph (-Wtrigraphs)
	oss << (previous == '?' ? "\\?" : "?");
	break;
      default:
	if(static_cast<unsigned char>(c) < 0x20) {
	  oss << '\\' << std::oct << std::setw(3) << std::setfill('0') << static_cast<int>(c) << std::dec;
	} else {
	  oss << c;
	}
      }
      previous = c;
    }
    oss << '"';
    return oss.str();
  }

  /*!
   * Text which can be written in a comment
   *
   * \param text the text
   *
   * \return the text without comment end nor trigraph ( \c ??/ at end of line would continue the comment on the
   *         next line, and \c -Wtrigraphs reports every trigraph)
   */
  inline
  std::string
  commentSafe(std::string text) {
    std::size_t pos = 0;
    while((pos = text.find("*/", pos)) != std::string::npos) {
      text.replace(pos, 2, "* /");
    }
    pos = 0;
    while((pos = text.find("??", pos)) != std::string::npos) {
      text.replace(pos, 2, "? ?");
    }
    return text;
  }

  /*!
   * Write Doxygen comment
   *
   * \param out the output stream
   * \param indent the indentation
   * \param text the comment (nothing is written when empty)
   */
  inline
  void
  writeDoc(std::ostream& out, const std::string& indent, const std::string& text) {
    if(text.empty()) {
      return;
    }
    out << indent << "/*!\n";
    std::istringstream lines(commentSafe(text));
    std::string line;
    while(std::getline(lines, line)) {
      while(!line.empty() && (line.back() == '\r' || line.back() == ' ')) {
	line.pop_back();
      }
      out << indent << " *" << (line.empty() ? "" : " ") << line << '\n';
    }
    out << indent << " */\n";
  }

  /*!
   * Schema description (description or title)
   *
   * \param schema the schema
   *
   * \return the description
   */
  inline
  std::string
  describe(const Schema& schema) {
    return schema.description.value_or(schema.title.value_or(""));
  }

  /*!
   * Join description and remark
   *
   * \param description the description
   * \param remark the remark
   *
   * \return the documentation
   */
  inline
  std::string
  document(const std::string& description, const std::string& remark) {
    if(remark.empty()) {
      return description;
    }
    if(description.empty()) {
      return remark;
    }
    return description + "\n\n" + remark;
  }

  /*!
   * Field constraints
   */
  struct Constraints {
    /*! C++ expressions ( \c anch::val::minLength(1) , ...) */
    std::vector<std::string> code;
    /*! Documentation ( \c minLength \c 1 , ...) */
    std::vector<std::string> doc;
  };

  /*!
   * Join strings
   *
   * \param values the strings
   * \param separator the separator
   *
   * \return the joined strings
   */
  inline
  std::string
  join(const std::vector<std::string>& values, const std::string& separator) {
    std::string res;
    for(const std::string& value : values) {
      if(!res.empty()) {
	res += separator;
      }
      res += value;
    }
    return res;
  }

  /*!
   * Check if JSON number is written as an integer (without fraction nor exponent)
   *
   * \param text the JSON number
   *
   * \return \c true for integer
   */
  inline
  bool
  isIntegerText(const std::string& text) {
    return text.find_first_of(".eE") == std::string::npos;
  }

  /*!
   * JSON number value
   *
   * \param text the JSON number
   *
   * \return the value
   */
  inline
  long double
  numberValue(const std::string& text) {
    long double value = 0.0L;
    std::from_chars(text.data(), text.data() + text.size(), value);
    return value;
  }

  /*!
   * Check that JSON number can be written as a C++ literal: integers which fit in \c int64_t or \c uint64_t ,
   * other numbers in \c double range (a literal outside this range does not compile with \c -Werror )
   *
   * \param text the JSON number
   *
   * \return \c true when the number can be written by \ref numberLiteral
   */
  inline
  bool
  representable(const std::string& text) {
    const char* end = text.data() + text.size();
    if(isIntegerText(text)) {
      int64_t signedValue = 0;
      uint64_t unsignedValue = 0;
      if(std::from_chars(text.data(), end, signedValue).ec == std::errc()
	 || std::from_chars(text.data(), end, unsignedValue).ec == std::errc()) {
	return true;
      }
    }
    double value = 0.0;
    auto res = std::from_chars(text.data(), end, value);
    return res.ec == std::errc() && res.ptr == end && std::isfinite(value);
  }

  /*!
   * C++ number literal from JSON number (JSON numbers are valid C++ literals, except integers which do not fit in
   * \c int64_t : they are written as unsigned or floating point numbers)
   *
   * \param text the JSON number
   *
   * \return the C++ literal
   */
  inline
  std::string
  numberLiteral(const std::string& text) {
    if(!isIntegerText(text)) {
      return text;
    }
    if(text == "-9223372036854775808") { // 9223372036854775808 does not fit in int64_t
      return "(-9223372036854775807 - 1)";
    }
    int64_t signedValue = 0;
    if(std::from_chars(text.data(), text.data() + text.size(), signedValue).ec == std::errc()) {
      return text;
    }
    uint64_t unsignedValue = 0;
    if(std::from_chars(text.data(), text.data() + text.size(), unsignedValue).ec == std::errc()) {
      return text + "u";
    }
    return text + ".0";
  }

  /*!
   * Check if JSON number is in C++ integer type range
   *
   * \param text the JSON number
   * \param cpp the C++ integer type ( \c int32_t or \c int64_t )
   *
   * \return \c true when number is in range
   */
  inline
  bool
  inRange(const std::string& text, const std::string& cpp) {
    long double value = numberValue(text);
    if(cpp == "int32_t") {
      return value >= static_cast<long double>(INT32_MIN) && value <= static_cast<long double>(INT32_MAX);
    }
    return value >= static_cast<long double>(INT64_MIN) && value <= static_cast<long double>(INT64_MAX);
  }

  /*!
   * Decode JSON pointer token ( \c ~1 → \c / , \c ~0 → \c ~ )
   *
   * \param token the token
   *
   * \return the decoded token
   */
  inline
  std::string
  decodePointer(const std::string& token) {
    std::string res;
    for(std::size_t i = 0 ; i < token.size() ; ++i) {
      if(token[i] == '~' && i + 1 < token.size() && (token[i + 1] == '0' || token[i + 1] == '1')) {
	res += (token[i + 1] == '0') ? '~' : '/';
	++i;
      } else {
	res += token[i];
      }
    }
    return res;
  }

  /*!
   * \brief Models generator
   */
  /*!
   * Operation parameter (path or query)
   */
  struct OpParameter {
    /*! Parameter name in the request */
    std::string name;
    /*! C++ argument name */
    std::string arg;
    /*! Type */
    Type type;
    /*! Parameter is required */
    bool required = true;
    /*! Description */
    std::string description;
  };

  /*!
   * Form mapping of a model ( \c anch::rest::FormMapper )
   */
  struct FormMapping {
    /*! Model is used as \c multipart/form-data body (files and JSON parts), \c application/x-www-form-urlencoded otherwise */
    bool multipart = false;
    /*! \c toForm statements */
    std::vector<std::string> write;
    /*! \c fromForm statements */
    std::vector<std::string> read;
    /*! \c side argument is used */
    bool side = false;
  };

  /*!
   * Operation result kind
   */
  enum class ResultKind {
    /*! No content */
    VOID,
    /*! Serialized value */
    VALUE,
    /*! Serialized value or \c 404 ( \c std::optional ) */
    OPTIONAL,
    /*! Raw response ( \c anch::rest::Response ) */
    RESPONSE
  };

  /*!
   * Operation to generate
   */
  struct Op {
    /*! anch-rest method ( \c get , \c post , \c put , \c patch or \c del ) */
    std::string method;
    /*! Path pattern */
    std::string path;
    /*! C++ method name */
    std::string name;
    /*! Documentation */
    std::string description;
    /*! Operation is deprecated */
    bool deprecated = false;
    /*! Path parameters (pattern order) */
    std::vector<OpParameter> pathParameters;
    /*! Query parameters (required first) */
    std::vector<OpParameter> queryParameters;
    /*! Request body type (when operation has a body) */
    std::optional<Type> body;
    /*! Request body argument name */
    std::string bodyArg;
    /*! Request body codecs ( \c anch::codec::Json , \c anch::codec::Text ) */
    std::vector<std::string> consumes;
    /*! Request body description */
    std::string bodyDescription;
    /*! Request body is a raw stream (media types without codec): \c std::istream& server side,
     * \c anch::rest::RawBody client side */
    bool raw = false;
    /*! Content type sent by the client for raw bodies without content type */
    std::string rawContentType;
    /*! Result kind */
    ResultKind result = ResultKind::VOID;
    /*! Result type ( \c VALUE and \c OPTIONAL ) */
    Type resultType;
    /*! Result codecs */
    std::vector<std::string> produces;
    /*! Success status enumeration value ( \c CREATED , ...) when it is not the default one */
    std::string status;
  };

  class Generator {
  private:
    /*! OpenAPI document */
    const OpenAPI& _api;
    /*! Options */
    const ModelOptions& _options;
    /*! Used type names */
    std::set<std::string> _names;
    /*! Type name of each component schema */
    std::map<std::string,std::string> _componentTypes;
    /*! Models names */
    std::set<std::string> _modelNames;
    /*! Models to build: name and schema */
    std::vector<std::pair<std::string,const Schema*>> _pending;
    /*! Resolved properties types (inherited properties are resolved once) */
    std::map<const Schema*,Type> _resolved;
    /*! Unsupported properties: reason */
    std::map<const Schema*,std::string> _unsupported;
    /*! Properties constraints (inherited properties constraints are computed once) */
    std::map<const Schema*,Constraints> _constraints;
    /*! Built models */
    std::map<std::string,Model> _models;
    /*! Enumerations */
    std::map<std::string,Enumeration> _enums;
    /*! Component schemas resolved as alias */
    std::map<std::string,Type> _aliasTypes;
    /*! Component schemas which can not be resolved: reason */
    std::map<std::string,std::string> _aliasErrors;
    /*! Component schemas being resolved (recursive aliases detection) */
    std::set<std::string> _resolving;
    /*! Aliases */
    std::vector<Alias> _aliases;
    /*! Warnings */
    std::vector<std::string> _warnings;
    /*! Operations (endpoints) */
    std::vector<Op> _operations;
    /*! Operations methods names */
    std::set<std::string> _operationNames;
    /*! \c anch::UUID is used */
    bool _uuid = false;
    /*! \c anch::date::Date or \c std::chrono::year_month_day is used */
    bool _date = false;
    /*! Models used as form bodies: their form mapping */
    std::map<std::string,FormMapping> _forms;
    /*! \c anch::rest::Part is used in models */
    bool _parts = false;
    /*! Form codecs used by operations */
    std::set<std::string> _formCodecs;
    /*! \c anch::rest::Form is used as raw body */
    bool _rawForm = false;

  public:
    Generator(const OpenAPI& api, const ModelOptions& options): _api(api), _options(options) {
      // Nothing to do
    }

    GeneratedModels
    generate() {
      checkOptions();
      const auto& schemas = _api.components.schemas;
      // Reserve components names first: inline schemas get other names +
      for(const auto& [name, schema] : schemas) {
	_componentTypes[name] = uniqueName(pascal(name));
      }
      // Reserve components names first: inline schemas get other names -
      // Models and enumerations +
      for(const auto& [name, schema] : schemas) {
	const std::string& type = _componentTypes[name];
	if(isModel(schema)) {
	  _modelNames.insert(type);
	  _pending.emplace_back(type, &schema);
	} else if(isStringEnum(schema)) {
	  addEnum(type, schema);
	}
      }
      // Models and enumerations -
      // Aliases +
      for(const auto& [name, schema] : schemas) {
	const std::string& type = _componentTypes[name];
	if(_modelNames.contains(type) || _enums.contains(type)) {
	  continue;
	}
	try {
	  _aliases.push_back(Alias{type, describe(schema), resolveComponent(name)});
	} catch(const Unsupported& error) {
	  warn("components.schemas." + name, error.reason);
	} catch(const GeneratorError& error) {
	  throw GeneratorError("components.schemas." + name + ": " + error.what());
	}
      }
      // Aliases -
      // Operations (inline schemas of bodies and responses are models) +
      if(_options.server || _options.client) {
	collectOperations();
      }
      // Operations -
      // Models (inline models are added while building) +
      for(std::size_t i = 0 ; i < _pending.size() ; ++i) {
	auto [name, schema] = _pending[i];
	buildModel(name, *schema);
      }
      // Models -
      std::vector<std::string> order = sortModels();
      buildFormMappings();
      GeneratedModels res;
      std::ostringstream header;
      writeHeader(header, order);
      res.header = header.str();
      std::ostringstream source;
      writeSource(source);
      res.source = source.str();
      if(_options.server || _options.client) {
	std::ostringstream formsHeader;
	writeFormsHeader(formsHeader);
	res.formsHeader = formsHeader.str();
	std::ostringstream formsSource;
	writeFormsSource(formsSource);
	res.formsSource = formsSource.str();
      }
      if(_options.server) {
	std::ostringstream serverHeader;
	writeServerHeader(serverHeader);
	res.serverHeader = serverHeader.str();
	std::ostringstream serverSource;
	writeServerSource(serverSource);
	res.serverSource = serverSource.str();
      }
      if(_options.client) {
	std::ostringstream clientHeader;
	writeClientHeader(clientHeader);
	res.clientHeader = clientHeader.str();
	std::ostringstream clientSource;
	writeClientSource(clientSource);
	res.clientSource = clientSource.str();
      }
      res.warnings = _warnings;
      return res;
    }

  private:
    // Endpoints (endpointGenerator.cpp) +
    /*!
     * Collect operations of document paths: types are resolved (inline schemas become models)
     */
    void collectOperations();

    /*!
     * Build operation
     *
     * \param method the OpenAPI method ( \c get , \c post , ...)
     * \param path the path pattern
     * \param item the path item
     * \param operation the operation
     *
     * \throw Unsupported unsupported operation
     */
    Op buildOperation(const std::string& method, const std::string& path, const anch::oas::PathItem& item,
		      const anch::oas::Operation& operation);

    /*!
     * Parameter from reference
     */
    const anch::oas::Parameter& parameter(const anch::oas::Parameter& parameter) const;

    /*!
     * C++ type of operation argument
     */
    std::string argumentType(const Type& type, bool optional) const;

    /*!
     * Operation method declaration (without return type and class name)
     */
    std::string signature(const Op& op, bool defaults, bool client) const;

    /*!
     * Operation result C++ type
     */
    std::string resultType(const Op& op) const;

    /*!
     * API name (service and client prefix)
     */
    std::string apiName() const;

    /*!
     * Build form mappings of form body models ( \c anch::rest::FormMapper statements and warnings)
     */
    void buildFormMappings();

    /*!
     * Include directive of a form codec when operations use it
     *
     * \param multipart \c multipart/form-data codec, \c application/x-www-form-urlencoded otherwise
     */
    std::string formCodecInclude(bool multipart) const;

    void writeFormsHeader(std::ostream& out) const;
    void writeFormsSource(std::ostream& out) const;
    void writeServerHeader(std::ostream& out) const;
    void writeServerSource(std::ostream& out) const;
    void writeClientHeader(std::ostream& out) const;
    void writeClientSource(std::ostream& out) const;
    void writeOperationDoc(std::ostream& out, const Op& op, const std::string& indent, bool client) const;

    /*!
     * Some operation has a raw body
     */
    bool hasRawBody() const;
    // Endpoints (endpointGenerator.cpp) -

    void
    checkOptions() const {
      if(_options.name.empty() || _options.name.find_first_of("/\\") != std::string::npos) {
	throw GeneratorError("Invalid file name '" + _options.name + "'");
      }
      std::size_t cur = 0;
      do {
	std::size_t pos = _options.ns.find("::", cur);
	std::string part = _options.ns.substr(cur, pos == std::string::npos ? std::string::npos : pos - cur);
	if(!isIdentifier(part)) {
	  throw GeneratorError("Invalid namespace '" + _options.ns + "'");
	}
	if(pos == std::string::npos) {
	  break;
	}
	cur = pos + 2;
      } while(true);
      if(!_options.api.empty() && !isIdentifier(_options.api)) {
	throw GeneratorError("Invalid API name '" + _options.api + "'");
      }
    }

    void
    warn(const std::string& path, const std::string& reason) {
      if(_options.strict) {
	throw GeneratorError(path + ": " + reason);
      }
      _warnings.push_back(path + ": " + reason + ": not generated");
    }

    /*!
     * Warn when a minimum size is greater than the maximum one: no value can be valid (error in strict mode)
     *
     * \param min the minimum
     * \param max the maximum
     * \param minKeyword the minimum keyword
     * \param maxKeyword the maximum keyword
     * \param path the property path
     */
    void
    inconsistent(const std::optional<uint64_t>& min, const std::optional<uint64_t>& max,
		 const std::string& minKeyword, const std::string& maxKeyword, const std::string& path) {
      if(min.has_value() && max.has_value() && min.value() > max.value()) {
	constraintWarning(path, minKeyword + " " + std::to_string(min.value()) + " is greater than " + maxKeyword + " "
			  + std::to_string(max.value()) + ": no value is valid");
      }
    }

    /*!
     * Constraint warning: the property is generated without this constraint (error in strict mode)
     */
    void
    constraintWarning(const std::string& path, const std::string& reason) {
      if(_options.strict) {
	throw GeneratorError(path + ": " + reason);
      }
      _warnings.push_back(path + ": " + reason);
    }

    std::string
    uniqueName(const std::string& base) {
      std::string name = base;
      for(int i = 2 ; _names.contains(name) ; ++i) {
	name = base + std::to_string(i);
      }
      _names.insert(name);
      return name;
    }

    /*!
     * Check if schema is a file ( \c string with \c binary format) or an array of files
     */
    static bool
    isBinary(const Schema& schema) {
      if(schema.type == "array" && schema.items) {
	return isBinary(*schema.items);
      }
      return schema.type == "string" && schema.format == "binary";
    }

    static bool
    isModel(const Schema& schema) {
      if(schema.ref || !schema.oneOf.empty() || !schema.anyOf.empty()) {
	return false;
      }
      if(!schema.allOf.empty()) {
	return true;
      }
      if(schema.type.has_value() && schema.type.value() != "object") {
	return false;
      }
      // Object without properties and without additional properties is an empty model
      return !schema.properties.empty()
	|| (schema.additionalProperties.has_value() && !schema.additionalProperties->allowed);
    }

    static bool
    isStringEnum(const Schema& schema) {
      if(schema.enumeration.empty() || (schema.type.has_value() && schema.type.value() != "string")) {
	return false;
      }
      bool string = false;
      for(const Scalar& value : schema.enumeration) {
	if(value.kind == Scalar::Kind::STRING) {
	  string = true;
	} else if(value.kind != Scalar::Kind::NUL) {
	  return false;
	}
      }
      return string;
    }

    static bool
    hasNull(const Schema& schema) {
      if(schema.nullable.value_or(false)) {
	return true;
      }
      for(const Scalar& value : schema.enumeration) {
	if(value.kind == Scalar::Kind::NUL) {
	  return true;
	}
      }
      return false;
    }

    void
    addEnum(const std::string& name, const Schema& schema) {
      Enumeration enumeration;
      enumeration.name = name;
      enumeration.description = describe(schema);
      std::set<std::string> values;
      std::set<std::string> identifiers;
      for(const Scalar& value : schema.enumeration) {
	if(value.kind != Scalar::Kind::STRING || !values.insert(value.text).second) {
	  continue;
	}
	std::string base = enumValueName(value.text);
	std::string identifier = base;
	for(int i = 2 ; identifiers.contains(identifier) ; ++i) {
	  identifier = base + "_" + std::to_string(i);
	}
	identifiers.insert(identifier);
	enumeration.values.emplace_back(value.text, identifier);
      }
      _enums[name] = std::move(enumeration);
    }

    /*!
     * Component schema from reference
     */
    std::string
    componentName(const std::string& ref) const {
      if(!ref.starts_with(SCHEMAS_REF)) {
	throw Unsupported{"reference '" + ref + "' (only '" + SCHEMAS_REF + "' references are supported)"};
      }
      std::string name = decodePointer(ref.substr(SCHEMAS_REF.size()));
      if(!_api.components.schemas.contains(name)) {
	throw GeneratorError("Unknown reference '" + ref + "'");
      }
      return name;
    }

    /*!
     * Resolve component schema which is neither a model nor an enumeration (once)
     */
    Type
    resolveComponent(const std::string& name) {
      auto found = _aliasTypes.find(name);
      if(found != _aliasTypes.end()) {
	return found->second;
      }
      auto error = _aliasErrors.find(name);
      if(error != _aliasErrors.end()) {
	throw Unsupported{error->second};
      }
      if(!_resolving.insert(name).second) {
	throw Unsupported{"recursive reference '" + SCHEMAS_REF + name + "' (only models can be recursive)"};
      }
      try {
	Type type = resolve(_api.components.schemas.at(name), _componentTypes.at(name));
	_resolving.erase(name);
	_aliasTypes[name] = type;
	return type;
      } catch(const Unsupported& unsupported) {
	_resolving.erase(name);
	_aliasErrors[name] = unsupported.reason;
	throw;
      } catch(...) {
	_resolving.erase(name);
	throw;
      }
    }

    /*!
     * Resolve schema C++ type
     *
     * \param schema the schema
     * \param name the name to use for inline model or enumeration
     */
    Type
    resolve(const Schema& schema, const std::string& name) {
      // Reference +
      if(schema.ref.has_value()) {
	std::string component = componentName(schema.ref.value());
	const std::string& type = _componentTypes.at(component);
	if(_modelNames.contains(type)) {
	  return generatedType(type, true, false);
	}
	if(_enums.contains(type)) {
	  return generatedType(type, false, hasNull(_api.components.schemas.at(component)));
	}
	return resolveComponent(component);
      }
      // Reference -
      if(!schema.oneOf.empty() || !schema.anyOf.empty()) {
	throw Unsupported{"oneOf / anyOf composition (polymorphism is not supported by anch-json)"};
      }
      // Inline model or enumeration +
      if(isModel(schema)) {
	std::string type = uniqueName(name);
	_modelNames.insert(type);
	_pending.emplace_back(type, &schema);
	return generatedType(type, true, hasNull(schema));
      }
      if(isStringEnum(schema)) {
	std::string type = uniqueName(name);
	addEnum(type, schema);
	return generatedType(type, false, hasNull(schema));
      }
      // Inline model or enumeration -
      std::string kind = schema.type.value_or((schema.properties.empty() && !schema.additionalProperties.has_value()) ? "" : "object");
      bool nullable = schema.nullable.value_or(false);
      // Containers +
      if(kind == "array") {
	if(!schema.items) {
	  throw Unsupported{"array without items"};
	}
	Type item = resolve(*schema.items, name + "Item");
	if(item.container != Container::NONE) {
	  throw Unsupported{"array of arrays or maps (nested containers are not supported by anch-json)"};
	}
	item.container = Container::VECTOR;
	item.nullable = nullable;
	return item;
      }
      if(kind == "object") {
	if(schema.additionalProperties.has_value() && schema.additionalProperties->schema) {
	  Type value = resolve(*schema.additionalProperties->schema, name + "Value");
	  if(value.container != Container::NONE) {
	    throw Unsupported{"map of arrays or maps (nested containers are not supported by anch-json)"};
	  }
	  value.container = Container::MAP;
	  value.nullable = nullable;
	  return value;
	}
	throw Unsupported{"free-form object (anch-json has no generic JSON value type)"};
      }
      // Containers -
      // Primitives +
      Type type;
      type.nullable = nullable;
      std::string format = schema.format.value_or("");
      if(kind == "string") {
	if(format == "uuid" && _options.uuid) {
	  type.cpp = "anch::UUID";
	  _uuid = true;
	} else if(format == "date-time" && _options.date) {
	  type.cpp = "anch::date::Date";
	  _date = true;
	} else if(format == "date" && _options.date) {
	  type.cpp = "std::chrono::year_month_day";
	  _date = true;
	} else {
	  type.cpp = "std::string";
	}
      } else if(kind == "integer") {
	type.cpp = (format == "int32") ? "int32_t" : "int64_t";
      } else if(kind == "number") {
	type.cpp = (format == "float") ? "float" : "double";
      } else if(kind == "boolean") {
	type.cpp = "bool";
      } else if(kind.empty()) {
	throw Unsupported{"schema without type (any value)"};
      } else {
	throw Unsupported{"type '" + kind + "'"};
      }
      if(!format.empty()) {
	type.remark = "Format: " + format;
      }
      if(!schema.enumeration.empty()) {
	std::string values;
	for(const Scalar& value : schema.enumeration) {
	  values += (values.empty() ? "" : ", ") + (value.kind == Scalar::Kind::STRING ? literal(value.text) : value.text);
	}
	type.remark += (type.remark.empty() ? "" : "\n") + std::string("Allowed values: ") + values;
      }
      return type;
      // Primitives -
    }

    /*!
     * Merge \c allOf schemas: properties (with the model which declares them), required properties and additional
     * properties
     */
    void
    merge(const Schema& schema, const std::string& owner, Composite& result, std::set<const Schema*>& path) {
      if(!path.insert(&schema).second) {
	throw Unsupported{"recursive allOf composition"};
      }
      for(const Schema& sub : schema.allOf) {
	const Schema* target = &sub;
	std::string subOwner = owner;
	if(sub.ref.has_value()) {
	  std::string component = componentName(sub.ref.value());
	  target = &_api.components.schemas.at(component);
	  subOwner = _componentTypes.at(component);
	}
	if(!target->oneOf.empty() || !target->anyOf.empty()) {
	  throw Unsupported{"allOf with oneOf / anyOf composition (polymorphism is not supported by anch-json)"};
	}
	merge(*target, subOwner, result, path);
      }
      for(const auto& [name, property] : schema.properties) {
	result.properties[name] = Property{&property, owner};
      }
      result.required.insert(schema.required.begin(), schema.required.end());
      if(schema.additionalProperties.has_value()) {
	result.additional = &schema.additionalProperties.value();
      }
      path.erase(&schema);
    }

    /*!
     * Resolve property type once: inherited properties share their types and warnings
     *
     * \throw Skipped unsupported property (the warning is reported once)
     */
    Type
    resolveProperty(const std::string& json, const Property& property) {
      auto found = _resolved.find(property.schema);
      if(found != _resolved.end()) {
	return found->second;
      }
      auto unsupported = _unsupported.find(property.schema);
      if(unsupported != _unsupported.end()) {
	throw Skipped{unsupported->second};
      }
      std::string path = property.owner + "." + json;
      try {
	Type type = resolve(*property.schema, property.owner + pascal(json));
	_resolved[property.schema] = type;
	return type;
      } catch(const Unsupported& error) {
	_unsupported[property.schema] = error.reason;
	warn(path, error.reason);
	throw Skipped{error.reason};
      } catch(const GeneratorError& error) {
	throw GeneratorError(path + ": " + error.what());
      }
    }

    /*!
     * Schema which holds value constraints: references to alias components are followed
     *
     * \param schema the property (or items) schema
     *
     * \return the schema, \c nullptr for models and enumerations (they have no value constraint)
     */
    const Schema*
    constrained(const Schema& schema) const {
      const Schema* current = &schema;
      for(std::size_t depth = 0 ; current->ref.has_value() ; ++depth) {
	if(depth > _api.components.schemas.size()) { // recursive aliases are not generated
	  return nullptr;
	}
	std::string component = componentName(current->ref.value());
	const std::string& type = _componentTypes.at(component);
	if(_modelNames.contains(type) || _enums.contains(type)) {
	  return nullptr;
	}
	current = &_api.components.schemas.at(component);
      }
      return current;
    }

    /*!
     * Value constraints of single value: string, number and enumeration constraints
     *
     * \param schema the schema which holds constraints ( \c nullptr for none)
     * \param type the value type
     * \param path the property path for warnings
     * \param res the constraints to complete
     */
    void
    valueConstraints(const Schema* schema, const Type& type, const std::string& path, Constraints& res) {
      if(schema == nullptr || type.model || type.enumeration) {
	return;
      }
      const std::string& cpp = type.cpp;
      bool string = (cpp == "std::string");
      bool formatted = (cpp == "anch::UUID" || cpp == "anch::date::Date" || cpp == "std::chrono::year_month_day");
      bool integer = (cpp == "int32_t" || cpp == "int64_t");
      bool number = integer || cpp == "float" || cpp == "double";
      auto ignored = [&](const std::string& keyword) {
	constraintWarning(path, keyword + " is ignored for " + cpp + " value");
      };
      // Strings +
      auto length = [&](const std::optional<uint64_t>& value, const std::string& keyword) {
	if(!value.has_value()) {
	  return;
	}
	if(string) {
	  res.code.push_back("anch::val::" + keyword + "(" + std::to_string(value.value()) + ")");
	  res.doc.push_back(keyword + " " + std::to_string(value.value()));
	} else if(!formatted) { // formatted strings are checked by their own parser
	  ignored(keyword);
	}
      };
      length(schema->minLength, "minLength");
      length(schema->maxLength, "maxLength");
      if(string) {
	inconsistent(schema->minLength, schema->maxLength, "minLength", "maxLength", path);
      }
      if(schema->pattern.has_value() && string) {
	const std::string& pattern = schema->pattern.value();
	try { // pattern is checked here: an invalid pattern would raise an error when mapper is registered
	  std::regex check(pattern, std::regex::ECMAScript);
	  res.code.push_back("anch::val::pattern(" + literal(pattern) + ")");
	  res.doc.push_back("pattern " + pattern);
	} catch(const std::regex_error& error) {
	  constraintWarning(path, "pattern " + pattern + " is not a valid ECMAScript regular expression (" + error.what() + "): ignored");
	}
      }
      if(schema->format.has_value() && string) { // unknown formats are not checked by anch-json
	res.code.push_back("anch::val::format(" + literal(schema->format.value()) + ")");
      }
      // Strings -
      // Numbers +
      auto bound = [&](const std::optional<Scalar>& value, bool exclusive, const std::string& keyword, const std::string& exclusiveKeyword) {
	if(!value.has_value()) {
	  return;
	}
	if(!number) {
	  ignored(keyword);
	  return;
	}
	if(value->kind != Scalar::Kind::NUMBER) {
	  constraintWarning(path, keyword + " is not a number: ignored");
	  return;
	}
	if(!representable(value->text)) {
	  constraintWarning(path, keyword + " " + value->text + " is outside double range: ignored");
	  return;
	}
	if(integer && !inRange(value->text, cpp)) {
	  constraintWarning(path, keyword + " " + value->text + " is outside " + cpp + " range");
	}
	const std::string& name = exclusive ? exclusiveKeyword : keyword;
	res.code.push_back("anch::val::" + name + "(" + numberLiteral(value->text) + ")");
	res.doc.push_back(name + " " + value->text);
      };
      bound(schema->minimum, schema->exclusiveMinimum.value_or(false), "minimum", "exclusiveMinimum");
      bound(schema->maximum, schema->exclusiveMaximum.value_or(false), "maximum", "exclusiveMaximum");
      if(number && schema->minimum.has_value() && schema->maximum.has_value()
	 && schema->minimum->kind == Scalar::Kind::NUMBER && schema->maximum->kind == Scalar::Kind::NUMBER
	 && representable(schema->minimum->text) && representable(schema->maximum->text)) { // ignored bounds are not compared
	long double min = numberValue(schema->minimum->text);
	long double max = numberValue(schema->maximum->text);
	bool exclusive = schema->exclusiveMinimum.value_or(false) || schema->exclusiveMaximum.value_or(false);
	if(min > max || (exclusive && min == max)) {
	  constraintWarning(path, "minimum " + schema->minimum->text + " and maximum " + schema->maximum->text
			    + " are inconsistent: no value is valid");
	}
      }
      if(schema->multipleOf.has_value()) {
	const Scalar& divisor = schema->multipleOf.value();
	if(!number) {
	  ignored("multipleOf");
	} else if(divisor.kind != Scalar::Kind::NUMBER || !(numberValue(divisor.text) > 0.0L) || !representable(divisor.text)) {
	  constraintWarning(path, "multipleOf has to be a strictly positive number: ignored");
	} else {
	  res.code.push_back("anch::val::multipleOf(" + numberLiteral(divisor.text) + ")");
	  res.doc.push_back("multipleOf " + divisor.text);
	}
      }
      // Numbers -
      // Enumeration (string enumerations are generated as enum class) +
      if(!schema->enumeration.empty()) {
	std::vector<std::string> values;
	for(const Scalar& value : schema->enumeration) {
	  if(value.kind == Scalar::Kind::NUL) { // null is allowed by nullable
	    continue;
	  }
	  bool valid = false;
	  if(number) {
	    valid = (value.kind == Scalar::Kind::NUMBER) && representable(value.text)
	      && (!integer || (isIntegerText(value.text) && inRange(value.text, cpp)));
	  } else if(cpp == "bool") {
	    valid = (value.kind == Scalar::Kind::BOOLEAN);
	  } else if(string) {
	    valid = (value.kind == Scalar::Kind::STRING);
	  }
	  if(!valid) {
	    constraintWarning(path, "enum value " + (value.kind == Scalar::Kind::STRING ? literal(value.text) : value.text) + " is not a "
		 + cpp + " value: enum is not checked");
	    values.clear();
	    break;
	  }
	  values.push_back(value.kind == Scalar::Kind::STRING ? literal(value.text) : (number ? numberLiteral(value.text) : value.text));
	}
	if(!values.empty()) {
	  res.code.push_back("anch::val::allowed<" + cpp + ">({" + join(values, ", ") + "})");
	}
      }
      // Enumeration -
    }

    /*!
     * Property constraints (computed once: inherited properties share their constraints and warnings)
     *
     * \param property the property schema
     * \param type the property type
     * \param path the property path for warnings
     *
     * \return the constraints
     */
    const Constraints&
    propertyConstraints(const Schema& property, const Type& type, const std::string& path) {
      auto found = _constraints.find(&property);
      if(found != _constraints.end()) {
	return found->second;
      }
      Constraints res;
      const Schema* schema = _options.constraints ? constrained(property) : nullptr;
      if(schema == nullptr) {
	return _constraints[&property] = res;
      }
      auto size = [&](const std::optional<uint64_t>& value, const std::string& keyword) {
	if(value.has_value()) {
	  res.code.push_back("anch::val::" + keyword + "(" + std::to_string(value.value()) + ")");
	  res.doc.push_back(keyword + " " + std::to_string(value.value()));
	}
      };
      const Schema* items = nullptr;
      switch(type.container) {
      case Container::VECTOR:
	size(schema->minItems, "minItems");
	size(schema->maxItems, "maxItems");
	inconsistent(schema->minItems, schema->maxItems, "minItems", "maxItems", path);
	if(schema->uniqueItems.value_or(false)) {
	  if(type.cpp == "bool") { // std::vector<bool> items are not references
	    constraintWarning(path, "uniqueItems is not supported for boolean arrays: ignored");
	  } else {
	    res.code.push_back("anch::val::uniqueItems");
	    res.doc.push_back("uniqueItems");
	  }
	}
	items = schema->items ? constrained(*schema->items) : nullptr;
	break;
      case Container::MAP:
	size(schema->minProperties, "minProperties");
	size(schema->maxProperties, "maxProperties");
	inconsistent(schema->minProperties, schema->maxProperties, "minProperties", "maxProperties", path);
	items = (schema->additionalProperties.has_value() && schema->additionalProperties->schema)
	  ? constrained(*schema->additionalProperties->schema) : nullptr;
	break;
      case Container::NONE:
	valueConstraints(schema, type, path, res);
	break;
      }
      if(type.container != Container::NONE) {
	Constraints itemsConstraints;
	valueConstraints(items, type, path + (type.container == Container::VECTOR ? "[]" : "{}"), itemsConstraints);
	if(!itemsConstraints.code.empty()) {
	  res.code.push_back("anch::val::items(" + join(itemsConstraints.code, ", ") + ")");
	}
	if(!itemsConstraints.doc.empty()) {
	  res.doc.push_back("items: " + join(itemsConstraints.doc, ", "));
	}
      }
      return _constraints[&property] = res;
    }

    void
    buildModel(const std::string& name, const Schema& schema) {
      Model model;
      model.name = name;
      model.description = describe(schema);
      Composite composite;
      std::set<const Schema*> path;
      try {
	merge(schema, name, composite, path);
      } catch(const Unsupported& error) {
	warn(name, error.reason);
	model.skipped.push_back("allOf: " + error.reason);
      } catch(const GeneratorError& error) {
	throw GeneratorError(name + ": " + error.what());
      }
      if(composite.additional != nullptr && composite.additional->schema && !composite.properties.empty()) {
	warn(name + ".additionalProperties", "additional properties of model with properties");
	model.skipped.push_back("additionalProperties: additional properties of model with properties");
      }
      for(const std::string& required : composite.required) {
	if(!composite.properties.contains(required)) {
	  constraintWarning(name, "required property '" + required + "' is not defined in properties");
	}
      }
      std::set<std::string> members;
      for(const auto& [json, property] : composite.properties) {
	try {
	  Field field;
	  field.json = json;
	  field.type = resolveProperty(json, property);
	  auto form = _forms.find(name);
	  if(form != _forms.end() && form->second.multipart && isBinary(*property.schema)) {
	    // File of multipart body: anch::rest::Part keeps file name and media type
	    field.type.cpp = "anch::rest::Part";
	    field.type.remark = "File (multipart/form-data part)";
	    field.part = true;
	    _parts = true;
	  }
	  std::string member = memberName(json);
	  while(members.contains(member) || _names.contains(member)) {
	    member += '_';
	  }
	  members.insert(member);
	  field.member = member;
	  bool nullable = field.type.nullable || property.schema->nullable.value_or(false);
	  field.required = composite.required.contains(json);
	  field.optional = (field.type.container == Container::NONE) && (!field.required || nullable);
	  const Constraints& constraints = field.part ? Constraints{} : propertyConstraints(*property.schema, field.type, property.owner + "." + json);
	  field.constraints = constraints.code;
	  std::string remark = field.type.remark;
	  if(!constraints.doc.empty()) {
	    remark += (remark.empty() ? "" : "\n") + std::string("Constraints: ") + join(constraints.doc, ", ");
	  }
	  field.description = document(describe(*property.schema), remark);
	  model.fields.push_back(std::move(field));
	} catch(const Skipped& skipped) {
	  model.skipped.push_back(json + ": " + skipped.reason);
	}
      }
      _models[name] = std::move(model);
    }

    /*!
     * Sort models: strongly connected components (Tarjan) of the models graph, dependencies first.\n
     * Single value references inside a component (recursion) become \c std::shared_ptr .
     *
     * \return the models in declaration order
     */
    std::vector<std::string>
    sortModels() {
      std::map<std::string,std::size_t> index;
      std::map<std::string,std::size_t> low;
      std::set<std::string> onStack;
      std::vector<std::string> stack;
      std::vector<std::vector<std::string>> components;
      std::size_t counter = 0;
      std::function<void(const std::string&)> visit = [&](const std::string& name) {
	index[name] = low[name] = counter++;
	stack.push_back(name);
	onStack.insert(name);
	for(const Field& field : _models.at(name).fields) {
	  if(!field.type.model) {
	    continue;
	  }
	  const std::string& next = field.type.cpp;
	  if(!index.contains(next)) {
	    visit(next);
	    low[name] = std::min(low[name], low[next]);
	  } else if(onStack.contains(next)) {
	    low[name] = std::min(low[name], index[next]);
	  }
	}
	if(low[name] == index[name]) {
	  std::vector<std::string> component;
	  std::string item;
	  do {
	    item = stack.back();
	    stack.pop_back();
	    onStack.erase(item);
	    component.push_back(item);
	  } while(item != name);
	  components.push_back(std::move(component));
	}
      };
      for(const auto& [name, model] : _models) {
	if(!index.contains(name)) {
	  visit(name);
	}
      }
      // Tarjan gives components in reverse topological order: dependencies first +
      std::vector<std::string> order;
      for(std::vector<std::string>& component : components) {
	std::set<std::string> members(component.begin(), component.end());
	std::sort(component.begin(), component.end());
	for(const std::string& name : component) {
	  for(Field& field : _models.at(name).fields) {
	    if(field.type.model && field.type.container == Container::NONE && members.contains(field.type.cpp)) {
	      field.pointer = true;
	      field.optional = false;
	    }
	  }
	  order.push_back(name);
	}
      }
      // Tarjan gives components in reverse topological order: dependencies first -
      return order;
    }

    std::string
    qualified(const Type& type) const {
      return (type.model || type.enumeration) ? "::" + _options.ns + "::" + type.cpp : type.cpp;
    }

    /*!
     * Member initializer: value types are initialized (\c anch::date::Date to epoch instead of now)
     */
    static std::string
    initializer(const Field& field) {
      if(field.optional || field.pointer || field.type.container != Container::NONE || field.type.model
	 || field.type.cpp == "std::string" || field.type.cpp == "anch::UUID") {
	return "";
      }
      return (field.type.cpp == "anch::date::Date") ? "{false}" : "{}";
    }

    static std::string
    cppType(const Type& type, bool optional, bool pointer) {
      std::string res = type.cpp;
      if(pointer) {
	res = "std::shared_ptr<" + res + ">";
      }
      switch(type.container) {
      case Container::VECTOR:
	res = "std::vector<" + res + ">";
	break;
      case Container::MAP:
	res = "std::map<std::string," + res + ">";
	break;
      case Container::NONE:
	break;
      }
      if(optional) {
	res = "std::optional<" + res + ">";
      }
      return res;
    }

    void
    writeBanner(std::ostream& out) const {
      out << "/*\n"
	  << "  Generated by anch-oas-gen from " << commentSafe(_api.info.title) << ' ' << commentSafe(_api.info.version)
	  << " (OpenAPI " << commentSafe(_api.openapi) << "): do not edit.\n"
	  << "*/\n";
    }

    void
    writeHeader(std::ostream& out, const std::vector<std::string>& order) const {
      writeBanner(out);
      out << "#pragma once\n\n";
      if(_date) {
	out << "#include <chrono>\n";
      }
      out << "#include <cstdint>\n"
	  << "#include <map>\n"
	  << "#include <memory>\n"
	  << "#include <optional>\n"
	  << "#include <string>\n"
	  << "#include <vector>\n\n"
	  << "#include \"json/factory.hpp\"\n"
	  << "#include \"mapping.hpp\"\n"
	  << "#include \"val/constraints.hpp\"\n";
      if(_date) {
	out << "#include \"date/date.hpp\"\n";
      }
      if(_uuid) {
	out << "#include \"uuid.hpp\"\n";
      }
      if(_parts) {
	out << "\n#include \"rest/form.hpp\"\n";
      }
      out << "\nnamespace " << _options.ns << " {\n\n";
      // Enumerations +
      if(!_enums.empty()) {
	out << "  // Enumerations +\n";
	for(const auto& [name, enumeration] : _enums) {
	  writeDoc(out, "  ", enumeration.description);
	  out << "  enum class " << name << " {\n";
	  for(std::size_t i = 0 ; i < enumeration.values.size() ; ++i) {
	    const auto& [value, identifier] = enumeration.values[i];
	    out << "    " << identifier << (i + 1 < enumeration.values.size() ? "," : "")
		<< " /*!< \\c " << commentSafe(literal(value)) << " */\n";
	  }
	  out << "  };\n\n";
	}
	out << "  // Enumerations -\n\n";
      }
      // Enumerations -
      // Models +
      if(!order.empty()) {
	out << "  // Models +\n";
	for(const std::string& name : order) {
	  out << "  struct " << name << ";\n";
	}
	out << '\n';
	for(const std::string& name : order) {
	  const Model& model = _models.at(name);
	  writeDoc(out, "  ", model.description);
	  out << "  struct " << name << " {\n";
	  for(std::size_t i = 0 ; i < model.fields.size() ; ++i) {
	    const Field& field = model.fields[i];
	    writeDoc(out, "    ", field.description);
	    out << "    " << cppType(field.type, field.optional, field.pointer) << ' ' << field.member
		<< initializer(field) << ";\n";
	    out << '\n';
	  }
	  out << "    /*! Equality operator: every member is compared (used by \\c uniqueItems constraint) */\n"
	      << "    bool operator==(const " << name << "&) const = default;\n";
	  for(const std::string& skipped : model.skipped) {
	    out << "    // Not generated: " << commentSafe(skipped) << '\n';
	  }
	  out << "  };\n\n";
	}
	out << "  // Models -\n\n";
      }
      // Models -
      // Aliases +
      if(!_aliases.empty()) {
	out << "  // Aliases +\n";
	for(const Alias& alias : _aliases) {
	  writeDoc(out, "  ", document(alias.description, alias.type.remark));
	  out << "  using " << alias.name << " = " << cppType(alias.type, false, false) << ";\n\n";
	}
	out << "  // Aliases -\n\n";
      }
      // Aliases -
      out << "}  // " << _options.ns << "\n";
      // anch-json declarations +
      if(!_enums.empty()) {
	out << "\nnamespace anch::json {\n\n";
	for(const auto& [name, enumeration] : _enums) {
	  out << "  /*! \\c ::" << _options.ns << "::" << name << " has a dedicated mapper (JSON string) */\n"
	      << "  template<>\n"
	      << "  constexpr bool isPrimitive<::" << _options.ns << "::" << name << ">() {\n"
	      << "    return true;\n"
	      << "  }\n\n";
	}
	out << "}  // anch::json\n";
      }
      // anch-json declarations -
      // Descriptions (anch::Mapping, for every format) +
      if(!order.empty()) {
	out << "\n// Descriptions +\n";
	for(const std::string& name : order) {
	  const Model& model = _models.at(name);
	  std::string type = "::" + _options.ns + "::" + name;
	  const bool registered = std::any_of(model.fields.begin(), model.fields.end(), [](const Field& field) { return !field.part; });
	  out << "template<>\n"
	      << "struct anch::Mapping<" << type << "> {\n"
	      << "  static void\n"
	      << "  registerObject(" << (registered ? "" : "[[maybe_unused]] ")
	      << "anch::ObjectMapper<" << type << "> auto& mapper) {\n";
	  for(const Field& field : model.fields) {
	    if(field.part) {
	      out << "    // " << commentSafe(field.json) << ": file (multipart/form-data part), not mapped in JSON\n";
	    }
	  }
	  if(!registered) {
	    out << "    // No property\n";
	  } else {
	    out << "    mapper\n";
	    for(const Field& field : model.fields) {
	      if(field.part) {
		continue;
	      }
	      out << "      .registerField(" << literal(field.json) << ", &" << type << "::" << field.member
		  << (field.required ? ", anch::val::required" : "");
	      for(const std::string& constraint : field.constraints) {
		out << ", " << constraint;
	      }
	      out << ")\n";
	    }
	    out << "      ;\n";
	  }
	  out << "  }\n"
	      << "};\n\n";
	}
	out << "// Descriptions -\n";
      }
      // Descriptions -
    }

    void
    writeEnumMapper(std::ostream& out, const Enumeration& enumeration) const {
      std::string type = "::" + _options.ns + "::" + enumeration.name;
      std::string mapper = "PrimitiveMapper<" + type + ">";
      out << "  // " << enumeration.name << " mapper +\n"
	  << "  template<>\n"
	  << "  " << mapper << "::PrimitiveMapper():\n"
	  << "    anch::json::GenericMapper<" << mapper << "," << type << ">() {\n"
	  << "    // Nothing to do\n"
	  << "  }\n\n"
	  << "  template<>\n"
	  << "  " << mapper << "::~PrimitiveMapper() {\n"
	  << "    // Nothing to do\n"
	  << "  }\n\n"
	  << "  template<>\n"
	  << "  void\n"
	  << "  " << mapper << "::serializeValue(const " << type << "& value, anch::json::WriterContext& context) const {\n"
	  << "    std::string text;\n"
	  << "    switch(value) {\n";
      for(const auto& [value, identifier] : enumeration.values) {
	out << "    case " << type << "::" << identifier << ":\n"
	    << "      text = " << literal(value) << ";\n"
	    << "      break;\n";
      }
      out << "    }\n"
	  << "    anch::json::Factory<std::string>::getInstance().serialize(text, context);\n"
	  << "  }\n\n"
	  << "  template<>\n"
	  << "  bool\n"
	  << "  " << mapper << "::deserializeValue(" << type << "& value, anch::json::ReaderContext& context) const {\n"
	  << "    static const std::map<std::string," << type << "> VALUES = {\n";
      for(const auto& [value, identifier] : enumeration.values) {
	out << "      {" << literal(value) << ", " << type << "::" << identifier << "},\n";
      }
      out << "    };\n"
	  << "    std::string text;\n"
	  << "    if(!anch::json::lexString(text, context)) {\n"
	  << "      return false;\n"
	  << "    }\n"
	  << "    auto iter = VALUES.find(text);\n"
	  << "    if(iter == VALUES.end()) {\n"
	  << "      throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, context, \"Invalid "
	  << enumeration.name << " value '\" + text + \"'\");\n"
	  << "    }\n"
	  << "    value = iter->second;\n"
	  << "    return true;\n"
	  << "  }\n\n"
	  << "  template class " << mapper << ";\n"
	  << "  // " << enumeration.name << " mapper -\n\n";
    }

    void
    writeSource(std::ostream& out) const {
      writeBanner(out);
      out << "#include \"" << _options.name << ".hpp\"\n\n"
	  << "#include <map>\n"
	  << "#include <string>\n\n"
	  << "#include \"json/json.hpp\"\n"
	  << "#include \"json/lexer.hpp\"\n"
	  << "#include \"json/mappingError.hpp\"\n"
	  << "#include \"json/primitiveMapper.hpp\"\n";
      // Enumerations mappers +
      if(!_enums.empty()) {
	out << "\n// Enumerations +\n"
	    << "namespace anch::json {\n\n";
	for(const auto& [name, enumeration] : _enums) {
	  writeEnumMapper(out, enumeration);
	}
	out << "}  // anch::json\n"
	    << "// Enumerations -\n";
      }
      // Enumerations mappers -
    }

  };

}  // anch::oas::detail
