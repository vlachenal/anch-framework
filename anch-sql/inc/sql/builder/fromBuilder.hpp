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


namespace anch {
  namespace sql {

    class SelectBuilder;
    class SQLQuery;
    class ClausesBuilder;

    /*!
     * SQL from table builder
     *
     * \since 0.1
     *
     * \author Vincent Lachenal
     */
    class FromBuilder: public anch::sql::SelectBuilderPart {

      // Constructors +
    public:
      /*!
       * Prohibits \ref FromBuilder default constructor
       */
      FromBuilder() = delete;

      /*!
       * \ref FromBuilder constructor
       *
       * \param select the base \ref SelectBuilder
       * \param table the table
       */
      FromBuilder(anch::sql::SelectBuilder& select, const std::string& table);

      /*!
       * \ref FromBuilder constructor
       *
       * \param select the base \ref SelectBuilder
       * \param subquery the subquery
       */
      FromBuilder(anch::sql::SelectBuilder& select, const anch::sql::SelectBuilder& subquery);

      /*!
       * \ref FromBuilder constructor
       *
       * \param select the base \ref SelectBuilder
       * \param subquery the subquery
       */
      FromBuilder(anch::sql::SelectBuilder& select, const anch::sql::SQLQuery& subquery);

      /*!
       * \ref FromBuilder constructor
       *
       * \param select the base \ref SelectBuilder
       * \param subquery the subquery
       * \param alias the 'table' alias
       */
      FromBuilder(anch::sql::SelectBuilder& select, const anch::sql::SelectBuilder& subquery, const std::string& alias);

      /*!
       * \ref FromBuilder constructor
       *
       * \param select the base \ref SelectBuilder
       * \param subquery the subquery
       * \param alias the 'table' alias
       */
      FromBuilder(anch::sql::SelectBuilder& select, const anch::sql::SQLQuery& subquery, const std::string& alias);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref FromBuilder destructor
       */
      virtual ~FromBuilder();
      // Destructor -

      // SQL commands +
    public:
      /*!
       * Add table to 'from' part (old school join)
       *
       * \param table the table to add
       *
       * \return \c this
       */
      FromBuilder& selfJoin(const std::string& table);

      /*!
       * Add table to 'from' part (old school join)
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       *
       * \return \c this
       */
      FromBuilder& selfJoin(const anch::sql::SQLQuery& subquery, const std::string& alias);

      /*!
       * Add table to 'from' part (old school join)
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       *
       * \return \c this
       */
      FromBuilder& selfJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias);

      /*!
       * Add inner join on other table
       *
       * \param table the table
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& innerJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add inner join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& innerJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add inner join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& innerJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add inner join on other table
       *
       * \param table the table
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& join(const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add inner join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& join(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add inner join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& join(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add left outer join on other table
       *
       * \param table the table
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& leftOuterJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add left outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& leftOuterJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add left outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& leftOuterJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add left outer join on other table
       *
       * \param table the table
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& leftJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add left outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& leftJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add left outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& leftJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add right outer join on other table
       *
       * \param table the table
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& rightOuterJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add right outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& rightOuterJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add right outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& rightOuterJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add right outer join on other table
       *
       * \param table the table
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& rightJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add right outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& rightJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add right outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& rightJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add full outer join on other table
       *
       * \param table the table
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& fullOuterJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add full outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& fullOuterJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add full outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      FromBuilder& fullOuterJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add full outer join on other table
       *
       * \param table the table
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& fullJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add full outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& fullJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add full outer join on other table
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       * \param clauses the join clauses
       *
       * \return \c this
       */
      inline FromBuilder& fullJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add natural join on other table
       *
       * \param table the table to add
       *
       * \return \c this
       */
      FromBuilder& naturalJoin(const std::string& table);

      /*!
       * Add natural join on other 'table'
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       *
       * \return \c this
       */
      FromBuilder& naturalJoin(const anch::sql::SQLQuery& subquery, const std::string& alias);

