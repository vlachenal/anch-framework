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
#include <string>
#include <string_view>
#include <vector>

namespace anch::sql {

  /*!
   * \brief Lexical rules of an SQL dialect
   *
   * Used to find \c '?' placeholders outside string literals, quoted identifiers and comments.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct SqlSyntax {

    /*! Backslash escapes characters in string literals (MySQL) */
    bool backslashEscape = false;

    /*! Double quotes delimit string literals instead of identifiers (MySQL without \c ANSI_QUOTES ) */
    bool doubleQuoteString = false;

    /*! Back quotes delimit identifiers (MySQL, SQLite) */
    bool backquoteIdentifier = false;

    /*! Square brackets delimit identifiers (SQLite) */
    bool bracketIdentifier = false;

    /*! \c E'...' strings with backslash escapes (PostgreSQL) */
    bool escapeString = false;

    /*! Dollar quoted strings \c $tag$...$tag$ (PostgreSQL) */
    bool dollarQuote = false;

    /*! Nested block comments (PostgreSQL) */
    bool nestedComment = false;

    /*! \c '#' line comments (MySQL) */
    bool hashComment = false;

    /*! \c '--' line comments have to be followed by a space (MySQL) */
    bool dashCommentNeedsSpace = false;

    /*! Standard SQL */
    static const SqlSyntax STANDARD;

    /*! PostgreSQL syntax (\c standard_conforming_strings enabled) */
    static const SqlSyntax POSTGRESQL;

    /*! MySQL / MariaDB syntax (default SQL mode) */
    static const SqlSyntax MYSQL;

    /*! SQLite syntax */
    static const SqlSyntax SQLITE;

  };

  /*!
   * Find \c '?' placeholders in SQL query. Placeholders in string literals, quoted identifiers and comments are
   * ignored. An unterminated literal or comment hides the rest of the query (the database reports the syntax error).
   *
   * \param query the SQL query
   * \param syntax the SQL dialect lexical rules
   *
   * \return the placeholders positions
   *
   * \since 0.2
   */
  std::vector<std::size_t> findPlaceholders(std::string_view query, const anch::sql::SqlSyntax& syntax);

  /*!
   * Replace \c '?' placeholders by numbered placeholders (\c $1 , \c $2 , ... for PostgreSQL)
   *
   * \param query the SQL query
   * \param positions the placeholders positions (from \ref findPlaceholders )
   * \param prefix the numbered placeholder prefix
   *
   * \return the query with numbered placeholders
   *
   * \since 0.2
   */
  std::string numberPlaceholders(std::string_view query, const std::vector<std::size_t>& positions, char prefix = '$');

}  // anch::sql
