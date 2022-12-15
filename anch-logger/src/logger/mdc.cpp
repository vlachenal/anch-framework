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
#include "logger/mdc.hpp"

#include <sstream>
#include <thread>

using anch::logger::MapDiagnosticContext;

// Constants +
const std::string anch::logger::MDC_THREAD_ID("threadId");
std::map<std::string,std::string> anch::logger::MDC_CONST;
thread_local MapDiagnosticContext anch::logger::MDC;
// Constants -

MapDiagnosticContext::MapDiagnosticContext(): _context(MDC_CONST) {
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  _context[anch::logger::MDC_THREAD_ID] = oss.str();
}

MapDiagnosticContext::~MapDiagnosticContext() {
  // Nothing to do
}

void
MapDiagnosticContext::add(const std::string& key, const std::string& value) noexcept {
  if(key == anch::logger::MDC_THREAD_ID || MDC_CONST.find(key) == MDC_CONST.end()) {
    return;
  }
  put(key, value);
}

void
MapDiagnosticContext::remove(const std::string& key) noexcept {
  if(key == anch::logger::MDC_THREAD_ID || MDC_CONST.find(key) == MDC_CONST.end()) {
    return;
  }
  auto iter = _context.find(key);
  if(iter != _context.end()) {
    _context.erase(iter);
  }
}

void
MapDiagnosticContext::reset() noexcept {
  for(auto iter = _context.begin() ; iter != _context.end() ; ++iter) {
    if(iter->first == anch::logger::MDC_THREAD_ID
       || MDC_CONST.find(iter->first) == MDC_CONST.end()) {
      continue;
    }
    _context.erase(iter);
  }
}

void
MapDiagnosticContext::copy(const MapDiagnosticContext& other) noexcept {
  for(auto iter = other._context.cbegin() ; iter != other._context.cend() ; ++iter) {
    if(iter->first == anch::logger::MDC_THREAD_ID
       || MDC_CONST.find(iter->first) == MDC_CONST.end()) {
      continue;
    }
    _context[iter->first] = iter->second;
  }
}
