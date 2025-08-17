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

namespace anch::json {

  // Raw value +
  template<typename M, typename T>
  inline
  void
  GenericMapper<M,T>::serialize(const T& value, anch::json::WriterContext& context) const {
    static_cast<const M*>(this)->serializeValue(value, context);
  }

  template<typename M, typename T>
  inline
  void
  GenericMapper<M,T>::serialize(const std::optional<T>& value, anch::json::WriterContext& context) const {
    serialize(value.value(), context);
  }

  template<typename M, typename T>
  inline
  void
  GenericMapper<M,T>::serialize(const T* const value, anch::json::WriterContext& context) const {
    serialize(*value, context);
  }

  template<typename M, typename T>
  void
  GenericMapper<M,T>::serialize(const std::vector<T>& value, anch::json::WriterContext& context) const {
    context.beginArray();
    for(auto iter = value.cbegin() ; iter != value.cend() ; ++iter) {
      context.next();
      serialize(*iter, context);
    }
    context.endArray();
  }

  template<typename M, typename T>
  void
  GenericMapper<M,T>::serialize(const std::list<T>& value, anch::json::WriterContext& context) const {
    context.beginArray();
    for(auto iter = value.cbegin() ; iter != value.cend() ; ++iter) {
      context.next();
      serialize(*iter, context);
    }
    context.endArray();
  }

  template<typename M, typename T>
  void
  GenericMapper<M,T>::serialize(const std::set<T>& value, anch::json::WriterContext& context) const {
    context.beginArray();
    for(auto iter = value.cbegin() ; iter != value.cend() ; ++iter) {
      context.next();
      serialize(*iter, context);
    }
    context.endArray();
  }

  template<typename M, typename T>
  void
  GenericMapper<M,T>::serialize(const std::map<std::string,T>& value, anch::json::WriterContext& context) const {
    context.beginObject();
    for(auto iter = value.cbegin() ; iter != value.cend() ; ++iter) {
      context.writeField(iter->first);
      serialize(iter->second, context);
    }
    context.endObject();
  }
  // Raw value -

  // With field +
  template<typename M, typename T>
  inline
  bool
  GenericMapper<M,T>::serialize(const std::string& field,
				const T& value,
				anch::json::WriterContext& context) const {
    context.writeField(field);
    serialize(value, context);
    return true;
  }

  template<typename M, typename T>
  bool
  GenericMapper<M,T>::serialize(const std::string& field,
				const std::optional<T>& value,
				anch::json::WriterContext& context) const {
    if(!value.has_value()) {
      return context.writeNull(field);
    }
    context.writeField(field);
    serialize(value, context);
    return true;
  }

  template<typename M, typename T>
  bool
  GenericMapper<M,T>::serialize(const std::string& field,
				const T* const value,
				anch::json::WriterContext& context) const {
    if(value == NULL) {
      return context.writeNull(field);
    }
    context.writeField(field);
    serialize(value, context);
    return true;
  }

  template<typename M, typename T>
  bool
  GenericMapper<M,T>::serialize(const std::string& field,
				const std::vector<T>& value,
				anch::json::WriterContext& context) const {
    if(value.empty()) {
      return context.writeEmptyArray(field);
    }
    context.writeField(field);
    serialize(value, context);
    return true;
  }

  template<typename M, typename T>
  bool
  GenericMapper<M,T>::serialize(const std::string& field,
				const std::list<T>& value,
				anch::json::WriterContext& context) const {
    if(value.empty()) {
      return context.writeEmptyArray(field);
    }
    context.writeField(field);
    serialize(value, context);
    return true;
  }

  template<typename M, typename T>
  bool
  GenericMapper<M,T>::serialize(const std::string& field,
				const std::set<T>& value,
				anch::json::WriterContext& context) const {
    if(value.empty()) {
      return context.writeEmptyArray(field);
    }
    context.writeField(field);
    serialize(value, context);
    return true;
  }

  template<typename M, typename T>
  bool
  GenericMapper<M,T>::serialize(const std::string& field,
				const std::map<std::string,T>& value,
				anch::json::WriterContext& context) const {
    if(value.empty()) {
      return context.writeEmptyObject(field);
    }
    context.writeField(field);
    serialize(value, context);
    return true;
  }
  // With field -

}