      /*!
       * Add natural join on other 'table'
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       *
       * \return \c this
       */
      FromBuilder& naturalJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias);

      /*!
       * Add cross join on other table
       *
       * \param table the table to add
       *
       * \return \c this
       */
      FromBuilder& crossJoin(const std::string& table);

      /*!
       * Add cross join on other 'table'
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       *
       * \return \c this
       */
      FromBuilder& crossJoin(const anch::sql::SQLQuery& subquery, const std::string& alias);

      /*!
       * Add cross join on other 'table'
       *
       * \param subquery the subquery to add as table
       * \param alias the 'table' alias
       *
       * \return \c this
       */
      FromBuilder& crossJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias);

      /*!
       * Add 'WHERE' and clauses
       *
       * \param clauses the clauses to add
       *
       * \return the \ref SelectBuilder
       */
      anch::sql::SelectBuilder& where(const anch::sql::ClausesBuilder& clauses);
      // SQL commands -

      // Methods +
    private:
      /*!
       * Add join
       *
       * \param join the join type
       * \param subquery the subquery
       * \param alias the 'table' alias
       */
      void addJoin(const std::string& join, const anch::sql::SelectBuilder& subquery, const std::string& alias);

      /*!
       * Add join
       *
       * \param join the join type
       * \param subquery the subquery
       * \param alias the 'table' alias
       */
      void addJoin(const std::string& join, const anch::sql::SQLQuery& subquery, const std::string& alias);

      /*!
       * Add join
       *
       * \param join the join type
       * \param table the table
       * \param clauses the join clauses
       */
      void addJoin(const std::string& join, const std::string& table, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add join
       *
       * \param join the join type
       * \param subquery the subquery
       * \param alias the 'table' alias
       * \param clauses the join clauses
       */
      void addJoin(const std::string& join, const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);

      /*!
       * Add join
       *
       * \param join the join type
       * \param subquery the subquery
       * \param alias the 'table' alias
       * \param clauses the join clauses
       */
      void addJoin(const std::string& join, const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses);
      // Methods -

    };


    FromBuilder&
    FromBuilder::join(const std::string& table, const anch::sql::ClausesBuilder& clauses) {
      return innerJoin(table, clauses);
    }

    FromBuilder&
    FromBuilder::join(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses) {
      return innerJoin(subquery, alias, clauses);
    }

    FromBuilder&
    FromBuilder::join(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses) {
      return innerJoin(subquery, alias, clauses);
    }

    FromBuilder&
    FromBuilder::leftJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses) {
      return leftOuterJoin(table, clauses);
    }

    FromBuilder&
    FromBuilder::leftJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses) {
      return leftOuterJoin(subquery, alias, clauses);
    }

    FromBuilder&
    FromBuilder::leftJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses) {
      return leftOuterJoin(subquery, alias, clauses);
    }

    FromBuilder&
    FromBuilder::rightJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses) {
      return rightOuterJoin(table, clauses);
    }

    FromBuilder&
    FromBuilder::rightJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses) {
      return rightOuterJoin(subquery, alias, clauses);
    }

    FromBuilder&
    FromBuilder::rightJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses) {
      return rightOuterJoin(subquery, alias, clauses);
    }

    FromBuilder&
    FromBuilder::fullJoin(const std::string& table, const anch::sql::ClausesBuilder& clauses) {
      return fullOuterJoin(table, clauses);
    }

    FromBuilder&
    FromBuilder::fullJoin(const anch::sql::SQLQuery& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses) {
      return fullOuterJoin(subquery, alias, clauses);
    }

    FromBuilder&
    FromBuilder::fullJoin(const anch::sql::SelectBuilder& subquery, const std::string& alias, const anch::sql::ClausesBuilder& clauses) {
      return fullOuterJoin(subquery, alias, clauses);
    }

  }  // sql
}  // anch
