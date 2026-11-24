/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "json/constants.hpp"
#include "json/mappingError.hpp"
#include "json/lexer.hpp"


namespace anch::json {

  // Generic implementations +
  // Constructors +
  template<typename T>
  ObjectMapper<T>::ObjectMapper(): anch::json::GenericMapper<ObjectMapper<T>, T>(), _writers(), _readers(), _required() {
    static_assert(anch::Described<T>, "T has to be described by an anch::Mapping<T> specialization (mapping.hpp), declared in"
		  " the header of T");
    anch::describe<T>(*this);
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
  template<typename G, typename W, typename... C>
  void
  ObjectMapper<T>::addWriter(const std::string& key, G get, W write, const C&... constraints) {
    if constexpr (anch::val::hasValueConstraints<C...>() || anch::val::isRequired<C...>()) {
      _writers.push_back(FieldWriter{key, WriteFn([key, get, write, ...cts = constraints](const T& obj, anch::json::WriterContext& context) -> bool {
	decltype(auto) val = std::invoke(get, obj);
	if constexpr (anch::val::hasValueConstraints<C...>()) {
	  anch::val::detail::checkConstraints(val, context, cts...);
	} else {
	  ((void)cts, ...);
	}
	if constexpr (anch::val::isRequired<C...>()) {
	  if(anch::json::detail::writeEmptyRequired(key, val, context)) {
	    return true;
	  }
	} else {
	  (void)key;
	}
	return write(val, context);
      })});
    } else {
      _writers.push_back(FieldWriter{key, WriteFn([get, write](const T& obj, anch::json::WriterContext& context) -> bool {
	return write(std::invoke(get, obj), context);
      })});
    }
  }

  template<typename T>
  void
  ObjectMapper<T>::addReader(const std::string& key, anch::json::DeserializeFn<T>&& reader, bool required, bool nullable) {
    FieldReader& field = _readers[key];
    field.read = std::move(reader);
    if(required) {
      if(field.required == NOT_REQUIRED) {
	field.required = _required.size();
	_required.push_back(key);
      }
      field.rejectNull = !nullable;
    }
  }

  template<typename T>
  template<typename P, typename... C>
  requires (anch::val::FieldConstraint<C,P> && ...)
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, P T::* value, [[maybe_unused]] C&&... constraints) {
    static_assert(!std::is_reference<P>::value, "Can not deserialize reference value"); // raise compile-time error if value is a reference
    if constexpr (!std::is_same_v<anch::json::detail::MapperType<P>, P>) {
      // std::optional, pointers, collections and maps: mapper of the value type
      return this->template registerMapped<anch::json::detail::MapperType<P>>(key, value, std::forward<C>(constraints)...);
    } else {
      auto write = [=, this](const auto& val, anch::json::WriterContext& context) -> bool {
	static_cast<void>(this); // used by some instantiations only
	if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(key, val, context);
	  } else { // Use 'unpointered' type
	    return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().serialize(key, val, context);
	  }

	} else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_reference<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(key, val, context);
	  } else { // Use 'unreferenced' type
	    return anch::json::Factory<typename std::decay<P>::type>::getInstance().serialize(key, val, context);
	  }

	} else { // Basic type
	  if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(key, val, context);
	  } else {
	    return anch::json::Factory<P>::getInstance().serialize(key, val, context);
	  }
	}
      };
      addWriter(key, [value](const T& obj) -> const P& { return obj.*value; }, write, constraints...);
      auto read = [=, this](T& obj, anch::json::ReaderContext& context) -> bool {
	static_cast<void>(this); // used by some instantiations only
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
      };
      addReader(key, withConstraints(read, value, std::forward<C>(constraints)...), anch::val::isRequired<C...>(), anch::val::isNullable<P>());
      return *this;
    }
  }

  template<typename T>
  template<typename MT, typename P, typename... C>
  ObjectMapper<T>&
  ObjectMapper<T>::registerMapped(const std::string& key, P T::* value, [[maybe_unused]] C&&... constraints) {
    auto write = [=, this](const auto& val, anch::json::WriterContext& context) -> bool {
      static_cast<void>(this); // used by some instantiations only
      if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	return this->serialize(key, val, context);
      } else {
	return anch::json::Factory<MT>::getInstance().serialize(key, val, context);
      }
    };
    addWriter(key, [value](const T& obj) -> const P& { return obj.*value; }, write, constraints...);
    auto read = [=, this](T& obj, anch::json::ReaderContext& context) -> bool {
      static_cast<void>(this); // used by some instantiations only
      if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	return this->deserialize(obj.*value, context);
      } else {
	return anch::json::Factory<MT>::getInstance().deserialize(obj.*value, context);
      }
    };
    addReader(key, withConstraints(read, value, std::forward<C>(constraints)...), anch::val::isRequired<C...>(), anch::val::isNullable<P>());
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  requires (anch::val::FieldConstraint<C,P> && ...)
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, std::function<P(const T&)> getter, const C&... constraints) {
    using MT = anch::json::detail::MapperType<P>;
    auto write = [=, this](const auto& val, anch::json::WriterContext& context) -> bool {
      static_cast<void>(this); // used by some instantiations only
      if constexpr (std::is_same<MT, P>::value) { // Field of its value type

	if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(key, val, context);
	  } else { // Use 'unpointered' type
	    return anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().serialize(key, val, context);
	  }

	} else if constexpr (std::is_reference<P>::value) { // Remove reference on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_reference<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(key, val, context);
	  } else { // Use 'unreferenced' type
	    return anch::json::Factory<typename std::decay<P>::type>::getInstance().serialize(key, val, context);
	  }

	} else { // Basic type
	  if constexpr (std::is_same<MT, T>::value) { // if same, use direct call to avoid recursive instanciation
	    return this->serialize(key, val, context);
	  } else {
	    return anch::json::Factory<MT>::getInstance().serialize(key, val, context);
	  }
	}

      } else { // std::optional, pointers, collections and maps: mapper of the value type
	if constexpr (std::is_same<MT, T>::value) {
	  return this->serialize(key, val, context);
	} else {
	  return anch::json::Factory<MT>::getInstance().serialize(key, val, context);
	}
      }
    };
    addWriter(key, getter, write, constraints...);
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  requires (anch::val::FieldConstraint<C,P> && ...)
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, std::function<void(T&, const P&)> setter, [[maybe_unused]] C&&... constraints) {
    using MT = anch::json::detail::MapperType<P>;
    static_assert(!std::is_reference<P>::value, "Can not deserialize reference value"); // raise compile-time error if value is a reference
    // Check value constraints before calling setter +
    auto set = [setter, ...cts = std::forward<C>(constraints)](T& obj, const auto& inst, anch::json::ReaderContext& context) -> void {
      if constexpr (anch::val::hasValueConstraints<C...>()) {
	anch::val::detail::checkConstraints(inst, context, cts...);
      } else {
	((void)cts, ...);
	(void)context;
      }
      std::invoke(setter, obj, inst);
    };
    // Check value constraints before calling setter -
    addReader(key, anch::json::DeserializeFn<T>([=, this](T& obj, anch::json::ReaderContext& context) -> bool {
      static_cast<void>(this); // used by some instantiations only
      if constexpr (std::is_same<MT, P>::value) { // Field of its value type

	if constexpr (std::is_pointer<P>::value) { // Remove pointer on parameter type to check the main type
	  if constexpr (std::is_same<T, typename std::remove_pointer<P>>::value) { // if same, use direct call to avoid recursive instanciation
	    T* inst = new T();
	    if(this->deserialize(*inst, context)) {
	      set(obj, inst, context);
	      return true;
	    }
	    return false;
	  } else { // Use 'unpointered' type
	    P* inst = new P();
	    if(anch::json::Factory<typename std::remove_pointer<P>::type>::getInstance().deserialize(*inst, context)) {
	      set(obj, inst, context);
	      return true;
	    }
	    return false;
	  }

	} else { // Basic type
	  if constexpr (std::is_same<P, T>::value) { // if same, use direct call to avoid recursive instanciation
	    T inst;
	    if(this->deserialize(inst, context)) {
	      set(obj, inst, context);
	      return true;
	    }
	    return false;
	  } else {
	    P inst;
	    if(anch::json::Factory<P>::getInstance().deserialize(inst, context)) {
	      set(obj, inst, context);
	      return true;
	    }
	    return false;
	  }
	}

      } else { // std::optional, pointers, collections and maps: mapper of the value type
	if constexpr (std::is_same<MT, T>::value) {
	  T inst;
	  if(this->deserialize(inst, context)) {
	    set(obj, inst, context);
	    return true;
	  }
	  return false;
	} else {
	  MT inst;
	  if(anch::json::Factory<MT>::getInstance().deserialize(inst, context)) {
	    set(obj, inst, context);
	    return true;
	  }
	  return false;
	}
      }
    }), anch::val::isRequired<C...>(), anch::val::isNullable<P>());
    return *this;
  }

  template<typename T>
  template<typename P, typename... C>
  requires (anch::val::FieldConstraint<C,P> && ...)
  ObjectMapper<T>&
  ObjectMapper<T>::registerField(const std::string& key, std::function<P(const T&)> getter, std::function<void(T&, const P&)> setter, C&&... constraints) {
    registerField<P>(key, getter, std::as_const(constraints)...);
    return registerField<P>(key, setter, std::forward<C>(constraints)...);
  }
  template<typename T>
  template<typename R, typename P, typename... C>
  anch::json::DeserializeFn<T>
  ObjectMapper<T>::withConstraints(R& read, [[maybe_unused]] P T::* value, [[maybe_unused]] C&&... constraints) {
    if constexpr (anch::val::hasValueConstraints<C...>()) {
      return anch::json::DeserializeFn<T>([read, value, ...cts = std::forward<C>(constraints)](T& obj, anch::json::ReaderContext& context) -> bool {
	if(!read(obj, context)) {
	  return false;
	}
	anch::val::detail::checkConstraints(obj.*value, context, cts...);
	return true;
      });
    } else {
      return anch::json::DeserializeFn<T>(read);
    }
  }
  // Fields' registration -

  // Methods +
  template<typename T>
  void
  ObjectMapper<T>::serializeValue(const T& value, anch::json::WriterContext& context) const {
    context.beginObject();
    for(const FieldWriter& writer : _writers) {
      try {
	std::invoke(writer.write, value, context);
      } catch(anch::json::MappingError& error) {
	error.prependField(writer.key);
	throw;
      }
    }
    context.endObject();
  }

  template<typename T>
  bool
  ObjectMapper<T>::deserializeValue(T& value, anch::json::ReaderContext& context) const {
    if(!anch::json::objectHasValueLex(context)) {
      return false;
    }
    if(!_required.empty() && context.options.deserialize_check_constraints) {
      deserializeRequired(value, context);
      return true;
    }
    anch::json::lexObject([this, &value](const std::string& field, anch::json::ReaderContext& ctxt) -> void {
      readField(value, field, ctxt);
    }, context);
    return true;
  }

  template<typename T>
  inline
  const typename ObjectMapper<T>::FieldReader*
  ObjectMapper<T>::readField(T& value, const std::string& field, anch::json::ReaderContext& context) const {
    try {
      auto iter = _readers.find(field);
      if(iter == _readers.cend()) {
	context.parseUnknown();
	return NULL;
      }
      const FieldReader& reader = iter->second;
      if(!std::invoke(reader.read, value, context) && reader.rejectNull && context.options.deserialize_check_constraints) {
	anch::json::detail::rejectNull(context);
      }
      return &reader;
    } catch(anch::json::MappingError& error) {
      error.prependField(field);
      throw;
    }
  }

  template<typename T>
  void
  ObjectMapper<T>::deserializeRequired(T& value, anch::json::ReaderContext& context) const {
    // Found required fields: one bit per required field (on stack up to 64 required fields)
    uint64_t local = 0;
    std::vector<uint64_t> words;
    uint64_t* found = &local;
    if(_required.size() > 64) [[unlikely]] {
      words.assign((_required.size() + 63) / 64, 0);
      found = words.data();
    }
    anch::json::lexObject([this, &value, found](const std::string& field, anch::json::ReaderContext& ctxt) -> void {
      const FieldReader* reader = readField(value, field, ctxt);
      if(reader != NULL && reader->required != NOT_REQUIRED) {
	found[reader->required / 64] |= uint64_t{1} << (reader->required % 64);
      }
    }, context);
    anch::json::detail::checkRequired(_required, found, context);
  }
  // Methods -
  // Generic implementations -

}  // anch::json
