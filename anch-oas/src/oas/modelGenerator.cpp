/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "oas/modelGenerator.hpp"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>

using anch::oas::GeneratedModels;
using anch::oas::GeneratorError;
using anch::oas::ModelOptions;
using anch::oas::OpenAPI;
using anch::oas::Scalar;
using anch::oas::Schema;


// Constructors +
GeneratorError::GeneratorError(const std::string& message): std::runtime_error(message) {
  // Nothing to do
}
// Constructors -

namespace {

  /*! Supported references prefix */
  const std::string SCHEMAS_REF = "#/components/schemas/";

  /*! C++ keywords */
  const std::set<std::string> KEYWORDS = {
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
  const std::set<std::string> MACROS = {
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

  bool
  isAlnum(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
  }

  bool
  isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
  }

  bool
  isUpper(char c) {
    return std::isupper(static_cast<unsigned char>(c)) != 0;
  }

  bool
  isLower(char c) {
    return std::islower(static_cast<unsigned char>(c)) != 0;
  }

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
  std::string
  literal(const std::string& value) {
    std::ostringstream oss;
    oss << '"';
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
      default:
	if(static_cast<unsigned char>(c) < 0x20) {
	  oss << '\\' << std::oct << std::setw(3) << std::setfill('0') << static_cast<int>(c) << std::dec;
	} else {
	  oss << c;
	}
      }
    }
    oss << '"';
    return oss.str();
  }

  /*!
   * Text which can be written in a comment
   *
   * \param text the text
   *
   * \return the text without comment end
   */
  std::string
  commentSafe(std::string text) {
    std::size_t pos = 0;
    while((pos = text.find("*/", pos)) != std::string::npos) {
      text.replace(pos, 2, "* /");
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
   * Decode JSON pointer token ( \c ~1 → \c / , \c ~0 → \c ~ )
   *
   * \param token the token
   *
   * \return the decoded token
   */
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
    /*! \c anch::UUID is used */
    bool _uuid = false;
    /*! \c anch::date::Date or \c std::chrono::year_month_day is used */
    bool _date = false;

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
      // Models (inline models are added while building) +
      for(std::size_t i = 0 ; i < _pending.size() ; ++i) {
	auto [name, schema] = _pending[i];
	buildModel(name, *schema);
      }
      // Models -
      std::vector<std::string> order = sortModels();
      GeneratedModels res;
      std::ostringstream header;
      writeHeader(header, order);
      res.header = header.str();
      std::ostringstream source;
      writeSource(source, order);
      res.source = source.str();
      res.warnings = _warnings;
      return res;
    }

  private:
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
    }

    void
    warn(const std::string& path, const std::string& reason) {
      if(_options.strict) {
	throw GeneratorError(path + ": " + reason);
      }
      _warnings.push_back(path + ": " + reason + ": not generated");
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
      std::set<std::string> members;
      for(const auto& [json, property] : composite.properties) {
	try {
	  Field field;
	  field.json = json;
	  field.type = resolveProperty(json, property);
	  std::string member = memberName(json);
	  while(members.contains(member) || _names.contains(member)) {
	    member += '_';
	  }
	  members.insert(member);
	  field.member = member;
	  bool nullable = field.type.nullable || property.schema->nullable.value_or(false);
	  field.optional = (field.type.container == Container::NONE) && (!composite.required.contains(json) || nullable);
	  field.description = document(describe(*property.schema), field.type.remark);
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
	  << "#include \"json/factory.hpp\"\n";
      if(_date) {
	out << "#include \"date/date.hpp\"\n";
      }
      if(_uuid) {
	out << "#include \"uuid.hpp\"\n";
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
	    if(i + 1 < model.fields.size()) {
	      out << '\n';
	    }
	  }
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
      if(!_enums.empty() || !order.empty()) {
	out << "\nnamespace anch::json {\n\n";
	for(const auto& [name, enumeration] : _enums) {
	  out << "  /*! \\c ::" << _options.ns << "::" << name << " has a dedicated mapper (JSON string) */\n"
	      << "  template<>\n"
	      << "  constexpr bool isPrimitive<::" << _options.ns << "::" << name << ">() {\n"
	      << "    return true;\n"
	      << "  }\n\n";
	}
	for(const std::string& name : order) {
	  out << "  /*! \\c ::" << _options.ns << "::" << name << " fields registration */\n"
	      << "  template<>\n"
	      << "  void registerObject(ObjectMapper<::" << _options.ns << "::" << name << ">& mapper);\n\n";
	}
	out << "}  // anch::json\n";
      }
      // anch-json declarations -
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
    writeSource(std::ostream& out, const std::vector<std::string>& order) const {
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
      // Models mappers +
      if(!order.empty()) {
	out << "\n// Models +\n";
	for(const std::string& name : order) {
	  const Model& model = _models.at(name);
	  std::string type = "::" + _options.ns + "::" + name;
	  out << "template<>\n"
	      << "void\n"
	      << "anch::json::registerObject(" << (model.fields.empty() ? "[[maybe_unused]] " : "")
	      << "anch::json::ObjectMapper<" << type << ">& mapper) {\n";
	  if(model.fields.empty()) {
	    out << "  // No property\n";
	  } else {
	    out << "  mapper\n";
	    for(const Field& field : model.fields) {
	      bool exact = !field.optional && !field.pointer && field.type.container == Container::NONE;
	      out << "    .registerField" << (exact ? "" : "<" + qualified(field.type) + ">")
		  << '(' << literal(field.json) << ", &" << type << "::" << field.member << ")\n";
	    }
	    out << "    ;\n";
	  }
	  out << "}\n\n";
	}
	out << "// Models -\n";
      }
      // Models mappers -
    }

  };

}

GeneratedModels
anch::oas::generateModels(const OpenAPI& api, const ModelOptions& options) {
  Generator generator(api, options);
  return generator.generate();
}
