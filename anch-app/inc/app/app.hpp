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

#include "anch-app-export.hpp"

#include "app/args.hpp"

#include <string>
#include <vector>


namespace anch::app {

  /*!
   * \brief AnCH application with commons includes
   *
   * Provides bootstrap for applications which use AnCH framwork
   *
   * \version 0.1
   *
   * \author Vincent Lachenal
   */
  class ANCH_APP_EXPORT Application {

    // Attributes +
  protected:
    /*! Application's arguments handler */
    anch::app::Arguments* _handler;
    // Attributes -

    // Constructors +
  protected:
    /*!
     * \ref Application default constructor
     */
    Application();

  public:
    /*!
     * Forbids \ref Application copy constructor
     *
     * \param app the \ref Application not to copy
     */
    Application(const Application& app) = delete;

    /*!
     * Forbids \ref Application move constructor
     *
     * \param app the \ref Application not to move
     */
    Application(Application&& app) = delete;
    // Constructors -

    // Destructor +
  protected:
    /*!
     * \ref Application destructor.\n
     * Execture after().
     */
    virtual ~Application() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Run application.\n
     * \n
     * Run:
     *  - parse arguments
     *  - before()
     *  - execute()
     *
     * after() will be execute in destructor.\n
     * \n
     * This method can not be overriden.
     *
     * \param argc number of arguments (first one is the executable)
     * \param argv the arguments
     */
    virtual void run(int argc, char* argv[]) final;

  protected:
    /*!
     * Execute application.\n
     * This method has to be override.
     */
    virtual void execute() = 0;

    /*!
     * Register program argument
     *
     * \param arg the program's argument to register
     *
     * \return \c *this
     */
    Application& arg(const anch::cli::Arg& arg);

    /*!
     * Register program arguments
     *
     * \param args the program's arguments to register
     *
     * \return \c *this
     */
    Application& args(const std::vector<anch::cli::Arg>& args);

    /*!
     * Do something before execution.\n
     * Default implementation does nothing.
     */
    virtual void before();

    /*!
     * Do something before execution.\n
     * Default implementation does nothing.
     */
    virtual void after() noexcept;
    // Methods -

  };

}
