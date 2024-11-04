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
#include <optional>
#include <list>

#include "oas3/operation.hpp"
#include "oas3/server.hpp"

namespace anch::oas3 {

  struct PathItem {

    /*! Path reference ?? */
    std::optional<std::string> _ref;

    /*! Summary */
    std::optional<std::string> summary;

    /*! Description */
    std::optional<std::string> description;

    /*! GET declaration */
    std::optional<anch::oas3::Operation> getVerb;

    /*! PUT declaration */
    std::optional<anch::oas3::Operation> putVerb;

    /*! POST declaration */
    std::optional<anch::oas3::Operation> postVerb;

    /*! DELETE declaration */
    std::optional<anch::oas3::Operation> deleteVerb;

    /*! OPTIONS declaration */
    std::optional<anch::oas3::Operation> optionsVerb;

    /*! HEAD declaration */
    std::optional<anch::oas3::Operation> headVerb;

    /*! PATCH declaration */
    std::optional<anch::oas3::Operation> patchVerb;

    /*! TRACE declaration */
    std::optional<anch::oas3::Operation> traceVerb;

    /*! Additional servers declaration ... */
    std::optional<std::list<anch::oas3::Server>> servers;

    // \todo generic paths parameters

  };

}
