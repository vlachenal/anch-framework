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
#include <thread>
#include <condition_variable>
#include <future>


namespace anch {

  /*!
   * \brief Manage collection as stream
   *
   * Each item fomr the collection will be added with \c push method.\n
   * The synchronization is managed with \c std::promise / \c std::future C++ features.\n
   *
   * \c Flux runs with:
   *   - item's consumer (mandatory)
   *   - error's handler (optional)
   *   - finalizer (optional)
   *
   * When not set, the \c ready method will set error handler to rethrow and finalizer to do nothing.\n
   * \n
   * When not sure, it is advise to call \c ready every time you have been to set consummer.
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
    /*! Consumer has been set flag */
    bool _con;

    /*! Promise consumer value */
    std::promise<std::function<void(const T&...)>> _pcon;

    /*! Consumer object function */
    std::shared_future<std::function<void(const T&...)>> _consumer;

    /*! Finalizer has been set flag */
    bool _fin;

    /*! Finalizer consumer value */
    std::promise<std::function<void()>> _pfin;

    /*! Finalizer object function */
    std::shared_future<std::function<void()>> _finalizer;

    /*! Error handler has been set flag */
    bool _err;

    /*! Error handler consumer value */
    std::promise<std::function<void()>> _perr;

    /*! Error handler object function */
    std::shared_future<std::function<void()>> _errorHandler;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Flux default constructor
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
     * Set ready to consume\n
     *
     * When consumer is not set, \c std::runtime_error will be raised.\n
     * When finalizer is not set, \c ready set it to nothing to do.\n
     * When error handler is not set, \c ready set it to rethrow.
     */
    void ready();

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
     * Object error handler setter
     *
     * \param errorHandler the error handler to use
     */
    void setErrorHandler(std::function<void()> errorHandler);
    // Accessors -

  };

}

#include "impl/flux.hpp"
