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

namespace anch {
  namespace json {

    template<typename T>
    void deserializeArray(std::istream& input, std::function<void(const T&)> pushFunc, std::function<auto((T& value, std::istream& input))> deserializeNonNull) {
      if(anch::json::isNull(input)) {
	return;
      }
      int current = input.get();
      if(current != anch::json::ARRAY_BEGIN) {
	throw 128;
      }
      anch::json::discardChars(input);
      if(input.peek() != anch::json::ARRAY_END) {
	while(input) {
	  T num;
	  std::invoke(deserializeNonNull, num, input);
	  std::invoke(pushFunc, num);
	  if(!anch::json::hasMoreField(input)) {
	    break;
	  }
	  anch::json::discardChars(input);
	}
      }
      if(!input || input.get() != anch::json::ARRAY_END) {
	throw 2048; // \todo error ...
      }
    }

  }  // json
}  // anch
