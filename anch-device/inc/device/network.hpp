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
#include <map>
#include <mutex>

#include "device/networkInterface.hpp"
#include "device/deviceException.hpp"


namespace anch {
  namespace device {

    /*!
     * Network device utilities class
     *
     * \author Vincent Lachenal
     */
    class Network {

    private:
      // Attributes +
      /*! Network interfaces indexed by their names */
      static std::map<std::string,anch::device::NetworkInterface>* _interfaces;

      /*! Mutex */
      static std::mutex MUTEX;
      // Attributes -

    private:
      // Constructors +
      /*!
       * Network configuration private constructor
       */
      Network();
      // Constructors -

    public:
      // Destructors +
      /*!
       * Network configuration destructor
       */
      virtual ~Network();
      // Destructors -

    public:
      // Methods +
      /*!
       * Retrieve network interface by its name.
       *
       * \param ifName the interface name
       *
       * \return the interface if found, \c NULL otherwise
       *
       * \throw anch::device::DeviceException Network interfaces error
       */
      static const anch::device::NetworkInterface* getInterface(const std::string& ifName);

      /*!
       * Retrieve all network interfaces.
       *
       * \return the network interfaces
       *
       * \throw anch::device::DeviceException Network interfaces error
       */
      static const std::map<std::string,anch::device::NetworkInterface>& getInterfaces();

      /*!
       * Reload network interfaces
       *
       * \throw anch::device::DeviceException Network interfaces error
       */
      static void reload();

    private:
      /*!
       * Load network interfaces
       *
       * \throw anch::device::DeviceException Network interfaces error
       */
      static void load();
      // Methods -

    };

  }
}
