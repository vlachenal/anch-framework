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


namespace anch {
  namespace json {

    // Generic implementations +
    template<typename T>
    ObjectMapper<T>::ObjectMapper(): _writers(), _readers() {
      anch::json::registerFields<T>(*this);
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
      _writers.push_back(std::function<bool(const T&, std::ostream&)>([=, this](const T& obj, std::ostream& out) -> bool {
	if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(obj.*value, out, std::optional<std::string>(key));
	  } else { // Use 'unpointered' type
	    return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	  }

	} else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_reference<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(obj.*value, out, std::optional<std::string>(key));
	  } else { // Use 'unreferenced' type
	    return anch::json::Factory<typename std::decay<P>::type>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	  }

	} else { // Basic type
	  if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(obj.*value, out, std::optional<std::string>(key));
	  } else {
	    return anch::json::Factory<P>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	  }
	}
      }));
      _readers[key] = std::function<void(T&, std::istream&)>([=, this](T& obj, std::istream& input) -> void {
	static_assert(!std::is_reference<P>::value, "Can not desrialize reference value"); // raise compile-time error if value is a reference
	if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->deserialize(obj.*value, input);
	  } else { // Use 'unpointered' type
	    return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().deserialize(obj.*value, input);
	  }

	} else { // Basic type
	  if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->deserialize(obj.*value, input);
	  } else {
	    return anch::json::Factory<P>::getInstance().deserialize(obj.*value, input);
	  }
	}
      });
      return *this;
    }

    template<typename T>
    template<typename MT, typename P>
    ObjectMapper<T>&
    ObjectMapper<T>::registerField(const std::string& key, P T::* value) {
      _writers.push_back(std::function<bool(const T&, std::ostream&)>([=, this](const T& obj, std::ostream& out) -> bool {
	if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->serialize(obj.*value, out, std::optional<std::string>(key));
	} else {
	  return anch::json::Factory<MT>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	}
      }));
      _readers[key] = std::function<void(T&, std::istream&)>([=, this](T& obj, std::istream& input) -> void {
	if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	  return this->deserialize(obj.*value, input);
	} else {
	  return anch::json::Factory<MT>::getInstance().deserialize(obj.*value, input);
	}
      });
      return *this;
    }

    template<typename T>
    template<typename P, typename MT>
    ObjectMapper<T>&
    ObjectMapper<T>::registerField(const std::string& key, std::function<P(const T&)> getter) {
      _writers.push_back(std::function<bool(const T&, std::ostream&)>([=, this](const T& obj, std::ostream& out) -> bool {
	if constexpr (std::is_same<MT, P>::value) { // Mapper type has not been specified

	  if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	    if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    } else { // Use 'unpointered' type
	      return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    }

	  } else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	    if constexpr (std::is_same<T, typename std::remove_reference<P>>::value) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    } else { // Use 'unreferenced' type
	      return anch::json::Factory<typename std::decay<P>::type>::getInstance().serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    }

	  } else { // Basic type
	    if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    } else {
	      return anch::json::Factory<MT>::getInstance().serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    }
	  }

	} else { // Parameter type has to be correctly set by caller when specified
	  if constexpr (std::is_same<MT, T>::value) {
	    return this->serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	  } else {
	    return anch::json::Factory<MT>::getInstance().serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	  }
	}
      }));
      return *this;
    }

    template<typename T>
    template<typename P, typename MT>
    ObjectMapper<T>&
    ObjectMapper<T>::registerField(const std::string& key, std::function<void(T&, const P&)> setter) {
      _readers[key] = std::function<void(T&, std::istream&)>([=, this](T& obj, std::istream& input) -> void {
	static_assert(!std::is_reference<P>::value, "Can not desrialize reference value"); // raise compile-time error if value is a reference
	if constexpr (std::is_same<MT, P>::value) { // Mapper type has not been specified

	  if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	    if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	      T* inst = new T();
	      this->deserialize(inst, input);
	      std::invoke(setter, obj, inst);
	    } else { // Use 'unpointered' type
	      P* inst = new P();
	      anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().deserialize(inst, input);
	      std::invoke(setter, obj, inst);
	    }

	  } else { // Basic type
	    if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	      T inst;
	      this->deserialize(inst, input);
	      std::invoke(setter, obj, inst);
	    } else {
	      P inst;
	      anch::json::Factory<P>::getInstance().deserialize(inst, input);
	      std::invoke(setter, obj, inst);
	    }
	  }

	} else { // Parameter type has to be correctly set by caller when specified
	  if constexpr (std::is_same<MT, T>::value) {
	    T inst;
	    this->deserialize(inst, input);
	    std::invoke(setter, obj, inst);
	  } else {
	    MT inst;
	    anch::json::Factory<MT>::getInstance().deserialize(inst, input);
	    std::invoke(setter, obj, inst);
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
    ObjectMapper<T>::serializeValue(const T& value, std::ostream& out) {
      out << anch::json::OBJECT_BEGIN;
      auto iter = _writers.begin();
      while(true) {
	bool added = std::invoke(*iter, value, out);
	if(++iter == _writers.end()) {
	  break;
	}
	if(added) {
	  out << anch::json::FIELD_SEPARATOR;
	}
      }
      out << anch::json::OBJECT_END;
    }

    template<typename T>
    bool
    ObjectMapper<T>::serialize(const T& value, std::ostream& out, const std::optional<std::string>& field) {
      return anch::json::serialize<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), field);
    }

    template<typename T>
    bool
    ObjectMapper<T>::serialize(const T* const value, std::ostream& out, const std::optional<std::string>& field) {
      return anch::json::serialize<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), field);
    }

    template<typename T>
    bool
    ObjectMapper<T>::serialize(const std::optional<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      return anch::json::serialize<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), field);
    }

    template<typename T>
    bool
    ObjectMapper<T>::serialize(const std::vector<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      anch::json::serializeArray<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), field);
      return true;
    }

    template<typename T>
    bool
    ObjectMapper<T>::serialize(const std::list<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      anch::json::serializeArray<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), field);
      return true;
    }

    template<typename T>
    bool
    ObjectMapper<T>::serialize(const std::set<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      anch::json::serializeArray<T>(value, out, std::bind_front(&ObjectMapper<T>::serializeValue, this), field);
      return true;
    }

    template<typename T>
    void
    ObjectMapper<T>::deserializeValue(T& value, std::istream& input) {
      char expected = anch::json::OBJECT_BEGIN;
      int current = '\0';
      while(input) {
	current = input.get();
	if(current != expected) {
	  throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
	}
	if(current == anch::json::OBJECT_BEGIN) {
	  expected = anch::json::OBJECT_END;
	  std::optional<std::string> fieldName = anch::json::getFieldName(input);
	  while(fieldName.has_value()) {
	    auto iter = _readers.find(fieldName.value());
	    if(iter == _readers.end()) {
	      throw anch::json::MappingError(anch::json::ErrorCode::UNEXPECTED_FIELD, input, fieldName.value());
	    } else {
	      std::invoke(iter->second, value, input);
	    }
	    if(!anch::json::hasMoreField(input)) {
	      break;
	    }
	    fieldName = anch::json::getFieldName(input);
	  }
	}
	anch::json::discardChars(input);
	current = input.get();
	if(current == anch::json::OBJECT_END) {
	  break;
	}
      }
      if(current != anch::json::OBJECT_END) {
	throw anch::json::MappingError(anch::json::ErrorCode::INVALID_FORMAT, input, static_cast<char>(current));
      }
    }

    template<typename T>
    void
    ObjectMapper<T>::deserialize(T& value, std::istream& input) {
      anch::json::deserialize<T>(value, input, std::bind_front(&ObjectMapper<T>::deserializeValue, this));
    }

    template<typename T>
    void
    ObjectMapper<T>::deserialize(std::optional<T>& value, std::istream& input) {
      anch::json::deserialize<T>(value, input, std::bind_front(&ObjectMapper<T>::deserializeValue, this));
    }

    template<typename T>
    void
    ObjectMapper<T>::deserialize(T* value, std::istream& input) {
      anch::json::deserialize<T>(value, input, std::bind_front(&ObjectMapper<T>::deserializeValue, this));
    }

    template<typename T>
    void
    ObjectMapper<T>::deserialize(std::vector<T>& value, std::istream& input) {
      anch::json::deserializeArray<T>(input,
				      [&value](const T& obj) -> void { value.push_back(obj); },
				      std::bind_front(&ObjectMapper<T>::deserializeValue, this));
    }

    template<typename T>
    void
    ObjectMapper<T>::deserialize(std::list<T>& value, std::istream& input) {
      anch::json::deserializeArray<T>(input,
				      [&value](const T& obj) -> void { value.push_back(obj); },
				      std::bind_front(&ObjectMapper<T>::deserializeValue, this));
    }

    template<typename T>
    void
    ObjectMapper<T>::deserialize(std::set<T>& value, std::istream& input) {
      anch::json::deserializeArray<T>(input,
				      [&value](const T& obj) -> void { value.insert(obj); },
				      std::bind_front(&ObjectMapper<T>::deserializeValue, this));
    }
    // Generic implementations -

  }  // json
}  // anch
