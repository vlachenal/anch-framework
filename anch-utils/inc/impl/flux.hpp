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

#include <stdexcept>


namespace anch {

  template<typename... T>
  Flux<T...>::Flux():
    _con(false),
    _pcon(),
    _consumer(_pcon.get_future()),
    _fin(false),
    _pfin(),
    _finalizer(_pfin.get_future()),
    _err(false),
    _perr(),
    _errorHandler(_perr.get_future()) {
    // Nothing to do
  }

  template<typename... T>
  Flux<T...>::~Flux() {
    // Nothing to do
  }

  template<typename... T>
  void
  Flux<T...>::ready() {
    if(!_con) {
      throw std::runtime_error("Consumer function is not set");
    }
    if(!_fin) {
      _pfin.set_value([](){});
    }
    if(!_err) {
      _perr.set_value([](){throw;});
    }
  }

  template<typename... T>
  void
  Flux<T...>::push(const T&... object) {
    _consumer.wait();
    _errorHandler.wait();
    try {
      std::invoke(_consumer.get(), object...);
    } catch(...) {
      std::invoke(_errorHandler.get());
    }
  }

  template<typename... T>
  void
  Flux<T...>::finalize() {
    _finalizer.wait();
    std::invoke(_finalizer.get());
  }

  template<typename... T>
  inline
  void
  Flux<T...>::setConsumer(std::function<void(const T&...)> consumer) {
    if(_con) {
      throw std::runtime_error("Consumer function is already set");
    }
    _pcon.set_value(consumer);
    _con = true;
  }

  template<typename... T>
  inline
  void
  Flux<T...>::setFinalizer(std::function<void()> finalizer) {
    if(_fin) {
      throw std::runtime_error("Finalizer function is already set");
    }
    _pfin.set_value(finalizer);
    _fin = true;
  }

  template<typename... T>
  inline
  void
  Flux<T...>::setErrorHandler(std::function<void()> errorHandler) {
    if(_err) {
      throw std::runtime_error("Finalizer function is already set");
    }
    _perr.set_value(errorHandler);
    _err = true;
  }

}
