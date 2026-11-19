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

#include "anch-cli-export.hpp"

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
  ANCH_CLI_EXPORT BindArg bindTrue(bool& dest);

  /*!
   * Bind \c false utility function
   *
   * \param dest the \c bool to set
   *
   * \return the binding function
   */
  ANCH_CLI_EXPORT BindArg bindFalse(bool& dest);

  /*!
   * Bind \c uint16_t value utility function
   *
   * \param dest the destination
   *
   * \return the binding function (which raises \c std::invalid_argument when value is empty, is not a number or is
   * out of range ; \c dest is not modified in this case)
   */
  ANCH_CLI_EXPORT BindArg bindNum(uint16_t& dest);

  /*!
   * Bind \c uint16_t value utility function
   *
   * \param dest the destination
   *
   * \return the binding function (which raises \c std::invalid_argument when value is empty, is not a number or is
   * out of range ; \c dest is not modified in this case)
   */
  ANCH_CLI_EXPORT BindArg bindNum(uint32_t& dest);

  /*!
   * Bind \c uint32_t value utility function
   *
   * \param dest the destination
   *
   * \return the binding function (which raises \c std::invalid_argument when value is empty, is not a number or is
   * out of range ; \c dest is not modified in this case)
   */
  ANCH_CLI_EXPORT BindArg bindNum(uint64_t& dest);

  /*!
   * Bind \c int64_t value utility function
   *
   * \param dest the destination
   *
   * \return the binding function (which raises \c std::invalid_argument when value is empty, is not a number or is
   * out of range ; \c dest is not modified in this case)
   */
  ANCH_CLI_EXPORT BindArg bindNum(int16_t& dest);

  /*!
   * Bind \c int32_t value utility function
   *
   * \param dest the destination
   *
   * \return the binding function (which raises \c std::invalid_argument when value is empty, is not a number or is
   * out of range ; \c dest is not modified in this case)
   */
  ANCH_CLI_EXPORT BindArg bindNum(int32_t& dest);

  /*!
   * Bind \c int64_t value utility function
   *
   * \param dest the destination
   *
   * \return the binding function (which raises \c std::invalid_argument when value is empty, is not a number or is
   * out of range ; \c dest is not modified in this case)
   */
  ANCH_CLI_EXPORT BindArg bindNum(int64_t& dest);

  /*!
   * Bind \c float value utility function
   *
   * \param dest the destination
   *
   * \return the binding function (which raises \c std::invalid_argument when value is empty, is not a number or is
   * out of range ; \c dest is not modified in this case)
   */
  ANCH_CLI_EXPORT BindArg bindNum(float& dest);

  /*!
   * Bind \c double value utility function
   *
   * \param dest the destination
   *
   * \return the binding function (which raises \c std::invalid_argument when value is empty, is not a number or is
   * out of range ; \c dest is not modified in this case)
   */
  ANCH_CLI_EXPORT BindArg bindNum(double& dest);

  /*!
   * Bind caracters' string value utility function
   *
   * \param dest the destination
   *
   * \return the binding function
   */
  ANCH_CLI_EXPORT BindArg bindStr(std::string& dest);

  /*!
   * Bind caracters' string value utility function and macro
   *
   * \param dest the destination
   *
   * \return the binding function
   */
  ANCH_CLI_EXPORT BindArg bindStr(std::optional<std::string>& dest);

  /*!
   * Add value to vector
   *
   * \param dest the \c std::vector to fill
   *
   * \return the binding function
   */
  ANCH_CLI_EXPORT BindArg bindCol(std::vector<std::string>& dest);

  /*!
   * Add value to list
   *
   * \param dest the \c sdt::list to fill
   *
   * \return the binding function
   */
  ANCH_CLI_EXPORT BindArg bindCol(std::list<std::string>& dest);

  /*!
   * Add value to set
   *
   * \param dest the \c sdt::set to fill
   *
   * \return the binding function
   */
  ANCH_CLI_EXPORT BindArg bindCol(std::set<std::string>& dest);

  /*!
   * Bind input file stream
   *
   * \param dest the \c std::filesystem::path to fill
   *
   * \return the binding function
   */
  ANCH_CLI_EXPORT BindArg bindPath(std::filesystem::path& dest);

  /*!
   * Bind input file stream
   *
   * \param dest the \c std::istream to fill
   *
   * \return the binding function (which raises \c std::invalid_argument when file does not exist, is not a regular file
   * or can not be read)
   */
  ANCH_CLI_EXPORT BindArg bindIFS(std::shared_ptr<std::istream>& dest);

  /*!
   * Bind output file stream
   *
   * \param dest the \c std::ostream to fill
   *
   * \return the binding function (which raises \c std::invalid_argument when file can not be opened for writing)
   */
  ANCH_CLI_EXPORT BindArg bindOFS(std::shared_ptr<std::ostream>& dest);

  /*!
   * Bind output file stream
   *
   * \param dest the \c std::ofstream to fill
   * \param mode the file open mode (default to \c std::ios_base::out )
   *
   * \return the binding function (which raises \c std::invalid_argument when file can not be opened for writing)
   */
  ANCH_CLI_EXPORT BindArg bindOFS(std::shared_ptr<std::ofstream>& dest, std::ios_base::openmode mode = std::ios_base::out);

  /*!
   * Bind \c std::cin to input stream
   *
   * \param dest the destination input stream
   */
  ANCH_CLI_EXPORT std::function<void(std::istream&)> bindPipe(std::shared_ptr<std::istream>& dest);

  /*!
   * Bind \c std::cout to output stream
   *
   * \param dest the destination output stream
   */
  ANCH_CLI_EXPORT std::function<void(std::ostream&)> bindCout(std::shared_ptr<std::ostream>& dest);

  /*!
   * Disable console formatting when environment variable \c ANCH_CLI_NO_FMT is set to \c 1
   */
  ANCH_CLI_EXPORT void manageEnvNoFormat();

}  // anch::cli
