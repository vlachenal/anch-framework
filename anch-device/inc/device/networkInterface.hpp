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
#pragma once

#include <iostream>

#include <net/if.h>

#include "device/deviceException.hpp"


namespace anch {
  namespace device {

    /*!
     * Network interface
     *
     * \author Vincent Lachenal
     */
    class NetworkInterface {
    private:
      // Attributes +
      /*! Interface name */
      std::string _name;

      /*! Interface IPV4 address */
      std::string _ipAddress;

      /*! Hardware address */
      std::string _macAddress;

      /*! Interface broadcast address */
      std::string _broadcastAddress;

      /*! Interface netmask */
      std::string _netmask;

      /*! Interface MTU */
      int _mtu;

      /*! Interface metric */
      int _metric;

      /*! Interface is local loopback */
      bool _localhost;
      // Attributes -

    public:
      // Constructor +
      /*!
       * \ref NetworkInterface constructor.
       *
       * \param interface data retrieved through \c ioctl POSIX call
       * \param isLocalhost interface is local loopback
       *
       * \throw anch::device::DeviceException Device error
       */
      NetworkInterface(const struct ifreq& interface, bool isLocalhost);
      // Constructor -

      // Destructor +
      /*!
       * \ref NetworkInterface destructor
       */
      virtual ~NetworkInterface();
      // Destructor -

    public:
      // Accessors +
      /*!
       * Interface name getter
       *
       * \return The interface name
       */
      inline const std::string& getName() const;

      /*!
       * Interface IP address getter
       *
       * \return The interface IP address
       */
      inline const std::string& getIpAddress() const;

      /*!
       * MAC address getter
       *
       * \return The MAC address
       */
      inline const std::string& getMacAddress() const;

      /*!
       * Interface broadcast address getter
       *
       * \return The interface broadcast address
       */
      inline const std::string& getBroadcastAddress() const;

      /*!
       * Interface netmask getter
       *
       * \return The interface netmask
       */
      inline const std::string& getNetmask() const;

      /*!
       * Interface MTU getter
       *
       * \return The interface MTU
       */
      inline int getMtu() const;

      /*!
       * Interface metric getter
       *
       * \return The interface metric
       */
      inline int getMetric() const;

      /*!
       * Is interface localhost getter
       *
       * \return \c true if interface is localhost, \c false otherwise
       */
      inline bool isLocalhost() const;
      // Accessors -

    };

    inline const std::string& NetworkInterface::getName() const {
      return _name;
    }

    inline const std::string& NetworkInterface::getIpAddress() const {
      return _ipAddress;
    }

    inline const std::string& NetworkInterface::getMacAddress() const {
      return _macAddress;
    }

    inline const std::string& NetworkInterface::getBroadcastAddress() const {
      return _broadcastAddress;
    }

    inline const std::string& NetworkInterface::getNetmask() const {
      return _netmask;
    }

    inline int NetworkInterface::getMtu() const {
      return _mtu;
    }

    inline int NetworkInterface::getMetric() const {
      return _metric;
    }

    inline bool NetworkInterface::isLocalhost() const {
      return _localhost;
    }

  }
}
