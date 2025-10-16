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
#include <map>

namespace anch::logger {

  /*! Thread ID key in diagnostic context */
  extern const std::string MDC_THREAD_ID;

  /*! MDC constant values */
  extern std::map<std::string,std::string> MDC_CONST;

  /*!
   * \brief Map Diagnostic Context
   *
   * Logger diagntostic context for logger (or other usage)
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class MapDiagnosticContext {

    // Attributes +
  private:
    /*! Thread context */
    std::map<std::string,std::string> _context;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref MapDiagnosticContext default constructor
     */
    MapDiagnosticContext();

    /*!
     * Forbids \ref MapDiagnosticContext copy constructor
     */
    MapDiagnosticContext(const MapDiagnosticContext&) = delete;

    /*!
     * Forbids \ref MapDiagnosticContext move constructor
     */
    MapDiagnosticContext(MapDiagnosticContext&&) = delete;
    // Constructors -

    // Destructors +
  public:
    /*!
     * Private \ref MapDiagnosticContext move destructor
     */
    virtual ~MapDiagnosticContext();
    // Destructors -

    // Methods +
  public:
    /*!
     * Add value to context if not exists
     *
     * \param key the key
     * \param value the value
     */
    void add(const std::string& key, const std::string& value) noexcept;

    /*!
     * Add or overwrite value to context
     *
     * \param key the key
     * \param value the value
     */
    void put(const std::string& key, const std::string& value) noexcept;

    /*!
     * Remove value from context
     *
     * \param key the key
     */
    void remove(const std::string& key) noexcept;

    /*!
     * Retrieve the map diagnostic context
     *
     * \return the map diagnostic context
     */
    const std::map<std::string,std::string>& get() const noexcept;

    /*!
     * Reset diagnostics context but thread identifier and constants
     */
    void reset() noexcept;


    /*!
     * Copy diagnostics context from another one but thread identifier and constants
     *
     * \param other the diagnostic context to copy
     */
    void copy(const MapDiagnosticContext& other) noexcept;
    // Methods -

  };

  /*! \ref MapDiagnosticContext current thread instance */
  extern thread_local MapDiagnosticContext MDC;

}

#include "logger/impl/mdc.hpp"
