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
#ifndef _ANCH_RESOURCE_RESOURCE_H_
#define _ANCH_RESOURCE_RESOURCE_H_

#include <mutex>

namespace anch {
  namespace resource {

    class Ressource {
    private:
      /** {@link Resource} unique instance */
      static Resource* _self;

      /** Mutex */
      static std::mutex _mutex;

    private:
      /**
       * {@link Resource} private constructor
       */
      Ressource();

      /**
       * {@link Resource} destructor
       */
      virtual ~Ressource();

    public:
      /**
       * Get Resource unique instance
       *
       * @return The {@link Resource} unique instance
       */
      static Resource* getInstance();
    };

  }
}

#endif // _ANCH_RESOURCE_RESOURCE_H_
