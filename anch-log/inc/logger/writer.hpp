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

#include "logger/levels.hpp"
#include "logger/formatter/messageFormatter.hpp"


namespace anch::logger {

  /*!
   * File writer which is not thread safe
   *
   * \author Vincent Lachenal
   */
  class Writer {
    // Attributes +
  protected:
    /*! Ouput file stream */
    std::ostream* _output;

    /*! Message formatter */
    anch::logger::formatter::MessageFormatter _formatter;

  private:
    /*! File name */
    std::string _fileName;

    /*! Maximum file size */
    unsigned int _maxSize;

    /*! Maximum file index */
    int _maxIndex;

    /*! Current file index */
    int _fileIndex;
    // Attributes -

  public:
    // Constructors +
    /*!
     * \ref Writer constructor
     *
     * \param fileName The file name
     * \param linePattern The line pattern
     * \param maxSize The file maximum size before file rotation
     * \param maxIndex The maximum number of log files to keep
     */
    Writer(const std::string& fileName,
	   const std::string& linePattern,
	   unsigned int maxSize = 0,
	   int maxIndex = 0);

    /*!
     * \ref Writer constructor
     *
     * \param output The output to use
     * \param linePattern The line pattern
     */
    Writer(std::ostream* output, const std::string& linePattern);
    // Constructors -

    // Destructor +
    /*!
     * \ref Writer destructor
     */
    virtual ~Writer();
    // Destructor -

  public:
    /*!
     * Write message in the file
     *
     * \param category The logger category
     * \param level The message level
     * \param message Message to write
     */
    virtual void write(const std::string& category,
		       const anch::logger::Level& level,
		       const std::string& message);

  protected:
    /*!
     * Check if file has to be rotate according to configuration and its size
     *
     * \return true or false
     */
    bool rotate() const;

    /*!
     * Rotate files when current reachs the maximum file length.
     */
    void rotateFiles();

  };

}

#include "logger/impl/writter.hpp"
