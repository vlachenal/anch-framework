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
#include <mutex>

#include "resource/resource.hpp"


using std::mutex;

// Static initialization +
mutex Resource::_mutex;
Resource* Resource::_self = NULL;
// Static initialization -


/**
 * {@link Resource} private constructor
 */
Ressource::Ressource() {
  // Nothing to do
}

/**
 * {@link Resource} destructor
 */
Resource::~Ressource() {
  // Nothing to do
}

/**
 * Get Resource unique instance
 *
 * @return The {@link Resource} unique instance
 */
Resource*
Resource::getInstance() {
  _mutex.lock();
  if(_self == NULL) {
    _self = new Resource();
  }
  _mutex.unlock();
  return _self;
}
