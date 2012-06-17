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
#include "resource/section.hpp"


using std::string;
using anch::resource::Section;


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

/**
 * {@link Section} copy constructor
 */
Section::Section(const Section& section): _parameters(section._parameters) {
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

