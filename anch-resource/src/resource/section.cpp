/*
    This file is part of Configuration resource manager.

    Configuration resource manager is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Configuration resource manager is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Configuration resource manager.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>

#include "resource/section.hpp"


using std::string;
using resource::Section;


// Static initialization +
string Section::DEFAULT_VALUE = "";
// Static initialization -


// Constructors +
/**
 * Section constructor
 */
Section::Section() {
  // Nothing to do
}
// Constructors -

// Destructors +
/**
 * Section destructor
 */
Section::~Section() {
  // Nothing to do
}
// Destructors -

