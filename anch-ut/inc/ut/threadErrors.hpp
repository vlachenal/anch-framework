/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include <exception>
#include <mutex>

namespace anch::ut {

  /*!
   * \brief Exceptions raised in threads started by a unit test
   *
   * An exception (as a failed assertion) raised in a thread other than the test one can not be caught by the runner:
   * the process is terminated (\c std::terminate ) without any report.\n
   * Wrap thread functions with \ref wrap : the first exception raised in a wrapped function is kept, and it is raised
   * again in the test thread by \ref rethrow once threads have been joined.
   *
   * \code{.cpp}
   * anch::ut::ThreadErrors errors;
   * std::thread thread(errors.wrap([]() {
   *   anch::ut::assertThat(check(), "check failed in thread");
   * }));
   * thread.join();
   * errors.rethrow(); // the failed assertion is reported by the runner
   * \endcode
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class ThreadErrors {

    // Attributes +
  private:
    /*! Mutex */
    std::mutex _mutex;

    /*! First exception raised */
    std::exception_ptr _error;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ThreadErrors default constructor
     */
    ThreadErrors() = default;

    /*!
     * Forbids \ref ThreadErrors copy constructor (wrapped functions keep a reference on it)
     */
    ThreadErrors(const ThreadErrors&) = delete;

    /*!
     * Forbids \ref ThreadErrors move constructor (wrapped functions keep a reference on it)
     */
    ThreadErrors(ThreadErrors&&) = delete;
    // Constructors -

    // Methods +
  public:
    /*!
     * Wrap function to keep the exception it raises. This object has to outlive the returned function.
     *
     * \tparam F the function type
     *
     * \param func the function to wrap
     *
     * \return the wrapped function (which takes the same arguments and returns nothing)
     */
    template<typename F>
    auto wrap(F&& func);

    /*!
     * Keep the exception which is currently handled (when there is no previous one).\n
     * It has to be called in a \c catch block.
     */
    void capture() noexcept;

    /*!
     * Check if an exception has been raised
     *
     * \return \c true when an exception has been kept, \c false otherwise
     */
    bool failed();

    /*!
     * Raise the first exception which has been kept (do nothing when there is none)
     *
     * \throw the kept exception
     */
    void rethrow();
    // Methods -

  };

}  // anch::ut

#include "ut/impl/threadErrors.hpp"
