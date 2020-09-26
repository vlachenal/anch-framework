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

#include "json/constants.hpp"


namespace anch {
  namespace json {

    // JSON mapper early declaration
    template<typename T>
    class JSONFactory;

    // JSON mapper early declaration
    template<typename T>
    class JSONMapper;

    // Generic implementations +
    template<typename T>
    JSONMapper<T>::JSONMapper(): _writers(), _readers() {
      anch::json::registerFields<T>(*this);
      _writers.shrink_to_fit();
    }

    template<typename T>
    JSONMapper<T>::~JSONMapper() {
      // Nothing to do
    }

    template<typename T>
    inline
    const std::vector<std::function<bool(const T&, std::ostream&)>>&
    JSONMapper<T>::getWritterMapping() const {
      return _writers;
    }

    template<typename T>
    inline
    const std::map<std::string, std::function<void(T&, std::istream&)>>&
    JSONMapper<T>::getReaderMapping() const {
      return _readers;
    }

    /*!
     * Check if types are same because type function does not returns a type ?! (GCC bug ?)
     *
     * \tparam T1 the first type
     * \tparam T2 the second type
     *
     * \return \c true if types are same, \c false otherwise
     */
    template<typename T1, typename T2>
    constexpr bool isSame() {
      return std::is_same<T1,T2>();
    }

    /*!
     * Call mapping function with value reference because type function does not returns a type ?! (GCC bug ?)
     *
     * \tparam T the type to serialize
     *
     * \param value the value reference
     * \param out the output stream to write in
     * \param key the field key
     *
     * \return \c true if value has been serialized, \c false otherwise
     */
    template<typename T>
    bool callMappingFunctionRef(const T& value, std::ostream& out, const std::string& key) {
      return JSONFactory<T>::getInstance().serialize(value, out, std::optional<std::string>(key));
    }

    /*!
     * Call mapping function with value pointer because type function does not returns a type ?! (GCC bug ?)
     *
     * \tparam T the type to serialize
     *
     * \param value the value pointer
     * \param out the output stream to write in
     * \param key the field key
     *
     * \return \c true if value has been serialized, \c false otherwise
     */
    template<typename T>
    bool callMappingFunctionPtr(const T* const value, std::ostream& out, const std::string& key) {
      return JSONFactory<T>::getInstance().serialize(value, out, std::optional<std::string>(key));
    }

    template<typename T>
    template<typename P, typename MT>
    JSONMapper<T>&
    JSONMapper<T>::registerField(const std::string& key, P T::* value) {
      _writers.push_back(std::function<bool(const T&, std::ostream&)>([=, this](const T& obj, std::ostream& out) -> bool {
	if constexpr (std::is_same<MT, P>::value) { // Mapper type has not been specified

	  if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	    if constexpr (isSame<T, std::remove_pointer<P>()>()) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(obj.*value, out, std::optional<std::string>(key));
	    } else { // Use 'unpointered' type
	      return callMappingFunctionPtr(obj.*value, out, key);
	    }

	  } else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	    if constexpr (isSame<std::remove_reference<P>(), T>()) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(obj.*value, out, std::optional<std::string>(key));
	    } else { // Use 'unreferenced' type
	      return callMappingFunctionRef(obj.*value, out, key);
	    }

	  } else { // Basic type
	    if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(obj.*value, out, std::optional<std::string>(key));
	    } else {
	      return JSONFactory<MT>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	    }
	  }

	} else { // Parameter type has to be correctly set by caller when specified
	  if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(obj.*value, out, std::optional<std::string>(key));
	  } else {
	    return JSONFactory<MT>::getInstance().serialize(obj.*value, out, std::optional<std::string>(key));
	  }
	}
      }));
      return *this;
    }

    template<typename T>
    template<typename P, typename MT>
    JSONMapper<T>&
    JSONMapper<T>::registerField(const std::string& key, std::function<P(const T&)> getter) {
      _writers.push_back(std::function<bool(const T&, std::ostream&)>([=, this](const T& obj, std::ostream& out) -> bool {
	if constexpr (std::is_same<MT, P>::value) { // Mapper type has not been specified

	  if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	    if constexpr (isSame<T, std::remove_pointer<P>()>()) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    } else { // Use 'unpointered' type
	      return callMappingFunctionPtr(std::invoke(getter, obj), out, key);
	    }

	  } else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	    if constexpr (isSame<std::remove_reference<P>(), T>()) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    } else { // Use 'unreferenced' type
	      return callMappingFunctionRef(std::invoke(getter, obj), out, key);
	    }

	  } else { // Basic type
	    if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	      return this->serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    } else {
	      return JSONFactory<MT>::getInstance().serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	    }
	  }

	} else { // Parameter type has to be correctly set by caller when specified
	  if constexpr (std::is_same<MT, T>::value) {
	    return this->serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	  } else {
	    return JSONFactory<MT>::getInstance().serialize(std::invoke(getter, obj), out, std::optional<std::string>(key));
	  }
	}
      }));
      return *this;
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const T& value, std::ostream& out, const std::optional<std::string>& field) {
      JSONMapper<T>& mapper = JSONFactory<T>::getInstance();
      if(field.has_value()) {
	out << STRING_DELIMITER << field.value() << STRING_DELIMITER << FIELD_VALUE_SEPARATOR;
      }
      out << OBJECT_BEGIN;
      auto iter = mapper.getWritterMapping().begin();
      while(true) {
	bool added = std::invoke(*iter, value, out);
	if(++iter == mapper.getWritterMapping().end()) {
	  break;
	}
	if(added) {
	  out << FIELD_SEPARATOR;
	}
      }
      out << OBJECT_END;
      return true;
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const T* const value, std::ostream& out, const std::optional<std::string>& field) {
      if(value == NULL) {
	return false;
      }
      return this->serialize(*value, out, field);
    }

    template<typename T>
    bool
    JSONMapper<T>::serialize(const std::optional<T>& value, std::ostream& out, const std::optional<std::string>& field) {
      if(!value.has_value()) {
	return false;
      }
      return this->serialize(value.value(), out, field);
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
