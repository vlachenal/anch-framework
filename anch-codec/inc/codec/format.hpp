/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

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

#include "anch-codec-export.hpp"

#include <string>
#include <vector>

namespace anch::codec {

  /*!
   * Format supported by the program: anch-codec (\c text ) and each codec plugin (\c json , ...) register their
   * format when their library is loaded. Formats are only described (introspection: startup logs, accepted media
   * types, ...): values are serialized with codecs types (see \ref anch::codec::Codecs "Codecs").
   *
   * \author Vincent Lachenal
   *
   * \since 0.3
   */
  struct ANCH_CODEC_EXPORT Format {
    /*! Format name (\c text , \c json , ...) */
    std::string name;

    /*! Media types handled by the format codecs (\c application/json , ...) */
    std::vector<std::string> mediaTypes;
  };

  /*!
   * Register format. A format which is already registered (same name, from several shared libraries for example)
   * is not registered twice.
   *
   * \param format the format
   *
   * \return \c true when format has been registered, \c false when it was already registered
   *
   * \since 0.3
   */
  ANCH_CODEC_EXPORT bool registerFormat(anch::codec::Format format);

  /*!
   * Get registered formats (thread safe)
   *
   * \return the formats sorted by name
   *
   * \since 0.3
   */
  ANCH_CODEC_EXPORT std::vector<anch::codec::Format> formats();

}
