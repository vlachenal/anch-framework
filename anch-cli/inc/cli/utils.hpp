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
#include <memory>
#include <filesystem>
#include <fstream>

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

  /*!
   * Bind input file stream
   *
   * \param dest the \c std::filesystem::path to fill
   *
   * \return the binding function
   */
  BindArg bindPath(std::filesystem::path& dest);

  /*!
   * Bind input file stream
   *
   * \param dest the \c std::istream to fill
   *
   * \return the binding function
   */
  BindArg bindIFS(std::shared_ptr<std::istream>& dest);

  /*!
   * Bind output file stream
   *
   * \param dest the \c std::ostream to fill
   *
   * \return the binding function
   */
  BindArg bindOFS(std::shared_ptr<std::ostream>& dest);

  /*!
   * Bind output file stream
   *
   * \param dest the \c std::ofstream to fill
   *
   * \return the binding function
   */
  BindArg bindOFS(std::shared_ptr<std::ofstream>& dest, std::ios_base::openmode mode = std::ios_base::out);

  /*!
   * Bind \c std::cin to input stream
   *
   * \param dest the destination input stream
   */
  std::function<void(std::istream&)> bindPipe(std::shared_ptr<std::istream>& dest);

  /*!
   * Disable console formatting when environment variable \c ANCH_CLI_NO_FMT is set to \c 1
   */
  void manageEnvNoFormat();

}  // anch::cli
