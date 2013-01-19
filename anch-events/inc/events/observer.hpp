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
#ifndef _ANCH_EVENTS_OSERVER_H_
#define _ANCH_EVENTS_OSERVER_H_

namespace anch {
  namespace events {

    /**
     * Observer abstract class.<br>
     * It has to be implemented to handle events correctly.
     *
     * @author Vincent Lachenal
     */
    template<typename T>
    class Observer {

    private:
      /** Observer identifier */
      uint16_t _identifier;

    public:
      // Methods +
      /**
       * Receive event notification
       *
       * @param event The event to receive
       */
      virtual void notify(const T& event) const throw() = 0;
      // Methods -

    public:
      // Accessors +
      /**
       * Observer identifier getter
       *
       * @return The observer identifier
       */
      inline uint16_t getIdentifier() const {
	return _identifier;
      }

      /**
       * Observer identifier setter
       *
       * @param identifier The observer identifier to set
       */
      inline void setIdentifier(uint16_t  identifier) {
	_identifier = identifier;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_EVENTS_OSERVER_H_
