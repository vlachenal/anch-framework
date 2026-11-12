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

#include <cstddef>

#include "conf/section.hpp"
#include "conf/confError.hpp"

namespace anch::conf {

  /*!
   * \brief Configuration placeholders resolution
   *
   * A placeholder is <tt>${alternative|alternative|...}</tt> in a configuration value. An alternative is
   * either <tt>resolver=argument</tt> (\c env=HOME , \c conf=section.key , registered resolvers) or a
   * default value. The first alternative which resolves is used:
   * \code{.ini}
url=http://${env=HOST|localhost}:${conf=server.port}/
   * \endcode
   *
   * Resolution is event driven: values without pending dependency are resolved first; each resolved
   * value is written in its section and fires a "resolved" event in a synchronous queue. Values which
   * reference it with \c conf= are then completed and fire their own event, until the queue is empty.\n
   * \n
   * Resolved values are literal: a value which contains <tt>${...}</tt> after resolution (for example an
   * environment variable) is not interpreted again. With the watchdog, resolution always terminates:
   * - a resolution chain longer than the maximum depth is an error (\c CYCLE )
   * - values still waiting when the queue is empty reference each other: the cycle is an error (\c CYCLE )
   *
   * Other errors: unknown resolver (\c RES_NOT_FOUND ), placeholder without resolvable alternative
   * (\c NOT_RES ), invalid syntax such as nested placeholders (\c PARSING_ERROR ).
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  class Placeholders {

    // Attributes +
  public:
    /*! Default maximum resolution chain length */
    static constexpr std::size_t DEFAULT_MAX_DEPTH = 64;

  private:
    /*! Root section */
    anch::conf::Section& _section;

    /*! Maximum resolution chain length (watchdog) */
    std::size_t _maxDepth;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref Placeholders constructor
     *
     * \param section the root section (resolved values are written in it)
     * \param maxDepth the maximum resolution chain length
     */
    explicit Placeholders(anch::conf::Section& section, std::size_t maxDepth = DEFAULT_MAX_DEPTH) noexcept;

    /*! Prohibit \ref Placeholders copy */
    Placeholders(const Placeholders&) = delete;
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref Placeholders destructor
     */
    virtual ~Placeholders() noexcept;
    // Destructor -

    // Methods +
  public:
    /*!
     * Resolve every placeholder of the section and its subsections
     *
     * \return the number of resolved values
     *
     * \throw anch::conf::ConfError on resolution error
     */
    std::size_t resolve();

    /*! Prohibit \ref Placeholders copy */
    Placeholders& operator=(const Placeholders&) = delete;
    // Methods -

  };

}
