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

#include <sstream>
#include <condition_variable>
#include <mutex>
#include <utility>

#include "json/writerContext.hpp"


namespace anch::json {

  // Functions which write result in output stream +
  template<typename T>
  inline
  void
  serialize(const T& value, std::ostream& out, const anch::json::MappingOptions& options) {
    anch::json::WriterContext context(out, options);
    anch::json::Factory<T>::getInstance().serialize(value, context);
  }

  template<template<typename> typename C, typename T>
  inline
  void
  serialize(const C<T>& value, std::ostream& out, const anch::json::MappingOptions& options) {
    anch::json::WriterContext context(out, options);
    anch::json::Factory<T>::getInstance().serialize(value, context);
  }

  template<typename T>
  void
  serialize(anch::Flux<T>& value, std::ostream& out, const anch::json::MappingOptions& options) {
    anch::json::WriterContext context(out, options);
    auto& mapper = anch::json::Factory<T>::getInstance();
    std::mutex m;
    std::condition_variable cv;
    bool finished = false;
    bool first = true;
    value.setConsumer([&context, &mapper, &first](const T& val) {
      if(first) {
	context.beginArray();
	first = false;
      }
      context.next();
      mapper.serialize(val, context);
    });
    value.setFinalizer([&context, &first, &finished, &cv, &m]() {
      if(!first) {
	context.endArray();
      }
      // Under lock: the wake up can not be lost, and serialize can not return (and destroy cv) before notification
      std::lock_guard<std::mutex> lock(m);
      finished = true;
      cv.notify_one();
    });
    value.ready();
    std::unique_lock lk(m);
    cv.wait(lk, [&finished]() {
      return finished;
    }); // wait for end
  }

  template<typename T>
  inline
  void
  serialize(const std::map<std::string,T>& value, std::ostream& out, const anch::json::MappingOptions& options) {
    anch::json::WriterContext context(out, options);
    anch::json::Factory<T>::getInstance().serialize(value, context);
  }
  // Functions which write result in output stream -

  // Functions which returns std::string +
  template<typename T>
  inline
  std::string
  serialize(const T& value, const anch::json::MappingOptions& options) {
    std::ostringstream out;
    {
      anch::json::WriterContext context(out, options);
      anch::json::Factory<T>::getInstance().serialize(value, context);
    } // context is destroyed (and output flushed) before moving result
    return std::move(out).str(); // move internal string instead of copying it
  }

  template<template<typename> typename C, typename T>
  inline
  std::string
  serialize(const C<T>& value, const anch::json::MappingOptions& options) {
    std::ostringstream out;
    {
      anch::json::WriterContext context(out, options);
      anch::json::Factory<T>::getInstance().serialize(value, context);
    } // context is destroyed (and output flushed) before moving result
    return std::move(out).str(); // move internal string instead of copying it
  }

  template<typename T>
  inline
  std::string
  serialize(const std::map<std::string,T>& value, const anch::json::MappingOptions& options) {
    std::ostringstream out;
    {
      anch::json::WriterContext context(out, options);
      anch::json::Factory<T>::getInstance().serialize(value, context);
    } // context is destroyed (and output flushed) before moving result
    return std::move(out).str(); // move internal string instead of copying it
  }
  // Functions which returns std::string -

}
