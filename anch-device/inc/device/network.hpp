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

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "device/networkInterface.hpp"
#include "device/deviceException.hpp"


namespace anch::device {

  /*!
   * \brief Network interfaces
   *
   * Network interfaces are listed with \c getifaddrs : every interface is listed, with or without IP address, with its
   * IPv4 and IPv6 addresses. They are loaded on first access and kept until \ref reload .\n
   * Interfaces are returned by copy: a \ref reload does not invalidate what has been retrieved before, and every method
   * can be called concurrently.
   *
   * \author Vincent Lachenal
   */
  class ANCH_DEVICE_EXPORT Network {

  public:
    // Constructors +
    /*!
     * Utility class: no instance
     */
    Network() = delete;
    // Constructors -

  public:
    // Methods +
    /*!
     * Retrieve network interface by its name.
     *
     * \param ifName the interface name
     *
     * \return the interface if found, \c std::nullopt otherwise
     *
     * \throw anch::device::DeviceException Network interfaces error
     */
    static std::optional<anch::device::NetworkInterface> getInterface(const std::string& ifName);

    /*!
     * Retrieve all network interfaces.
     *
     * \return the network interfaces indexed by their names
     *
     * \throw anch::device::DeviceException Network interfaces error
     */
    static std::map<std::string,anch::device::NetworkInterface> getInterfaces();

    /*!
     * Reload network interfaces. On error, the previously loaded interfaces are kept.
     *
     * \throw anch::device::DeviceException Network interfaces error
     */
    static void reload();

  private:
    /*!
     * Get current network interfaces snapshot (loaded on first call)
     *
     * \return the network interfaces
     *
     * \throw anch::device::DeviceException Network interfaces error
     */
    static std::shared_ptr<const std::map<std::string,anch::device::NetworkInterface>> snapshot();

    /*!
     * Load network interfaces
     *
     * \return the network interfaces
     *
     * \throw anch::device::DeviceException Network interfaces error
     */
    static std::map<std::string,anch::device::NetworkInterface> load();
    // Methods -

  };

}  // anch::device
