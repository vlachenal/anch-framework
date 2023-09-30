/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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

#include <optional>
#include <vector>
#include <string>

#include "events/observer.hpp"
#include "network/socketEvent.hpp"

#include "ut/defaultRunner.hpp"
#include "ut/unit.hpp"

namespace anch::ut {

  /*!
   * Command action
   */
  enum class CommandAction {

    /*! Execute test command */
    TEST = 10,

    /*! Shutdown unit tests runner command */
    SHUTDOWN = 20

  };

  /*!
   * \brief Command message
   *
   * Messages are formatted with \c anch::json
   */
  struct CommandMessage {
    /*! Action (required) */
    CommandAction action;

    /*! Command arguments */
    std::optional<std::vector<std::string>> arg;
  };

  /*!
   * \brief Socket server unit tests runner
   *
   * This runner will execute test on receive messages
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class SocketServerRunner: public anch::ut::DefaultRunner, private anch::events::Observer<anch::network::SocketEvent> {

  private:
    /*! Unit tests */
    anch::ut::UnitTests _tests;

  public:
    /*!
     * Forbids \ref SocketServerRunner default constructor
     */
    SocketServerRunner() = delete;

    /*!
     * Forbids \ref SocketServerRunner copy constructor
     */
    SocketServerRunner(const SocketServerRunner&) = delete;

    /*!
     * \ref SocketServerRunner constructor.\n
     * Initialize unit tests and start socket server
     *
     * \param uri the socket URI
     * \param tests the units tests declaration
     */
    SocketServerRunner(const std::string& uri, const anch::ut::UnitTests& tests);

  public:
    /*!
     * \ref SocketServerRunner destructor
     */
    virtual ~SocketServerRunner();

  public:
    /*!
     * Handle message
     *
     * \param evt the network message
     */
    virtual void handle(const anch::events::Event<anch::network::SocketEvent>& evt) noexcept override;

  };

}  // anch::ut
