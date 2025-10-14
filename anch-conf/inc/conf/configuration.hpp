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

#include <map>
#include <optional>
#include <string>
#include <vector>
#include <filesystem>

#include "conf/section.hpp"
#include "conf/confError.hpp"

namespace anch::conf {

  /*!
   * \brief Application configuration
   *
   * Load and manage configuration for an application.\n
   * It provides profiles inspired by Spring framework.
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  class Configuration {

    // Attributes +
  private:
    /*! Configuration has been loaded flag */
    bool _loaded;

    /*! Configuration file base name without profile suffix and without extension */
    std::string _name;

    /*! Profiles' list. Default: ANCH_PROFILES environment variable */
    std::vector<std::string> _profiles;

    /*! Files to include */
    std::vector<std::string> _includes;

    /*! Files to include */
    std::vector<std::string> _folders;

    /*! Root section */
    anch::conf::Section _root;
    // Attributes -

    // Constructors +
  private:
    /*!
     * \ref Configuration private constructor
     */
    Configuration();

  public:
    /*!
     * Forbids \ref Configuration copy constructor
     *
     * \param conf the \ref Configuration not to copy
     */
    Configuration(const Configuration& conf) = delete;

    /*!
     * Forbids \ref Configuration move constructor
     *
     * \param conf the \ref Configuration not to move
     */
    Configuration(Configuration&& conf) = delete;
    // Constructors -

    // Destructor +
  private:
    /*!
     * \ref Configuration destructor
     */
    virtual ~Configuration();
    // Destructor -

    // Methods +
  public:
    /*!
     * Get \ref Configuration unique instance
     *
     * \return the reference of the \ref Configuration
     */
    static const Configuration& inst();

    /*!
     * Initialize \ref Configuration
     *
     * \return the reference of the new \ref Configuration
     */
    static Configuration& loader() noexcept;

    /*!
     * Folders' setter
     *
     * \param folders the folders to inspect
     *
     * \return \c this
     */
    Configuration& folders(const std::vector<std::filesystem::path>& folders) noexcept;

    /*!
     * Configuration base name setter
     *
     * \param name the name to set
     *
     * \return \c this
     */
    Configuration& name(const std::string& name) noexcept;

    /*!
     * Configuration profiles setter
     *
     * \param profiles the profiles to set
     *
     * \return \c this
     */
    Configuration& profiles(const std::vector<std::string>& profiles) noexcept;

    /*!
     * Parse \ref Configuration from base name.\n
     * Configuration will load configuration files in this order:
     *  - look for <name> file with registered extenions (default name to application ; ini, conf, cnf and properties extensions will always been registered) in folders
     *  - raise error when file is not found
     *  - load the first file found (others will be ignored)
     *  - load includes files
     *  - for each active profiles, repeat the previous loading with <file>-<profile>.<extension>
     *
     * \param filePath The resource file path
     *
     * \return the \ref Configuration
     */
    Configuration& load();

    /*!
     * Get section
     *
     * \param section the section's path
     *
     * \return the section when found, \c NULL otherwise.
     */
    const anch::conf::Section* section(const std::string& path) const noexcept;

    /*!
     * Get value
     *
     * \param param the value's path
     *
     * \return the optional result
     */
    std::optional<std::string> value(const std::string& path) const noexcept;

  private:
    /*!
     * Load profile's configuration files.\n
     * Do not raise error when configuration has not been found.\n
     * This will override any previously defined value.
     *
     * \param profile the profile to load
     */
    void loadProfile(const std::string& profile);
    // Methods -

  };

}

#include "conf/impl/configuration.hpp"
