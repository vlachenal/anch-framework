/*
    This file is part of ANCH Framework.

    ANCH Framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ANCH Framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _ANCH_LOGGER_LEVEL_H_
#define _ANCH_LOGGER_LEVEL_H_

#include <iostream>
#include <map>

namespace anch {
  namespace logger {

    /**
     * Logging levels definitions
     *
     * @author Vincent Lachenal
     */
    enum Level {
      TRACE = 0,
      DEBUG = 10,
      INFO = 20,
      WARN = 30,
      ERROR = 40,
      FATAL = 50
    };

  }
}

namespace anch {
  namespace logger {

    /**
     * Level labels for logging facilities
     *
     * @author Vincent Lachenal
     */
    const std::map<const Level,const std::string> LEVEL_LABEL = {
      {Level::TRACE,"TRACE"},
      {Level::DEBUG,"DEBUG"},
      {Level::INFO,"INFO"},
      {Level::WARN,"WARN"},
      {Level::ERROR,"ERROR"},
      {Level::FATAL,"FATAL"},
    };

  }
}

namespace anch {
  namespace logger {

    /**
     * Level labels for logging facilities
     *
     * @author Vincent Lachenal
     */
    const std::map<const std::string,const Level> LABEL_LEVEL = {
      {"TRACE",Level::TRACE},
      {"DEBUG",Level::DEBUG},
      {"INFO",Level::INFO},
      {"WARN",Level::WARN},
      {"ERROR",Level::ERROR},
      {"FATAL",Level::FATAL},
    };

  }
}

#endif // _ANCH_LOGGER_LEVEL_H_
