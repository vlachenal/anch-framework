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
#include <vector>
#include <map>
#include <ostream>
#include <memory>

#include "cli/app.hpp"
#include "cli/arg.hpp"

namespace anch::cli {

  /*! \c RegisteredArg internal class declaration */
  class RegisteredArg;

  /*!
   * \brief CLI arguments handler
   *
   * This class aims to register, parse and check arguments.\n
   * If not defined, \ref ArgHandler will add help (\c -h and/or \c --help ) and version options (\c -v and/or \c --version ).
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class ArgHandler {

    // Attributes +
  private:
    /*! Application inforations */
    anch::cli::App _app;

    /*! Options */
    std::vector<std::shared_ptr<anch::cli::RegisteredArg>> _options;

    /*! Options indexed by short option */
    std::map<char, std::shared_ptr<anch::cli::RegisteredArg>> _sopts;

    /*! Options indexed by long option */
    std::map<std::string, std::shared_ptr<anch::cli::RegisteredArg>> _lopts;

    /*! Positional options */
    std::vector<std::shared_ptr<anch::cli::RegisteredArg>> _positionals;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref ArgHandler constructor
     *
     * \param app the application informations
     * \param options the arguments' definition (default to empty \c std::vector )
     *
     * \throw std::invalid_argument invalid argument registering
     */
    ArgHandler(const anch::cli::App& app, const std::vector<anch::cli::Arg>& options = {});

    /*!
     * \ref ArgHandler constructor
     *
     * \param options the arguments' definition
     *
     * \throw std::invalid_argument invalid argument registering
     */
    ArgHandler(const std::vector<anch::cli::Arg>& options);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref ArgHandler destructor
     */
    virtual ~ArgHandler();
    // Destructor -

    // Methods +
  public:
    /*!
     * Register argument
     *
     * \param arg the argument to register
     *
     * \return reference to \c this
     *
     * \throw std::invalid_argument invalid argument registering
     */
    anch::cli::ArgHandler& arg(anch::cli::Arg& arg);

    /*!
     * Parse command line arguments
     *
     * \param argc the number of arguments
     * \param argv the arguments' list
     *
     * \throw std::invalid_argument invalid argument usage
     */
    void parse(int argc, char** argv);

    /*!
     * Parse command line arguments
     *
     * \throw std::invalid_argument invalid or missing argument
     */
    void check();

    /*!
     * Parse and check command line arguments
     *
     * \param argc the number of arguments
     * \param argv the arguments' list
     *
     * \throw std::invalid_argument invalid or missing argument
     */
    void handle(int argc, char** argv);

    /*!
     * Print help and immediatly exit with return code \c 0
     *
     * \param out the output stream to print in
     */
    void printHelp(std::ostream& out);

    /*!
     * Print help and immediatly exit with return code \c 0
     *
     * \param out the output stream to print in
     */
    void printVersion(std::ostream& out);

    /*!
     * Print banner
     *
     * \param out the output stream to print in
     */
    void printBanner(std::ostream& out);

  private:
    /*!
     * Register argument help (\c -h and/or \c --help ) and version (\c -v and/or \c --version ) arguments if not found.\n
     * This method will be called on \ref handleArgs
     *
     * \param arg0 the first argument (to set application name when not set)
     *
     * \return reference to \c this
     */
    void build(const std::string& arg0);

    /*!
     * Print application name and its version
     *
     * \param out the output stream to print in
     */
    void printAppVersion(std::ostream& out);

    /*!
     * Print application name and its version
     *
     * \param out the output stream to print in
     *
     * \return \c true if not postional arguments has been found, \c false otherwise
     */
    bool printUsage(std::ostream& out);

    /*!
     * Print options
     *
     * \param out the output stream to print in
     */
    void printOptions(std::ostream& out);
    // Methods -

  };

}
