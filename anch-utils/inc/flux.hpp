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

#include <functional>


namespace anch {

  /*!
   * \brief Manage collection as stream
   *
   * \tparam T the collection's type
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename... T>
  class Flux {

    // Attributes +
  private:
    /*! Consumer object function */
    std::function<void(const T&...)> _consumer;

    /*! Finalizer object function (default to do nothing) */
    std::function<void()> _finalizer;

    /*! Error handler object function (default to rethrow) */
    std::function<void()> _errorHandler;
    // Attributes -

    // Constructors +
  public:
    /*!
     * Forbids \ref Flux default constructor
     */
    Flux();
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Flux destructor
     */
    virtual ~Flux();
    // Destructor -

    // Methods +
  public:
    /*!
     * Push object in stream
     *
     * \param object the object to push in stream
     */
    void push(const T&... object);

    /*!
     * Finalize treatment
     */
    void finalize();
    // Methods -

    // Accessors +
  public:
    /*!
     * Object consumer setter
     *
     * \param consumer the consumer to use
     */
    void setConsumer(std::function<void(const T&...)> consumer);

    /*!
     * Object finalizer setter
     *
     * \param finalizer the finalizer to use
     */
    void setFinalizer(std::function<void()> finalizer);

    /*!
     * Object finalizer setter
     *
     * \param finalizer the finalizer to use
     */
    void setErrorHandler(std::function<void()> errorHandler);
    // Accessors -

  };

}

#include "impl/flux.hpp"
