/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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

namespace anch {

  template<typename... T>
  Flux<T...>::Flux(): _consumer(), _finalizer([](){}), _errorHandler([](){throw;}) {
    // Nothing to do
  }

  template<typename... T>
  Flux<T...>::~Flux() {
    // Nothing to do
  }

  template<typename... T>
  void
  Flux<T...>::push(const T&... object) {
    try {
      std::invoke(_consumer, object...);
    } catch(...) {
      std::invoke(_errorHandler);
    }
  }

  template<typename... T>
  void
  Flux<T...>::finalize() {
    std::invoke(_finalizer);
  }

  template<typename... T>
  inline
  void
  Flux<T...>::setConsumer(std::function<void(const T&...)> consumer) {
    _consumer = consumer;
  }

  template<typename... T>
  inline
  void
  Flux<T...>::setFinalizer(std::function<void()> finalizer) {
    _finalizer = finalizer;
  }

  template<typename... T>
  inline
  void
  Flux<T...>::setErrorHandler(std::function<void()> errorHandler) {
    _errorHandler = errorHandler;
  }

}
