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
#ifndef _ANCH_COLLECTORS_H_
#define _ANCH_COLLECTORS_H_


namespace anch {

  /*!
   * \brief Stream collector functions
   *
   * It contains collectors for STL container library
   *
   * \author Vincent Lachenal
   */
  template<typename T>
  class Collectors {
  public:
    /*!
     * Insert value in container
     *
     * \param container the set
     * \param val the value
     */
    template<typename C>
    static void insert(C& container, const T& val);
  };

  // Implementations +
  template<typename T>
  template<typename C>
  void
  Collectors<T>::insert(C& container, const T& val) {
    container.insert(val);
  }
  // Implementations -

}

#endif // _ANCH_COLLECTORS_H_
