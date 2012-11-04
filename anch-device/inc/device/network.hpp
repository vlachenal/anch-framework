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
#ifndef _ANCH_DEVICE_NETWORK_H_
#define _ANCH_DEVICE_NETWORK_H_

#include <iostream>

#include "device/deviceException.hpp"


namespace anch {
  namespace device {

    /**
     * Network device utilities class
     *
     * @author Vincent Lachenal
     */
    class Network {

    private:
      // Attributes +
      /** Network unique instance */
      static Network* _self;

      /** Network interface name */
      std::string _ifaceName;

      /** Broadcast address */
      std::string _broadcastAddr;
      // Attributes -

    private:
      // Constructors +
      /**
       * Network configuration private constructor
       *
       * @param ifaceName Network interface name
       *
       * @throw anch::device::DeviceException Device initialization error
       */
      Network(const std::string& ifaceName)
	throw(anch::device::DeviceException);
      // Constructors -

      // Destructors +
      /**
       * Network configuration destructor
       */
      virtual ~Network();
      // Destructors -

    public:
      // Methods +
      /**
       * Initialize network configuration
       *
       * @param ifaceName Network interface name
       *
       * @throw anch::device::DeviceException Device initialization error
       */
      static void initialize(const std::string& ifaceName)
	throw(anch::device::DeviceException);
      // Methods -

      // Accessors +
      /**
       * Get broadcast address
       *
       * @return The broadcast address
       */
      static inline const std::string& getBroadcastAddress() {
	return _self->_broadcastAddr;
      }

      /**
       * Get the interface name
       *
       * @return The interface name
       */
      static inline const std::string& getInterfaceName() {
	return _self->_ifaceName;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_DEVICE_NETWORK_H_
