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

#include <cstdint>
#include <fstream>
#include <filesystem>

#include "log/writer.hpp"


namespace anch::log {

  /*!
   * \brief Log file writer
   *
   * Manage file writer with file rotation on size.\n
   * \n
   * Configuration:
   * - \c path: the log file path (mandatory ; its directory has to exist) ;
   * - \c max-size: maximum file size before rotation, in bytes, with optional unit \c K, \c M, \c G or \c T
   *   (powers of 1024). No rotation when not set ;
   * - \c max-rotate-files: number of archives to keep (\c \<path\>.0 is the most recent). Default to 1.
   *   With 0, the file is truncated on rotation.
   *
   * File system errors during rotation are ignored: the current file is kept and rotation is tried again later.
   *
   * \author Vincent Lachenal
   *
   * \since 0.1
   */
  class FileWriter: public Writer {

    // Attributes +
  protected:
    /*! Ouput file stream */
    std::ofstream _output;

    /*! File name */
    std::filesystem::path _path;

    /*! Maximum file size (0 for no rotation) */
    std::uint64_t _maxSize;

    /*! Number of archives to keep */
    std::uint32_t _maxFiles;
    // Attributes -

    // Constructors +
  public:
    /*!
     * \ref FileWriter constructor
     *
     * \param conf the writer's configuration
     *
     * \throw std::invalid_argument invalid configuration or file which can not be opened
     */
    FileWriter(const anch::conf::Section& conf);
    // Constructors -

    // Destructor +
  public:
    /*!
     * \ref FileWriter destructor
     */
    virtual ~FileWriter();
    // Destructor -

    // Methods +
  public:
    /*!
     * Write message in the file and flush it.\n
     * Rotate log file when needed.
     *
     * \param message Message to write
     */
    virtual void write(const std::string& message);

  protected:
    /*!
     * Write an already formatted line in the file (without flush) and rotate log file when needed.
     *
     * \param line the formatted line (without end of line)
     */
    void writeLine(const std::string& line);

    /*!
     * Check if file has to be rotate according to configuration and its size
     *
     * \return true or false
     */
    bool rotate();

    /*!
     * Rotate files when current reachs the maximum file length.
     */
    void rotateFiles();

    /*!
     * Archive path
     *
     * \param index the archive index
     *
     * \return \c \<path\>.\<index\>
     */
    std::filesystem::path archive(std::uint32_t index) const;
    // Methods -

  };

}
