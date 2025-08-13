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
#include <set>
#include <map>
#include <functional>
#include <type_traits>

#include "json/constants.hpp"
#include "json/mappingError.hpp"
#include "json/lexer.hpp"


namespace anch::json {

  // Generic implementations +
  // Constructors +
  template<typename T>
  ObjectMapper<T>::ObjectMapper(): anch::json::GenericMapper<ObjectMapper<T>, T>(), _writers(), _readers() {
    anch::json::registerObject<T>(*this);
    _writers.shrink_to_fit();
  }
  // Constructors -

  // Destructor +
  template<typename T>
  ObjectMapper<T>::~ObjectMapper() {
    // Nothing to do
  }
  // Destructor -

  // Fields' registration +
  template<typename T>
  template<typename P>
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, P T::* value) {
    static_assert(!std::is_reference<P>::value, "Can not deserialize reference value"); // raise compile-time error if value is a reference
    _writers.push_back(std::function<bool(const T&, anch::json::WriterContext&)>([=, this](const T& obj, anch::json::WriterContext& context) -> bool {
      if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->serialize(key, obj.*value, context);
	} else { // Use 'unpointered' type
	  return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().serialize(key, obj.*value, context);
	}

      } else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	if constexpr (std::is_same<T, typename std::remove_reference<P>>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->serialize(key, obj.*value, context);
	} else { // Use 'unreferenced' type
	  return anch::json::Factory<typename std::decay<P>::type>::getInstance().serialize(key, obj.*value, context);
	}

      } else { // Basic type
	if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->serialize(key, obj.*value, context);
	} else {
	  return anch::json::Factory<P>::getInstance().serialize(key, obj.*value, context);
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
    _writers.push_back(std::function<bool(const T&, anch::json::WriterContext&)>([=, this](const T& obj, anch::json::WriterContext& context) -> bool {
      if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	return this->serialize(key, obj.*value, context);
      } else {
	return anch::json::Factory<MT>::getInstance().serialize(key, obj.*value, context);
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
    _writers.push_back(std::function<bool(const T&, anch::json::WriterContext&)>([=, this](const T& obj, anch::json::WriterContext& context) -> bool {
      if constexpr (std::is_same<MT, P>::value) { // Mapper type has not been specified

	if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    //return this->serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	    return this->serialize(key, std::invoke(getter, obj), context);
	  } else { // Use 'unpointered' type
	    return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().serialize(key, std::invoke(getter, obj), context);
	  }

	} else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_reference<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    //return this->serialize(std::invoke(getter, obj), out, options, std::optional<std::string>(key));
	    return this->serialize(key, std::invoke(getter, obj), context);
	  } else { // Use 'unreferenced' type
	    return anch::json::Factory<typename std::decay<P>::type>::getInstance().serialize(key, std::invoke(getter, obj), context);
	  }

	} else { // Basic type
	  if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(key, std::invoke(getter, obj), context);
	  } else {
	    return anch::json::Factory<MT>::getInstance().serialize(key, std::invoke(getter, obj), context);
	  }
	}

      } else { // Parameter type has to be correctly set by caller when specified
	if constexpr (std::is_same<MT, T>::value) {
	  return this->serialize(key, std::invoke(getter, obj), context);
	} else {
	  return anch::json::Factory<MT>::getInstance().serialize(key, std::invoke(getter, obj), context);
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
  // Fields' registration -

  // Methods +
  template<typename T>
  void
  ObjectMapper<T>::serializeValue(const T& value, anch::json::WriterContext& context) const {
    context.output.put(anch::json::OBJECT_BEGIN);
    auto iter = _writers.begin();
    while(true) {
      bool added = std::invoke(*iter, value, context);
      if(++iter == _writers.end()) {
	break;
      }
      if(added) {
	context.output.put(anch::json::FIELD_SEPARATOR);
      }
    }
    context.output.put(anch::json::OBJECT_END);
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
  // Methods -
  // Generic implementations -

}  // anch::json
