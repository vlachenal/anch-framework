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
#include <map>
#include <mutex>

#include "device/networkInterface.hpp"
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
      /** Network interfaces indexed by their names */
      static std::map<std::string,anch::device::NetworkInterface>* _interfaces;

      /** Mutex */
      static std::mutex MUTEX;
      // Attributes -

    private:
      // Constructors +
      /**
       * Network configuration private constructor
       */
      Network();
      // Constructors -

    public:
      // Destructors +
      /**
       * Network configuration destructor
       */
      virtual ~Network();
      // Destructors -

    public:
      // Methods +
      /**
       * Retrieve network interface by its name.
       *
       * @param ifName The interface name
       *
       * @return The interface if found, <code>NULL</code> otherwise
       *
       * @throw anch::device::DeviceException Network interfaces error
       */
      static const anch::device::NetworkInterface* const getInterface(const std::string& ifName)
	throw(anch::device::DeviceException);

      /**
       * Retrieve all network interfaces.
       *
       * @return The network interfaces
       *
       * @throw anch::device::DeviceException Network interfaces error
       */
      static const std::map<std::string,anch::device::NetworkInterface>& getInterfaces()
	throw(anch::device::DeviceException);

      /**
       * Reload network interfaces
       *
       * @throw anch::device::DeviceException Network interfaces error
       */
      static void reload() throw(anch::device::DeviceException);

    private:
      /**
       * Load network interfaces
       *
       * @throw anch::device::DeviceException Network interfaces error
       */
      static void load() throw(anch::device::DeviceException);
      // Methods -

    };

  }
}

#endif // _ANCH_DEVICE_NETWORK_H_