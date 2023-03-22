/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2020 Vincent Lachenal

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

#include <string>
#include <functional>
#include <map>

namespace anch::ut {

  /*!
   * \brief Unit tests collection
   *
   * This class aims to provide (un)intialization functions (either global and per test) and unit tests registration
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class UnitTests {

    // Attrbutes +
  private:
    /*! Tests collection's name */
    std::string _name;

    /*! Tests collection's description */
    std::string _description;

    /*! Unit tests initialization function (default to nothing) */
    std::function<void(void)> _initFunc;

    /*! Unit tests uninitialization function (default to nothing) */
    std::function<void(void)> _uninitFunc;

    /*! Unit test initialization function (default to nothing) */
    std::function<void(void)> _beforeFunc;

    /*! Unit test uninitialization function (default to nothing) */
    std::function<void(void)> _afterFunc;

    /*! Unit tests collection */
    std::map<std::string, std::function<void(void)>> _tests;
    // Attrbutes -

    // Constructors +
  public:
    /*!
     * \ref UnitTests default construtor
     */
    UnitTests();
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref UnitTests destructor
     */
    virtual ~UnitTests();
    // Destructor -

    // Methods +
  public:
    /*!
     * Unit tests colletion name setter.\n
     * This field is used for reporting usage.
     *
     * \param name the name to set
     *
     * \return \c this
     */
    UnitTests& name(const std::string& name);

    /*!
     * Unit tests colletion description setter\n
     * This field is used for reporting usage.
     *
     * \param description the description to set
     *
     * \return \c this
     */
    UnitTests& description(const std::string& description);

    /*!
     * Unit tests colletion initialization function setter
     *
     * \param initFunc the function to set
     *
     * \return \c this
     */
    UnitTests& initialize(std::function<void(void)> initFunc);

    /*!
     * Unit tests colletion uninitialization function setter
     *
     * \param uninitFunc the function to set
     *
     * \return \c this
     */
    UnitTests& uninitialize(std::function<void(void)> uninitFunc);

    /*!
     * Unit test initialization function setter
     *
     * \param beforeFunc the function to set
     *
     * \return \c this
     */
    UnitTests& beforeTest(std::function<void(void)> beforeFunc);

    /*!
     * Unit test uninitialization function setter
     *
     * \param afterFunc the function to set
     *
     * \return \c this
     */
    UnitTests& afterTest(std::function<void(void)> afterFunc);

    /*!
     * Unit test function setter
     *
     * \param name the unit test's name
     * \param testFunc the function to add
     *
     * \return \c this
     */
    UnitTests& add(const std::string& name, std::function<void(void)> testFunc);

    /*!
     * Call initialiation unit tests collection
     */
    void initialize() const;

    /*!
     * Call uninitialiation unit tests collection
     */
    void uninitialize() const;

    /*!
     * Call initialiation unit test
     */
    void before() const;

    /*!
     * Call uninitialiation unit test
     */
    void after() const;
    // Methods -

    // Accessors +
    /*!
     * Unit tests collection's name getter
     *
     * \return the name
     */
    const std::string& getName() const;

    /*!
     * Unit tests collection's description getter
     *
     * \return the description
     */
    const std::string& getDescription() const;

    /*!
     * Unit tests gettter
     *
     * \return the tests
     */
    const std::map<std::string, std::function<void(void)>>& getTests() const;

    /*!
     * Unit tests setters
     *
     * \param tests the tests to set
     */
    void setTests(const std::map<std::string, std::function<void(void)>>& tests);
    // Accessors -

  };

  /*!
   * Setup unit tests collection
   *
   * \param tests the collection to fill
   */
  void setup(UnitTests& tests);

}  // anch::ut

#include "ut/impl/unit.hpp"
