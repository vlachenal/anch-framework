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
#include "rest/mapperRegistry.hpp"

using anch::rest::MapperRegistry;
using anch::rest::GlobalMapperRegistry;


MapperRegistry::MapperRegistry(): _mappers() {
  // Nothing to do
}

MapperRegistry::~MapperRegistry() {
  // Nothing to do
}

GlobalMapperRegistry::GlobalMapperRegistry(): MapperRegistry() {
  // Nothing to do
}

GlobalMapperRegistry::~GlobalMapperRegistry() {
  // Nothing to do
}
