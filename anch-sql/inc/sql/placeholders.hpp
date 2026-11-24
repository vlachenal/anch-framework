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

#include "anch-sql-export.hpp"

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace anch::sql {

  /*!
   * \brief Lexical rules of an SQL dialect
   *
   * Used to find named parameters ( \ref NamedQuery ) outside string literals, quoted identifiers and comments.
   *
   * \author Vincent Lachenal
   *
   * \since 0.2
   */
  struct ANCH_SQL_EXPORT SqlSyntax {

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

    /*! \c '?' is an operator, not a parameter (PostgreSQL \c jsonb operators \c ? , \c ?| , \c ?& ) */
    bool questionMarkOperator = false;

    /*! \c ':' between \c '[' and \c ']' separates array slice bounds (PostgreSQL \c arr[1:n] ) */
    bool arraySlice = false;

    /*! \c $1 is a positional parameter (PostgreSQL) */
    bool dollarParameters = false;

    /*! \c \@name and \c $name are parameters (SQLite) */
    bool prefixedParameters = false;

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
   * \brief Parameters style of a database driver ( \ref NamedQuery::render )
   *
   * \since 0.3
   */
  enum class ParameterStyle {
    /*! \c $1 , \c $2 , ... repeated for each use of a name ; lists are arrays (PostgreSQL) */
    DOLLAR,
    /*! \c ?1 , \c ?2 , ... repeated for each use of a name ; lists are expanded (SQLite) */
    NUMBERED,
    /*! \c ? for each use of a name ; lists are expanded (MySQL) */
    POSITIONAL
  };

  /*!
   * \brief SQL query with named parameters ( \c :name )
   *
   * The query is parsed once (\ref SqlSyntax : parameters in string literals, quoted identifiers and comments are
   * ignored) and rendered in the parameters style of a driver ( \ref render ). A parameter is \c ':' followed by
   * a name ( \c [A-Za-z_][A-Za-z0-9_]* ), which is not preceded by an identifier character or by \c ':'
   * ( \c x::int is a cast, \c := an assignment) and which is not an array slice bound (PostgreSQL \c arr[1:n] ;
   * a parameter in brackets is written in parentheses: \c arr[(:i)] ). A name can be used several times: its value
   * is bound once.\n
   * A parameter which is the only item of an \c IN list ( \c IN (:ids) , \c NOT IN (:ids) ) can be bound to a
   * list of values: PostgreSQL binds it as an array ( \c = ANY($1) , \c <> ALL($1) ), other databases expand the
   * list ( \c IN (?, ?, ?) ; an empty list is an empty subquery: \c IN is false and \c NOT IN true, as \c ANY and
   * \c ALL of an empty array).\n
   * Positional parameters ( \c ? , PostgreSQL \c $1 , SQLite \c ?1 , \c \@name and \c $name ) are refused.
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  class ANCH_SQL_EXPORT NamedQuery {

  public:
    /*! Element of a value which is not a list */
    static constexpr std::size_t SCALAR = static_cast<std::size_t>(-1);

    /*!
     * \brief Value bound to a parameter of the rendered query
     */
    struct Binding {
      /*! The name index (see \ref names ) */
      std::size_t slot;

      /*! The list element, \ref SCALAR for the whole value (a value which is not a list, or a PostgreSQL array) */
      std::size_t element;

      /*! Bindings are equal when they have the same name and element */
      bool operator==(const Binding&) const = default;
    };

    /*!
     * \brief Query rendered for a driver
     */
    struct Rendered {
      /*! The query in the parameters style of the driver */
      std::string query;

      /*! The value of each parameter of the rendered query (\c $1 or \c ?1 is the first one ; MySQL: each \c ? ) */
      std::vector<Binding> bindings;
    };

  private:
    /*! Use of a name in the query */
    struct Occurrence {
      /*! Parameter begin (the \c ':' ) */
      std::size_t begin;

      /*! Parameter end (after the name) */
      std::size_t end;

      /*! Name index */
      std::size_t slot;

      /*! The parameter is the only item of an \c IN list */
      bool in;

      /*! \c NOT IN */
      bool negated;

      /*! \c IN (or \c NOT ) keyword begin */
      std::size_t inBegin;

      /*! \c IN list opening parenthesis */
      std::size_t open;

      /*! \c IN list closing parenthesis */
      std::size_t close;
    };

    // Attributes +
    /*! The query */
    std::string _query;

    /*! The names, in order of first use */
    std::vector<std::string> _names;

    /*! Names uses */
    std::vector<Occurrence> _occurrences;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref NamedQuery constructor: parse query
     *
     * \param query the SQL query
     * \param syntax the SQL dialect lexical rules
     *
     * \throw SqlException positional parameter in query (non fatal)
     */
    NamedQuery(std::string_view query, const anch::sql::SqlSyntax& syntax);
    // Constructors -

    // Methods +
  public:
    /*!
     * Query getter
     *
     * \return the query
     */
    const std::string& query() const noexcept;

    /*!
     * Names getter
     *
     * \return the parameters names, in order of first use
     */
    const std::vector<std::string>& names() const noexcept;

    /*!
     * Find name index
     *
     * \param name the parameter name
     *
     * \return the name index, \c std::nullopt when the query has no such parameter
     */
    std::optional<std::size_t> slot(std::string_view name) const noexcept;

    /*!
     * Render query in the parameters style of a driver
     *
     * \param style the parameters style
     * \param sizes the list size bound to each name ( \ref SCALAR for a value which is not a list), empty when no
     *        list is bound
     *
     * \return the rendered query and its bindings
     *
     * \throw SqlException a list is bound to a parameter which is not the only item of an \c IN list (it can be
     *        used anywhere as a PostgreSQL array), or \c sizes does not have a size for each name (non fatal)
     */
    Rendered render(anch::sql::ParameterStyle style, std::span<const std::size_t> sizes = {}) const;

    /*!
     * Render query with other parameters names (queries built from parts whose names collide)
     *
     * \param names the new name of each name (see \ref names )
     *
     * \return the query with the new names
     *
     * \throw SqlException \c names does not have a name for each name (non fatal)
     */
    std::string rename(std::span<const std::string> names) const;
    // Methods -

  };

}  // anch::sql
