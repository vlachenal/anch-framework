/*
  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _ANCH_LOGGER_WRITER_H_
#define _ANCH_LOGGER_WRITER_H_

#include <iostream>
#include <mutex>

#include "logger/levels.hpp"
#include "logger/formatter/messageFormatter.hpp"


namespace anch {
  namespace logger {

    /**
     * File writer which manage conccurency access
     *
     * @author Vincent Lachenal
     */
    class Writer {
    private:
      // Attributes +
      /** Ouput file stream */
      std::ostream* _output;

      /** File name */
      std::string _fileName;

      /** Maximum file size */
      int _maxSize;

      /** Maximum file index */
      int _maxIndex;

      /** Current file index */
      int _fileIndex;

      /** Writer mutex */
      std::mutex _mutex;

      /** Message formatter */
      anch::logger::formatter::MessageFormatter _formatter;
      // Attributes -

    public:
      // Constructors +
      /**
       * {@link Writer} constructor
       *
       * @param fileName The file name
       * @param linePattern The line pattern
       * @param maxSize The file maximum size before file rotation
       * @param maxIndex The maximum number of log files to keep
       */
      Writer(const std::string& fileName,
	     const std::string& linePattern,
	     int maxSize = 0,
	     int maxIndex = 0);

      /**
       * {@link Writer} constructor
       *
       * @param output The output to use
       * @param linePattern The line pattern
       */
      Writer(std::ostream* output, const std::string& linePattern);
      // Constructors -

      // Destructor +
      /**
       * {@link Writer} destructor
       */
      virtual ~Writer();
      // Destructor -

    public:
      /**
       * Write message in the file
       *
       * @param category The logger category
       * @param level The message level
       * @param message Message to write
       */
      void write(const std::string& category,
		 const anch::logger::Level& level,
		 const std::string& message);

    private:
      /**
       * Rotate files when current reachs the maximum file length.
       */
      void rotateFiles();

    };

  }
}

#endif // _ANCH_LOGGER_WRITER_H_
