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

#include "sql/builder/selectBuilderPart.hpp"
#include "sql/builder/fromBuilder.hpp"


namespace anch {
  namespace sql {

    class SelectBuilder;

    /**
     * \brief Field builder
     *
     * This class will be used to append selected 'columns' in \c SELECT queries
     * and in <tt>ORDER BY</tt> and <tt>GROUP BY</tt> command.
     *
     * @since 0.1
     *
     * @author Vincent Lachenal
     */
    class FieldsBuilder: public SelectBuilderPart {
      friend anch::sql::SelectBuilder;

      // Constructors +
    private:
      /*!
       * Prohibits \ref FieldsBuilder default constructor
       */
      FieldsBuilder() = delete;

      /*!
       * \ref FieldsBuilder constructor
       *
       * \param select the \ref SelectBuilder
       * \param column the column
       */
      FieldsBuilder(anch::sql::SelectBuilder& select, const std::string& column);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref FieldsBuilder destructor
       */
      virtual ~FieldsBuilder();
      // Destructor -

      // SQL commands +
    public:
      /*!
       * Add new field
       *
       * \param column the column
       *
       * \return \c this
       */
      FieldsBuilder& field(const std::string& column);

      /*!
       * Add \c AS alias on field
       *
       * \param alias the alias
       *
       * \return \c this
       */
      FieldsBuilder& as(const std::string& alias);

      /*!
       * Add \c ASC order by direction.
       *
       * @return \c this
       */
      FieldsBuilder& asc();

      /*!
       * Add \c DESC order by direction.
       *
       * \return \c this
       */
      FieldsBuilder& desc();

      /*!
       * Switch to 'from' builder
       *
       * \param table the table where columns are selected from
       *
       * \return the new \ref FromBuilder
       */
      FromBuilder from(const std::string& table);

      // \todo from
      // SQL commands -

    };

  }  // sql
}  // anch
