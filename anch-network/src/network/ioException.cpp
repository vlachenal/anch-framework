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
#include "network/ioException.hpp"

#include <netdb.h>

using anch::network::IOException;

namespace {

  /*!
   * \brief \c getaddrinfo error category
   */
  class AddrinfoCategory: public std::error_category {
  public:
    virtual const char* name() const noexcept override {
      return "getaddrinfo";
    }

    virtual std::string message(int code) const override {
      return ::gai_strerror(code);
    }
  };

}

const std::error_category&
anch::network::addrinfoCategory() noexcept {
  static const AddrinfoCategory category;
  return category;
}

// Constructors +
IOException::IOException(const std::string& message, int errnum):
  std::system_error(errnum, std::generic_category(), message) {
  // Nothing to do
}

IOException::IOException(const std::string& message, const std::error_code& code):
  std::system_error(code, message) {
  // Nothing to do
}
// Constructors -

// Destructor +
IOException::~IOException() noexcept {
  // Nothing to do
}
// Destructor -
