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
#ifndef _ANCH_DEVICE_NETWORK_INTERFACE_H_
#define _ANCH_DEVICE_NETWORK_INTERFACE_H_

#include <iostream>

#include <net/if.h>


namespace anch {
  namespace device {

    /**
     * Network interface
     *
     * @author Vincent Lachenal
     */
    class NetworkInterface {
    private:
      // Attributes +
      /** Interface name */
      std::string _name;

      /** Interface IPV4 address */
      std::string _ipAddress;

      /** Hardware address */
      std::string _macAddress;

      /** Interface broadcast address */
      std::string _broadcastAddress;

      /** Interface netmask */
      std::string _netmask;

      /** Interface MTU */
      int _mtu;

      /** Interface metric */
      int _metric;
      // Attributes -

    public:
      // Constructor +
      /**
       * {@link NetworkInterface} constructor.
       *
       * @param interface Data retrieved through <code>ioctl</code> POSIX call
       */
      NetworkInterface(const struct ifreq& interface);
      // Constructor -

      // Destructor +
      /**
       * {@link NetworkInterface} destructor
       */
      virtual ~NetworkInterface();
      // Destructor -

    public:
      // Accessors +
      /**
       * Interface name getter
       *
       * @return The interface name
       */
      inline const std::string& getName() const {
	return _name;
      }

      /**
       * Interface IP address getter
       *
       * @return The interface IP address
       */
      inline const std::string& getIpAddress() const {
	return _ipAddress;
      }

      /**
       * MAC address getter
       *
       * @return The MAC address
       */
      inline const std::string& getMacAddress() const {
	return _macAddress;
      }

      /**
       * Interface broadcast address getter
       *
       * @return The interface broadcast address
       */
      inline const std::string& getBroadcastAddress() const {
	return _broadcastAddress;
      }

      /**
       * Interface netmask getter
       *
       * @return The interface netmask
       */
      inline const std::string& getNetmask() const {
	return _netmask;
      }

      /**
       * Interface MTU getter
       *
       * @return The interface MTU
       */
      inline int getMtu() const {
	return _mtu;
      }

      /**
       * Interface metric getter
       *
       * @return The interface metric
       */
      inline int getMetric() const {
	return _metric;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_DEVICE_NETWORK_INTERFACE_H_
