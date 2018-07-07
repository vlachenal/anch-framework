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

#include <set>
#include <unordered_set>
#include <map>


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
     * Insert value in \c std::set
     *
     * \param container the set
     * \param val the value
     */
    static void toSet(std::set<T>& container, const T& val);

    /*!
     * Insert value in \c std::unordered_set
     *
     * \param container the set
     * \param val the value
     */
    static void toUnorderedSet(std::unordered_set<T>& container, const T& val);

    /*!
     * Insert value in \c std::multiset
     *
     * \param container the set
     * \param val the value
     */
    static void toMultiset(std::multiset<T>& container, const T& val);

    /*!
     * Insert value in \c std::unordered_multiset
     *
     * \param container the set
     * \param val the value
     */
    static void toUnorderedMultiset(std::unordered_multiset<T>& container, const T& val);

    // /*!
    //  * Insert value in \c std::map
    //  *
    //  * \param container the map
    //  * \param val the value
    //  */
    // template<typename K, typename V>
    // static void toMap(std::map<K,V>& container, const T& val);

  };

  // Implementations +
  template<typename T>
  void
  Collectors<T>::toSet(std::set<T>& container, const T& val) {
    container.insert(val);
  }

  template<typename T>
  void
  Collectors<T>::toUnorderedSet(std::unordered_set<T>& container, const T& val) {
    container.insert(val);
  }

  template<typename T>
  void
  Collectors<T>::toMultiset(std::multiset<T>& container, const T& val) {
    container.insert(val);
  }

  template<typename T>
  void
  Collectors<T>::toUnorderedMultiset(std::unordered_multiset<T>& container, const T& val) {
    container.insert(val);
  }

  // template<typename K, typename V>
  // template<>
  // template<typename T>
  // void
  // Collectors<T>::toMap(std::map<K,V>& container, const T& val) {
  //   container.insert(val);
  // }

}

#endif // _ANCH_COLLECTORS_H_
