/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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

#include <ostream>

namespace anch::json {

  /*!
   * Serialize string value
   *
   * \tparam T the type to serialize as string
   *
   * \param value the value to serialize
   * \param out the output stream to write in
   * \param options the mapping options
   */
  template<typename T>
  inline
  void
  serializeNumericValue(const T& value,
			std::ostream& out,
		       [[maybe_unused]] const anch::json::MappingOptions& options) {
    out << value;
  }

}
