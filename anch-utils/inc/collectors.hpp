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

#include <vector>
#include <list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <deque>
#include <stack>
#include <queue>


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
     * Insert value in \c std::vector
     *
     * \param container the vector
     * \param val the value
     */
    static void toVector(std::vector<T>& container, const T& val);

    /*!
     * Insert value in \c std::list
     *
     * \param container the list
     * \param val the value
     */
    static void toList(std::list<T>& container, const T& val);

    /*!
     * Insert value in \c std::forward_list
     *
     * \param container the list
     * \param val the value
     */
    static void toForwardList(std::forward_list<T>& container, const T& val);

    /*!
     * Insert value in \c std::set
     *
     * \param container the set
     * \param val the value
     */
    static void toSet(std::set<T>& container, const T& val);

    /*!
     * Insert value in \c std::set
     *
     * \param container the set
     * \param val the value
     */
    static void toMultiset(std::multiset<T>& container, const T& val);

    /*!
     * Insert value in \c std::set
     *
     * \param container the set
     * \param val the value
     */
    static void toUnorderedSet(std::unordered_set<T>& container, const T& val);

    /*!
     * Insert value in \c std::deque
     *
     * \param container the deque
     * \param val the value
     */
    static void toDeque(std::deque<T>& container, const T& val);

    /*!
     * Insert value in \c std::stack
     *
     * \param container the stack
     * \param val the value
     */
    static void toStack(std::stack<T>& container, const T& val);

    /*!
     * Insert value in \c std::queue
     *
     * \param container the queue
     * \param val the value
     */
    static void toQueue(std::queue<T>& container, const T& val);

    /*!
     * Insert value in \c std::priority_queue
     *
     * \param container the queue
     * \param val the value
     */
    static void toPriorityQueue(std::priority_queue<T>& container, const T& val);

  };

  // Implementations +
  template<typename T>
  void
  Collectors<T>::toVector(std::vector<T>& container, const T& val) {
    container.push_back(val);
  }

  template<typename T>
  void
  Collectors<T>::toList(std::list<T>& container, const T& val) {
    container.push_back(val);
  }

  template<typename T>
  void
  Collectors<T>::toForwardList(std::forward_list<T>& container, const T& val) {
    container.push_front(val);
  }

  template<typename T>
  void
  Collectors<T>::toSet(std::set<T>& container, const T& val) {
    container.insert(val);
  }

  template<typename T>
  void
  Collectors<T>::toMultiset(std::multiset<T>& container, const T& val) {
    container.insert(val);
  }

  template<typename T>
  void
  Collectors<T>::toUnorderedSet(std::unordered_set<T>& container, const T& val) {
    container.insert(val);
  }

  template<typename T>
  void
  Collectors<T>::toDeque(std::deque<T>& container, const T& val) {
    container.push_back(val);
  }

  template<typename T>
  void
  Collectors<T>::toStack(std::stack<T>& container, const T& val) {
    container.push(val);
  }

  template<typename T>
  void
  Collectors<T>::toQueue(std::queue<T>& container, const T& val) {
    container.push(val);
  }

  template<typename T>
  void
  Collectors<T>::toPriorityQueue(std::priority_queue<T>& container, const T& val) {
    container.push(val);
  }
  // Implementations -

}

#endif // _ANCH_COLLECTORS_H_
