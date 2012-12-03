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
#ifndef _ANCH_FILE_FILE_H_
#define _ANCH_FILE_FILE_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <memory>

#include "file/fileException.hpp"
#include "date/date.hpp"


namespace anch {
  namespace file {

    /**
     * File class utility
     *
     * @author Vincent Lachenal
     */
    class File {
      // Attributes +
    public:
      /** {@link File} separator character */
      static const char SEP;

    private:
      /** The {@link File} path */
      std::string _path;

      /** The parent {@link File} */
      std::shared_ptr<File> _parent;

      /** Is {@link File} exist */
      bool _exists = false;

      /** Is {@link File} a directory */
      bool _directory = false;

      /** Is {@link File} readable */
      bool _readable = false;

      /** Is {@link File} writable */
      bool _writable = false;

      /** Is {@link File} executable */
      bool _executable = false;

      /** The {@link File} size */
      uint64_t _size;

      /** {@link File} last access */
      anch::date::Date _lastAccess;

      /** {@link File} last modification */
      anch::date::Date _lastModification;

      /** {@link File} last status change */
      anch::date::Date _lastStatusChange;
      // Attributes -

      // Constructors +
    public:
      /**
       * {@link File} constructor
       *
       * @param path The {@link File} path
       * @param init Initialize file members
       */
      File(const std::string& path, bool init = true);

      /**
       * {@link File} constructor
       *
       * @param parent The parent {@link File} path
       * @param name The {@link File} name
       */
      File(const std::string& parent, const std::string& name);

      /**
       * {@link File} constructor
       *
       * @param parent The parent {@link File}
       * @param name The {@link File} name
       */
      File(const File& parent, const std::string& name);

      /**
       * {@link File} constructor
       *
       * @param parent The parent {@link File}
       * @param name The {@link File} name
       */
      File(std::shared_ptr<File> parent, const std::string& name);

      /**
       * {@link File} copy constructor
       *
       * @param file {@link File} to copy
       */
      File(const File& file);
      // Constructors -

      // Destructor +
    public:
      /**
       * {@link File} destructor
       */
      virtual ~File();
      // Destructor -

      // Methods +
    public:
      /**
       * Create a new file
       */
      void createFile() throw(anch::file::FileException);

      /**
       * Create a new file
       *
       * @param out The output stream on the file
       */
      void createFile(std::ofstream& out) throw(anch::file::FileException);

      /**
       * Create directory
       *
       * @param parents Create parent directories if needed
       */
      void createDirectory(bool parents = false) throw(anch::file::FileException);

      /**
       * Delete this {@link File}
       */
      void deleteFile() throw(anch::file::FileException);

      /**
       * List files and directories in current directory.
       *
       * @return The files list
       */
      void list(std::vector<std::string>& files) throw(anch::file::FileException);

      /**
       * List files and directories in current directory.
       *
       * @return The {@link File} list
       */
      void list(std::vector<File>& files) throw(anch::file::FileException);

    private:
      /**
       * Initialize {@link File} according to its path
       */
      void initialize();
      // Methods -

      // Accessors +
    public:
      /**
       * Get the {@link File} path
       *
       * @return The {@link File} path
       */
      inline const std::string& getPath() const {
	return _path;
      }

      /**
       * Get the parent {@link File}
       *
       * @return The parent {@link File}
       */
      inline std::shared_ptr<File> getParent() const {
	return _parent;
      }

      /**
       * Check if {@link File} exist.
       *
       * @return <code>true</code> if {@link File} exists, <code>false</code> otherwise.
       */
      inline bool exists() const {
	return _exists;
      }

      /**
       * Check is {@link File} is a directory.
       *
       * @return <code>true</code> if {@link File} is a directory, <code>false</code> otherwise.
       */
      inline bool isDirectory() const {
	return _directory;
      }


      /**
       * Check is {@link File} is a regular file.
       *
       * @return <code>true</code> if {@link File} is a regular file, <code>false</code> otherwise.
       */
      inline bool isFile() const {
	return !_directory;
      }

      /**
       * Return the readable state
       *
       * @return <code>true</code> is the {@link File} can be read, <code>flase</code> otherwise.
       */
      inline bool canRead() const {
	return _readable;
      }

      /**
       * Return the writable state
       *
       * @return <code>true</code> is the {@link File} can be written, <code>flase</code> otherwise.
       */
      inline bool canWrite() const {
	return _writable;
      }

      /**
       * Return the executable state
       *
       * @return <code>true</code> is the {@link File} can be executed, <code>flase</code> otherwise.
       */
      inline bool canExecute() const {
	return _executable;
      }

      /**
       * {@link File} size getter
       *
       * @return The {@link File} size
       */
      inline uint64_t getSize() const {
	return _size;
      }

      /**
       * {@link File} last access getter
       *
       * @return The {@link File} last access
       */
      inline const anch::date::Date& getLastAccess() const {
	return _lastAccess;
      }

      /**
       * {@link File} last modification getter
       *
       * @return The {@link File} last modification
       */
      inline const anch::date::Date& getLastModification() const {
	return _lastModification;
      }

      /**
       * {@link File} last status change getter
       *
       * @return The {@link File} last status change
       */
      inline const anch::date::Date& getLastStatusChange() const {
	return _lastStatusChange;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_FILE_FILE_H_
