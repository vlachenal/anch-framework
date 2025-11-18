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

#include <map>
#include <set>
#include <functional>

#include "singleton.hpp"
#include "log/writer.hpp"


namespace anch::log {

  /*!
   * Create writer definition
   *
   * \param threadSafe default thread safe flag
   * \param lowPriority default low priority flag
   * \param section the writer section
   *
   * \return the new writer
   */
  using CreateWriter = std::function<anch::log::Writer*(bool, bool, const anch::conf::Section&)>;

  /*!
   * \brief Logger writers' registry
   *
   * Global logger writers' registry
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class WriterRegistry: public anch::Singleton<WriterRegistry> {
    friend anch::Singleton<WriterRegistry>;

    // Attributes +
  private:
    /*! Writers creator registry */
    std::map<std::string, anch::log::CreateWriter> _registry;

    /*! Writers registry */
    std::map<std::string, anch::log::Writer*> _writers;
    // Attributes -

    // Constructors +
  private:
    /*!
     * \ref WriterRegistry default private constructor
     */
    WriterRegistry();

  public:
    /*!
     * Forbids \ref WriterRegistry copy constructor
     *
     * \param other the \ref WriterRegistry not to copy
     */
    WriterRegistry(const WriterRegistry& other) = delete;

    /*!
     * Forbids \ref WriterRegistry move constructor
     *
     * \param other the \ref WriterRegistry not to move
     */
    WriterRegistry(WriterRegistry&& other) = delete;
    // Constructors -

    // Destructor +
  private:
    /*!
     * \ref WriterRegistry private destructor
     */
    virtual ~WriterRegistry();
    // Destructor -

    // Methods +
  public:
    /*!
     * Register writer
     *
     * \param name the writer's name
     * \param writerBuilder the wrtier builder function
     */
    void registerWriter(const std::string& name, const anch::log::CreateWriter& writerBuilder);

    /*!
     * Check if a writer's name has been registered
     *
     * \param name the writer name to check
     *
     * \return \c true when writer has been registered, \c false otherwise
     */
    bool contains(const std::string& name) const;

    /*!
     * Create new writer instance
     *
     * \param name the writer's name
     * \param threadSafe default thread safe flag
     * \param lowPriority default low priority flag
     * \param section the writer section
     *
     * \return the new writer
     */
    anch::log::Writer* create(const std::string& name,
			      bool threadSafe,
			      bool lowPriority,
			      const anch::conf::Section& section);
    // Methods -

  };

}

#include "log/impl/writerRegistry.hpp"
