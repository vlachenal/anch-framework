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

#include <list>
#include <vector>
#include <set>
#include <string>
#include <optional>
#include <functional>

namespace anch::cli {

  /*! Binding function alias */
  using BindArg = std::function<void(const std::string&)>;

  /*!
   * Bind \c true utility function
   *
   * \param dest the \c bool to set
   *
   * \return the binding function
   */
  BindArg bindTrue(bool& dest);

  /*!
   * Bind \c false utility function
   *
   * \param dest the \c bool to set
   *
   * \return the binding function
   */
  BindArg bindFalse(bool& dest);

  /*!
   * Bind caracters' string value utility function
   *
   * \param dest the destination
   *
   * \return the binding function
   */
  BindArg bindStr(std::string& dest);

  /*!
   * Bind caracters' string value utility function and macro
   *
   * \param dest the destination
   *
   * \return the binding function
   */
  BindArg bindStr(std::optional<std::string>& dest);

  /*!
   * Add value to vector
   *
   * \param dest the \c std::vector to fill
   *
   * \return the binding function
   */
  BindArg bindCol(std::vector<std::string>& dest);

  /*!
   * Add value to list
   *
   * \param dest the \c sdt::list to fill
   *
   * \return the binding function
   */
  BindArg bindCol(std::list<std::string>& dest);

  /*!
   * Add value to set
   *
   * \param dest the \c sdt::set to fill
   *
   * \return the binding function
   */
  BindArg bindCol(std::set<std::string>& dest);

}  // anch::cli
