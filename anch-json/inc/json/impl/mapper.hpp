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
#include <ostream>
#include <functional>

#include "json/constants.hpp"
#include "json/mapper.hpp"

#include <iostream> // \todo remove


namespace anch {
  namespace json {

    // JSON mapper early declaration
    template<typename T>
    class JSONMapper;

    // Generic implementations +
    template<typename T>
    JSONMapper<T>::JSONMapper(): writers(), readers() {
      anch::json::registerFields<T>(*this);
    }

    template<typename T>
    JSONMapper<T>::~JSONMapper() {
      // Nothing to do
    }

    template<typename T>
    inline
    const std::vector<std::function<bool(const T&, std::ostream&)>>&
    JSONMapper<T>::getWritterMapping() const {
      return writers;
    }

    template<typename T>
    inline
    const std::map<std::string, std::function<void(T&, std::istream&)>>&
    JSONMapper<T>::getReaderMapping() const {
      return readers;
    }

    template<typename T1, typename T2>
    constexpr bool isSame() {
      return std::is_same<T1,T2>();
    }

    template<typename T>
    bool callMappingFunctionRef(const T& value, std::ostream& out, const std::string& key) {
      //return JSONMapper<T>::getInstance().serialize(value, out, std::optional<std::string>(key));
      return JSONFactory<T>::getInstance().serialize(value, out, std::optional<std::string>(key));
    }

    template<typename T>
    bool callMappingFunctionPtr(const T* const value, std::ostream& out, const std::string& key) {
      //return JSONMapper<T>::getInstance().serialize(value, out, std::optional<std::string>(key));
      return JSONFactory<T>::getInstance().serialize(value, out, std::optional<std::string>(key));
    }

    template<typename T>
    template<typename P, typename MT>
    JSONMapper<T>&
    JSONMapper<T>::registerField(const std::string& key, P T::* value) {
      writers.push_back(std::function<bool(const T&, std::ostream&)>([=, this](const T& obj, std::ostream& out) -> bool {
	std::cout << "Begin execute func key=" << key << std::endl;
	if constexpr (std::is_same<MT, P>::value) { // Mapper type has not been specified
	  std::cout << "Mapper same type key=" << key << std::endl;

	  if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	    std::cout << "Pointer" << std::endl;
	    //auto strType = std::remove_pointer<P>();
	    //if constexpr (std::is_same<T, std::remove_pointer<P>::type>::value) { // if same, use direct call to avoid recursive instanciation
	    if constexpr (isSame<T, std::remove_pointer<P>()>()) { // if same, use direct call to avoid recursive instanciation
	      std::cout << "Pointer same type key=" << key << std::endl;
	      return serialize(obj.*value, out, std::optional<std::string>(key));
	    } else { // Use 'unpointered' type
	      std::cout << "Pointer different type key=" << key << std::endl;
	      //return JSONMapper<std::remove_pointer<P>::type>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	      //return callMappingFunctionPtr<std::remove_pointer<P>()>(obj.*value, out, key);
	      return callMappingFunctionPtr(obj.*value, out, key);
	      //return JSONMapper<std::remove_pointer<P>()>::getInstance().serialize(obj.*value, out, key);
	    }

	  } else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	    std::cout << "Reference key=" << key << std::endl;
	    //auto strType = std::remove_reference<P>::type;
	    if constexpr (isSame<std::remove_reference<P>(), T>()) { // if same, use direct call to avoid recursive instanciation
	      std::cout << "Reference same type key=" << key << std::endl;
	      return serialize(obj.*value, out, std::optional<std::string>(key));
	    } else { // Use 'unreferenced' type
	      std::cout << "Reference different type key=" << key << std::endl;
	      //return JSONMapper<std::remove_reference<P>::type>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	      return callMappingFunctionRef(obj.*value, out, key);
	    }

	  } else { // Basic type
	      std::cout << "Basic key=" << key << std::endl;
	    if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	      std::cout << "Basic same type key=" << key << std::endl;
	      return serialize(obj.*value, out, std::optional<std::string>(key));
	    } else {
	      std::cout << "Basic different type key=" << key << std::endl;
	      //return JSONMapper<MT>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	      return JSONFactory<MT>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	    }
	  }

	} else { // Parameter type has to be correctly set by caller when specified
	  std::cout << "Mapper different type key=" << key << std::endl;
	  if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	    std::cout << "Mapper different type and same type key=" << key << std::endl;
	    return serialize(obj.*value, out, std::optional<std::string>(key));
	  } else {
	    std::cout << "Mapper different type and different type key=" << key << std::endl;
	    //return JSONMapper<MT>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	    return JSONFactory<MT>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	  }
	}
	std::cout << "End execute func key=" << key << std::endl;
      }));
      return *this;
    }

    template<typename T>
    template<typename P, typename MT>
    JSONMapper<T>&
    JSONMapper<T>::registerField(const std::string& key, std::function<P(const T&)> getter) {
      writers.push_back(std::function<bool(const T&, std::ostream&)>([=, this](const T& obj, std::ostream& out) -> bool {
	if constexpr (std::is_same<MT, T>::value) {
	  return serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	} else {
	  //return JSONMapper<MT>::getInstance().serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	  return JSONFactory<MT>::getInstance().serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	}
      }));
      return *this;
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const T& value, std::ostream& out, const std::optional<std::string>& field) {
      std::cout << "Begin serialization " << field.value_or("EMPTY") << std::endl;
      JSONMapper<T>& mapper = JSONFactory<T>::getInstance();
      if(field.has_value()) {
	out << STRING_DELIMITER << field.value() << STRING_DELIMITER << FIELD_VALUE_SEPARATOR;
      }
      std::cout << "Field added" << std::endl;
      out << OBJECT_BEGIN;
      auto iter = mapper.getWritterMapping().begin();
      while(true) {
	std::cout << "Iterate for key " << field.value_or("EMPTY") << std::endl;
	bool added = std::invoke(*iter, value, out);
	if(++iter == mapper.getWritterMapping().end()) {
	  break;
	}
	if(added) {
	  out << FIELD_SEPARATOR;
	}
      }
      out << OBJECT_END;
      std::cout << "End serialization " << field.value_or("EMPTY") << std::endl;
      return true;
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const T* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      return serialize(*value, out, field);
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const std::optional<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      return serialize(value.value(), out, field);
    }

    template<typename T>
    bool
    serializeArrayFromContainer(const T& array, std::ostream& out, const std::optional<std::string>& field) {
      if(field.has_value()) {
	out << STRING_DELIMITER << field.value() << STRING_DELIMITER << FIELD_VALUE_SEPARATOR;
      }
      out << ARRAY_BEGIN;
      for(auto iter = array.begin() ; iter != array.end() ; ++iter) {
	out << JSONMapper<T>::serialize(*iter, out, anch::json::EMPTY_FIELD) << FIELD_SEPARATOR; // \todo last ','
      }
      out << ARRAY_END;
      return true;
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const std::vector<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      return anch::json::serializeArrayFromContainer(value, out, field);
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const std::list<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      return anch::json::serializeArrayFromContainer(value, out, field);
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const std::set<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      return anch::json::serializeArrayFromContainer(value, out, field);
    }
    // Generic implementations -

  }  // json
}  // anch
