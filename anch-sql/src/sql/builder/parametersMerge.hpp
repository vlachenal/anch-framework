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

#include <string>
#include <string_view>

#include "sql/parameters.hpp"

namespace anch::sql::detail {

  /*!
   * Unique parameter name of a column: characters which are not letters, digits or \c '_' are replaced by \c '_'
   * ( \c t.id gives \c t_id ), \c _2 , \c _3 , ... is appended when the name is already used
   *
   * \param parameters the parameters
   * \param column the column
   * \param suffix the name suffix ( \c _min , \c _max , ...)
   *
   * \return the name
   */
  std::string parameterName(const anch::sql::Parameters& parameters, const std::string& column, std::string_view suffix = "");

  /*!
   * Merge parameters of a query part: its parameters whose names are already used are renamed
   *
   * \param target the parameters of the query
   * \param part the query part
   * \param parameters the parameters of the query part
   *
   * \return the query part (with renamed parameters)
   */
  std::string mergeParameters(anch::sql::Parameters& target, const std::string& part, const anch::sql::Parameters& parameters);

}
