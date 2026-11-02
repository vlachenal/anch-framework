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
#include "log/mdc.hpp"

#include <sstream>
#include <thread>

using anch::log::MapDiagnosticContext;

// Constants +
const std::string anch::log::MDC_THREAD_ID("threadId");
std::map<std::string,std::string> anch::log::MDC_CONST;
thread_local MapDiagnosticContext anch::log::MDC;
// Constants -

MapDiagnosticContext::MapDiagnosticContext(): _context(MDC_CONST) {
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  _context[anch::log::MDC_THREAD_ID] = oss.str();
}

MapDiagnosticContext::~MapDiagnosticContext() {
  // Nothing to do
}

namespace {

  /*!
   * Check if key can not be modified by \c add, \c remove, \c reset and \c copy
   *
   * \param key the key
   *
   * \return \c true for thread identifier and constants, \c false otherwise
   */
  bool
  isProtected(const std::string& key) {
    return key == anch::log::MDC_THREAD_ID || anch::log::MDC_CONST.contains(key);
  }

}

void
MapDiagnosticContext::add(const std::string& key, const std::string& value) noexcept {
  if(isProtected(key)) {
    return;
  }
  _context.try_emplace(key, value); // only if not exists
}

void
MapDiagnosticContext::remove(const std::string& key) noexcept {
  if(isProtected(key)) {
    return;
  }
  _context.erase(key);
}

void
MapDiagnosticContext::reset() noexcept {
  for(auto iter = _context.begin() ; iter != _context.end() ; ) {
    if(isProtected(iter->first)) {
      ++iter;
    } else {
      iter = _context.erase(iter);
    }
  }
}

void
MapDiagnosticContext::copy(const MapDiagnosticContext& other) noexcept {
  for(const auto& [key, value] : other._context) {
    if(!isProtected(key)) {
      _context[key] = value;
    }
  }
}
