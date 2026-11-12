/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

namespace anch {

  /*!
   * \brief Meyers' singleton implementation.
   *
   * The instance is created on first \ref getInstance call. Its creation is thread safe according to the standard.\n
   * If \c T constructor throws an exception, the next \ref getInstance call will try to create the instance again.\n
   * \n
   * Usage:
   * \code{.cpp}
   * class MySingleton: public anch::Singleton<MySingleton> {
   *   friend anch::Singleton<MySingleton>;
   * private:
   *   MySingleton();
   *   ~MySingleton();
   * };
   * \endcode
   * - \c T has to inherit from \c anch::Singleton<T> ;
   * - \c T constructor and destructor should be private and \c anch::Singleton<T> declared as friend to prevent other instances ;
   * - copy and move are prohibited by this class: there is no need to delete them in \c T ;
   * - \c T destructor does not need to be \c virtual: the instance is never destroyed through a base class pointer.
   *
   * \b Destruction \b order: singletons are destroyed at program exit, in the reverse order of the \b end of their
   * construction. When a singleton uses another one in its destructor (or holds objects which depend on it), it has to
   * call the other singleton's \ref getInstance in its constructor: the other singleton will be fully constructed first,
   * so it will be destroyed after.\n
   * A singleton must not be used from a static object destructor or a detached thread after its destruction.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class Singleton {

    // Constructors +
  protected:
    /*!
     * \ref Singleton constructor
     */
    Singleton() noexcept = default;

  public:
    /*!
     * Prohibits \ref Singleton copy constructor
     */
    Singleton(const Singleton&) = delete;

    /*!
     * Prohibits \ref Singleton move constructor
     */
    Singleton(Singleton&&) = delete;
    // Constructors -

    // Destructor +
  protected:
    /*!
     * \ref Singleton destructor
     */
    ~Singleton() = default;
    // Destructor -

    // Operators +
  public:
    /*!
     * Prohibits \ref Singleton copy assignment
     */
    Singleton& operator=(const Singleton&) = delete;

    /*!
     * Prohibits \ref Singleton move assignment
     */
    Singleton& operator=(Singleton&&) = delete;
    // Operators -

    // Methods +
  public:
    /*!
     * Get unique instance of a class.
     *
     * \return the unique instance
     */
    static T& getInstance();
    // Methods -

  };

}

#include "impl/singleton.hpp"
