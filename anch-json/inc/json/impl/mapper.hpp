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
#pragma once

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <functional>
#include <type_traits>

#include "json/constants.hpp"
#include "json/mappingFunctions.hpp"
#include "json/mappingError.hpp"
#include "json/lexer.hpp"


namespace anch::json {

  // Generic implementations +
  template<typename T>
  ObjectMapper<T>::ObjectMapper(): anch::json::GenericMapper<ObjectMapper<T>, T>(), _writers(), _readers() {
    anch::json::registerObject<T>(*this);
    _writers.shrink_to_fit();
  }

  template<typename T>
  ObjectMapper<T>::~ObjectMapper() {
    // Nothing to do
  }

  template<typename T>
  template<typename P>
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, P T::* value) {
    static_assert(!std::is_reference<P>::value, "Can not deserialize reference value"); // raise compile-time error if value is a reference
    _writers.push_back(std::function<bool(const T&, std::ostream&, const anch::json::MappingOptions&)>([=, this](const T& obj, std::ostream& out, const anch::json::MappingOptions& options) -> bool {
      if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->serialize(obj.*value, out, options, std::optional<std::string>(key));
	} else { // Use 'unpointered' type
	  return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().serialize(obj.*value, out, options, std::optional<std::string>(key));
	}

      } else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	if constexpr (std::is_same<T, typename std::remove_reference<P>>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->serialize(obj.*value, out, options, std::optional<std::string>(key));
	} else { // Use 'unreferenced' type
	  return anch::json::Factory<typename std::decay<P>::type>::getInstance().serialize(obj.*value, out, options, std::optional<std::string>(key));
	}

      } else { // Basic type
	if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->serialize(obj.*value, out, options, std::optional<std::string>(key));
	} else {
	  return anch::json::Factory<P>::getInstance().serialize(obj.*value, out, options, std::optional<std::string>(key));
	}
      }
    }));
    _readers[key] = anch::json::DeserializeFn<T>([=, this](T& obj, anch::json::ReaderContext& context) -> bool {
      if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	using RP = typename std::remove_pointer<P>::type;
	obj.*value = new RP();
	if constexpr (std::is_same<T, RP>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->deserialize(*(obj.*value), context);
	} else { // Use 'unpointered' type
	  return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().deserialize(*(obj.*value), context);
	}

      } else { // Basic type
	if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->deserialize(obj.*value, context);
	} else {
	  return anch::json::Factory<P>::getInstance().deserialize(obj.*value, context);
	}
      }
    });
    return *this;
  }

  template<typename T>
  template<typename MT, typename P>
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, P T::* value) {
    _writers.push_back(std::function<bool(const T&, std::ostream&, const anch::json::MappingOptions&)>([=, this](const T& obj, std::ostream& out, const anch::json::MappingOptions& options) -> bool {
      if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	return this->serialize(obj.*value, out, options, std::optional<std::string>(key));
      } else {
	return anch::json::Factory<MT>::getInstance().serialize(obj.*value, out, options, std::optional<std::string>(key));
      }
    }));
    _readers[key] = anch::json::DeserializeFn<T>([=, this](T& obj, anch::json::ReaderContext& context) -> bool {
      if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	return this->deserialize(obj.*value, context);
      } else {
	return anch::json::Factory<MT>::getInstance().deserialize(obj.*value, context);
      }
    });
    return *this;
  }

  template<typename T>
  template<typename P, typename MT>
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, std::function<P(const T&)> getter) {
    _writers.push_back(std::function<bool(const T&, std::ostream&, const anch::json::MappingOptions&)>([=, this](const T& obj, std::ostream& out, const anch::json::MappingOptions& options) -> bool {
      if constexpr (std::is_same<MT, P>::value) { // Mapper type has not been specified

	if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	  } else { // Use 'unpointered' type
	    return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	  }

	} else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_reference<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	  } else { // Use 'unreferenced' type
	    return anch::json::Factory<typename std::decay<P>::type>::getInstance().serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	  }

	} else { // Basic type
	  if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	  } else {
	    return anch::json::Factory<MT>::getInstance().serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	  }
	}

      } else { // Parameter type has to be correctly set by caller when specified
	if constexpr (std::is_same<MT, T>::value) {
	  return this->serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	} else {
	  return anch::json::Factory<MT>::getInstance().serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	}
      }
    }));
    return *this;
  }

  template<typename T>
  template<typename P, typename MT>
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, std::function<void(T&, const P&)> setter) {
    static_assert(!std::is_reference<P>::value, "Can not deserialize reference value"); // raise compile-time error if value is a reference
    _readers[key] = anch::json::DeserializeFn<T>([=, this](T& obj, anch::json::ReaderContext& context) -> bool {
      if constexpr (std::is_same<MT, P>::value) { // Mapper type has not been specified

	if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    T* inst = new T();
	    if(this->deserialize(*inst, context)) {
	      std::invoke(setter, obj, inst);
	      return true;
	    }
	    return false;
	  } else { // Use 'unpointered' type
	    P* inst = new P();
	    if(anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().deserialize(*inst, context)) {
	      std::invoke(setter, obj, inst);
	      return true;
	    }
	    return false;
	  }

	} else { // Basic type
	  if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	    T inst;
	    if(this->deserialize(inst, context)) {
	      std::invoke(setter, obj, inst);
	      return true;
	    }
	    return false;
	  } else {
	    P inst;
	    if(anch::json::Factory<P>::getInstance().deserialize(inst, context)) {
	      std::invoke(setter, obj, inst);
	      return true;
	    }
	    return false;
	  }
	}

      } else { // Parameter type has to be correctly set by caller when specified
	if constexpr (std::is_same<MT, T>::value) {
	  T inst;
	  if(this->deserialize(inst, context)) {
	    std::invoke(setter, obj, inst);
	    return true;
	  }
	  return false;
	} else {
	  MT inst;
	  if(anch::json::Factory<MT>::getInstance().deserialize(inst, context)) {
	    std::invoke(setter, obj, inst);
	    return true;
	  }
	  return false;
	}
      }
    });
    return *this;
  }

  template<typename T>
  template<typename P, typename MT>
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, std::function<P(const T&)> getter, std::function<void(T&, const P&)> setter) {
    registerField<P,MT>(key, getter);
    return registerField<P,MT>(key, setter);
  }

  template<typename T>
  void
  ObjectMapper<T>::serializeValue(const T& value, std::ostream& out, const anch::json::MappingOptions& options) {
    out.put(anch::json::OBJECT_BEGIN);
    auto iter = _writers.begin();
    while(true) {
      bool added = std::invoke(*iter, value, out, options);
      if(++iter == _writers.end()) {
	break;
      }
      if(added) {
	out.put(anch::json::FIELD_SEPARATOR);
      }
    }
    out.put(anch::json::OBJECT_END);
  }

  template<typename T>
  bool
  ObjectMapper<T>::serialize(const T& value, std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field) {
    return anch::json::serialize<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), options, field);
  }

  template<typename T>
  bool
  ObjectMapper<T>::serialize(const T* const value, std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field) {
    return anch::json::serialize<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), options, field);
  }

  template<typename T>
  bool
  ObjectMapper<T>::serialize(const std::optional<T>& value, std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field) {
    return anch::json::serialize<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), options, field);
  }

  template<typename T>
  bool
  ObjectMapper<T>::serialize(const std::vector<T>& value, std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field) {
    anch::json::serializeArray<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), options, field);
    return true;
  }

  template<typename T>
  bool
  ObjectMapper<T>::serialize(const std::list<T>& value, std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field) {
    anch::json::serializeArray<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), options, field);
    return true;
  }

  template<typename T>
  bool
  ObjectMapper<T>::serialize(const std::set<T>& value, std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field) {
    anch::json::serializeArray<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), options, field);
    return true;
  }

  template<typename T>
  bool
  ObjectMapper<T>::serialize(const std::map<std::string,T>& value, std::ostream& out, const anch::json::MappingOptions& options, const std::optional<std::string>& field) {
    anch::json::serializeMap<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), options, field);
    return true;
  }

  template<typename T>
  bool
  ObjectMapper<T>::deserializeValue(T& value, anch::json::ReaderContext& context) const {
    if(!anch::json::objectHasValueLex(context)) {
      return false;
    }
    std::map<std::string, anch::json::LexFunc> lexers;
    for(auto iter = _readers.cbegin() ; iter != _readers.cend() ; ++iter) {
      lexers[iter->first] = std::move(std::bind_front(iter->second, std::ref(value)));
    }
    std::set<std::string> fields; // found fields for validation
    anch::json::lexObject(lexers, fields, context);
    return true;
  }
  // Generic implementations -

}  // anch::json
