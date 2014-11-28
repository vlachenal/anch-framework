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
#ifndef _ANCH_UTILS_SINGLETON_H_
#define _ANCH_UTILS_SINGLETON_H_

namespace anch {

  /*!
   * \brief Meyers' singleton implemtation.
   *
   * In C++11, this singleton is thread safe according to the standard.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class Singleton {
  public:
    /*!
     * Get unique instance of a class.
     *
     * \return the unique instance
     */
    template<typename... Args>
    static T& getInstance(Args... args) {
      static T instance(args);
      return instance;
    }
  };

}

#endif // _ANCH_UTILS_SINGLETON_H_
