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

#include <string>


namespace anch {
  namespace sql {

    /*!
     * Provides 'not equals' clause with \c <> operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string notEquals(const std::string& column);

    /*!
     * Provides 'equals' clause with \c = operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string equals(const std::string& column);

    /*!
     * Provides 'greater' clause with \c > operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string greater(const std::string& column);

    /*!
     * Provides 'greater or equals' clause with \c >= operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string greaterEquals(const std::string& column);

    /*!
     * Provides 'lesser' clause with \c < operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string lesser(const std::string& column);

    /*!
     * Provides 'lesser or equals' clause with \c <= operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string lesserEquals(const std::string& column);

    /*!
     * Provides 'like' clause with \c LIKE operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string like(const std::string& column);

    /*!
     * Provides 'not like' clause with <tt>NOT LIKE</tt> operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string notLike(const std::string& column);

    /*!
     * Provides 'between' clause with \c BETWEEN operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string between(const std::string& column);

    /*!
     * Provides 'not between' clause with <tt>NOT BETWEEN</tt> operator
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string notBetween(const std::string& column);

    /*!
     * Provides 'in' clause with \c IN operator.
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string in(const std::string& column);

    /*!
     * Provides 'not in' clause with <tt>NOT IN</tt> operator.
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string notIn(const std::string& column);

    /*!
     * Provides 'exists' clause with \c EXISTS operator.
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string exists(const std::string& column);

    /*!
     * Provides 'not exists' clause with <tt>NOT EXISTS</tt> operator.
     *
     * \param column the column
     *
     * \return clause request part
     */
    std::string notExists(const std::string& column);

  }  // sql
}  // anch
