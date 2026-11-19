/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "anch-device-export.hpp"

#include <string>
#include <vector>


namespace anch::device {

  class Network;

  /*!
   * \brief Network interface
   *
   * Snapshot of a network interface configuration built by \ref Network . Every information is optional: an empty
   * string (or \c 0 ) means that the interface does not have it (no IPv4 address, no broadcast address on a
   * point-to-point interface, no hardware address on a tunnel, ...).
   *
   * \author Vincent Lachenal
   */
  class ANCH_DEVICE_EXPORT NetworkInterface {
    friend class anch::device::Network;

  private:
    // Attributes +
    /*! Interface name */
    std::string _name;

    /*! Interface (first) IPv4 address */
    std::string _ipAddress;

    /*! Interface IPv6 addresses */
    std::vector<std::string> _ipv6Addresses;

    /*! Hardware address */
    std::string _macAddress;

    /*! Interface broadcast address */
    std::string _broadcastAddress;

    /*! Interface IPv4 netmask */
    std::string _netmask;

    /*! Interface MTU */
    int _mtu;

    /*! Interface metric */
    int _metric;

    /*! Interface is up */
    bool _up;

    /*! Interface is local loopback */
    bool _localhost;
    // Attributes -

  private:
    // Constructors +
    /*!
     * \ref NetworkInterface constructor: interface without any information.
     *
     * \param name the interface name
     */
    explicit NetworkInterface(const std::string& name);
    // Constructors -

  public:
    // Accessors +
    /*!
     * Interface name getter
     *
     * \return The interface name
     */
    inline const std::string& getName() const;

    /*!
     * Interface IPv4 address getter. When the interface has several IPv4 addresses, the first one is returned.
     *
     * \return The interface IPv4 address, empty if none
     */
    inline const std::string& getIpAddress() const;

    /*!
     * Interface IPv6 addresses getter. Link-local addresses have their scope ( \c %\<interface\> ).
     *
     * \return The interface IPv6 addresses
     *
     * \since 0.2
     */
    inline const std::vector<std::string>& getIpv6Addresses() const;

    /*!
     * MAC address getter
     *
     * \return The hardware address (hexadecimal bytes separated by \c : ), empty if none
     */
    inline const std::string& getMacAddress() const;

    /*!
     * Interface broadcast address getter
     *
     * \return The interface IPv4 broadcast address, empty if none
     */
    inline const std::string& getBroadcastAddress() const;

    /*!
     * Interface netmask getter
     *
     * \return The interface IPv4 netmask, empty if none
     */
    inline const std::string& getNetmask() const;

    /*!
     * Interface MTU getter
     *
     * \return The interface MTU, \c 0 if unknown
     */
    inline int getMtu() const;

    /*!
     * Interface metric getter (Linux always reports \c 0 )
     *
     * \return The interface metric
     */
    inline int getMetric() const;

    /*!
     * Is interface up getter
     *
     * \return \c true if interface is up, \c false otherwise
     *
     * \since 0.2
     */
    inline bool isUp() const;

    /*!
     * Is interface localhost getter
     *
     * \return \c true if interface is local loopback, \c false otherwise
     */
    inline bool isLocalhost() const;
    // Accessors -

  };

}  // anch::device

#include "device/impl/networkInterface.hpp"
