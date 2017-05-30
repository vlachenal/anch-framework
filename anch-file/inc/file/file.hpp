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

    /*!
     * File class utility
     *
     * \author Vincent Lachenal
     */
    class File {
      // Attributes +
    public:
      /*! \ref File separator character */
      static const char SEP;

    private:
      /*! The \ref File path */
      std::string _path;

      /*! The parent \ref File */
      std::shared_ptr<File> _parent;

      /*! Is \ref File exist */
      bool _exists = false;

      /*! Is \ref File a directory */
      bool _directory = false;

      /*! Is \ref File readable */
      bool _readable = false;

      /*! Is \ref File writable */
      bool _writable = false;

      /*! Is \ref File executable */
      bool _executable = false;

      /*! The \ref File size */
      uint64_t _size;

      /*! \ref File last access */
      anch::date::Date _lastAccess;

      /*! \ref File last modification */
      anch::date::Date _lastModification;

      /*! \ref File last status change */
      anch::date::Date _lastStatusChange;
      // Attributes -

      // Constructors +
    public:
      /*!
       * \ref File constructor
       *
       * \param path The \ref File path
       * \param init Initialize file members
       */
      File(const std::string& path, bool init = true);

      /*!
       * \ref File constructor
       *
       * \param parent The parent \ref File path
       * \param name The \ref File name
       */
      File(const std::string& parent, const std::string& name);

      /*!
       * \ref File constructor
       *
       * \param parent The parent \ref File
       * \param name The \ref File name
       */
      File(const File& parent, const std::string& name);

      /*!
       * \ref File constructor
       *
       * \param parent The parent \ref File
       * \param name The \ref File name
       */
      File(std::shared_ptr<File> parent, const std::string& name);

      /*!
       * \ref File copy constructor
       *
       * \param file \ref File to copy
       */
      File(const File& file);
      // Constructors -

      // Destructor +
    public:
      /*!
       * \ref File destructor
       */
      virtual ~File();
      // Destructor -

      // Methods +
    public:
      /*!
       * Create a new file
       *
       * \throw anch::file::FileException any error on file creation
       */
      void createFile();

      /*!
       * Create a new file
       *
       * \param out The output stream on the file
       *
       * \throw anch::file::FileException any error on file creation
       */
      void createFile(std::ofstream& out);

      /*!
       * Create directory
       *
       * \param parents Create parent directories if needed
       *
       * \throw anch::file::FileException any error on directory creation
       */
      void createDirectory(bool parents = false);

      /*!
       * Delete this \ref File
       *
       * \throw anch::file::FileException any error on file deletion
       */
      void deleteFile();

      /*!
       * List files and directories in current directory.
       *
       * \param files the files list
       *
       * \throw anch::file::FileException any error on directory list
       */
      void list(std::vector<std::string>& files);

      /*!
       * List files and directories in current directory.
       *
       * \param files The \ref File list
       *
       * \throw anch::file::FileException any error on directory list
       */
      void list(std::vector<File>& files);

    private:
      /*!
       * Initialize \ref File according to its path
       */
      void initialize();
      // Methods -

      // Accessors +
    public:
      /*!
       * Get the \ref File path
       *
       * \return the \ref File path
       */
      inline const std::string& getPath() const {
	return _path;
      }

      /*!
       * Get the parent \ref File
       *
       * \return the parent \ref File
       */
      inline std::shared_ptr<File> getParent() const {
	return _parent;
      }

      /*!
       * Check if \ref File exist.
       *
       * \return \c true if \ref File exists, \c false otherwise.
       */
      inline bool exists() const {
	return _exists;
      }

      /*!
       * Check is \ref File is a directory.
       *
       * \return \c true if \ref File is a directory, \c false otherwise.
       */
      inline bool isDirectory() const {
	return _directory;
      }

      /*!
       * Check is \ref File is a regular file.
       *
       * \return \c true if \ref File is a regular file, \c false otherwise.
       */
      inline bool isFile() const {
	return !_directory;
      }

      /*!
       * Return the readable state
       *
       * \return \c true is the \ref File can be read, \c false otherwise.
       */
      inline bool canRead() const {
	return _readable;
      }

      /*!
       * Return the writable state
       *
       * \return \c true is the \ref File can be written, \c false otherwise.
       */
      inline bool canWrite() const {
	return _writable;
      }

      /*!
       * Return the executable state
       *
       * \return \c true is the \ref File can be executed, \c false otherwise.
       */
      inline bool canExecute() const {
	return _executable;
      }

      /*!
       * \ref File size getter
       *
       * \return the \ref File size
       */
      inline uint64_t getSize() const {
	return _size;
      }

      /*!
       * \ref File last access getter
       *
       * \return the \ref File last access
       */
      inline const anch::date::Date& getLastAccess() const {
	return _lastAccess;
      }

      /*!
       * \ref File last modification getter
       *
       * \return the \ref File last modification
       */
      inline const anch::date::Date& getLastModification() const {
	return _lastModification;
      }

      /*!
       * \ref File last status change getter
       *
       * \return the \ref File last status change
       */
      inline const anch::date::Date& getLastStatusChange() const {
	return _lastStatusChange;
      }
      // Accessors -

    };

  }
}

#endif // _ANCH_FILE_FILE_H_
