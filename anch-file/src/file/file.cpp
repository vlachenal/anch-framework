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
#include "file/file.hpp"

#ifdef ANCH_POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#endif

using std::string;
using std::vector;
using std::ofstream;

using anch::file::File;


// Class static members initialization +
#ifdef ANCH_POSIX
const char File::SEP = '/';
#elif defined ANCH_WINDOWS
const char File::SEP = '\\';
#endif
// Class static members initialization -


// Constructors +
/**
 * {@link File} constructor
 *
 * @param path The {@link File} path
 */
File::File(const string& path) : _path(path) {
  initialize();
  string parentPath;
  size_t pos = path.find_last_of(SEP);
  if(pos == string::npos) {
#ifdef ANCH_POSIX
    char* parent_path = ::get_current_dir_name();
#elif defined ANCH_WINDOWS
    // TODO get Windowss implementation
#endif
    if(parent_path == nullptr) {
      parentPath = ".";
    } else {
      parentPath = parent_path;
      delete parent_path;
    }

  } else {
    parentPath = path.substr(0,pos);
  }
  _parent = new File(parentPath);
}

/**
 * {@link File} constructor
 *
 * @param parent The parent {@link File} path
 * @param name The {@link File} name
 */
File::File(const string& parent, const string& name) : _path(parent + SEP + name) {
  initialize();
  _parent = new File(parent);
}

/**
 * {@link File} constructor
 *
 * @param parent The parent {@link File}
 * @param name The {@link File} name
 */
File::File(const File& parent, const string& name) : _path(parent.getPath() + SEP + name) {
  initialize();
  _parent = new File(parent);
}

/**
 * {@link File} copy constructor
 *
 * @param file {@link File} to copy
 */
File::File(const File& file) : _path(file._path),
			       _parent(file._parent),
			       _exists(file._exists),
			       _directory(file._directory),
			       _readable(file._readable),
			       _writable(file._writable),
			       _executable(file._executable) {
  // Nothing to do mode
}
// Constructors -

// Destructor +
/**
 * {@link File} destructor
 */
File::~File() {
  delete _parent;
}
// Destructor -

// Methods +
/**
 * Create a new file
 */
void
File::createFile() throw(FileException) {
  ofstream out;
  createFile(out);
  out.close();
}

/**
 * Create a new file
 *
 * @param out The output stream on the file
 */
void
File::createFile(ofstream& out) throw(FileException) {
  if(!_exists) {
    if(out.is_open()) {
      out.close();
    }
    out.open(_path);
    if(out.fail()) {
      throw FileException(string("Can not create file ") + _path);
    }

  } else {
    throw FileException(string("File ") + _path + "already exists.");
  }
}

/**
 * Create directory
 *
 * @param parents Create parent directories if needed
 */
void
File::createDirectory(bool parents) throw(FileException) {
  if(!_exists && _parent->isDirectory()) {
    int status = mkdir(_path.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status == -1) {
      throw FileException(string("Can not create directory") + _path, errno);
    }

  } else {
    throw FileException(_path + " already exists or its parent is not a directory");
  }
}

/**
 * Delete this {@link File}
 */
void
File::deleteFile() throw(FileException) {
  if(_exists && !_directory && _writable) {
    int res = ::unlink(_path.data());
    if(res == -1) {
      throw FileException(string("Can not delete ") + _path.data(), errno);
    }

  } else {
      throw FileException(_path + " does not exist or is not writable or is a directory");
  }
}

/**
 * List files and directories in current directory.
 *
 * @return The files list
 */
vector<string>
File::list() throw(FileException) {
  if(_directory && _readable) {
    DIR* dir;
    struct dirent* entry;
    if((dir = opendir(_path.data())) == nullptr) {
      throw FileException(string("Can not open directory ") + _path, errno);
    }
    vector<string> files;
    while((entry = ::readdir(dir)) != nullptr) {
      files.push_back(entry->d_name);
    }
    files.shrink_to_fit();
    ::closedir(dir);
    return files;

  } else {
    throw FileException(_path + " is not a directory or is not readable.");
  }
}

/**
 * List files and directories in current directory.
 *
 * @return The {@link File} list
 */
vector<File>
File::listFiles() throw(FileException) {
  vector<string> res = list();
  vector<File> files;
  for(size_t i = 0 ; i < res.size() ; i++) {
    files.push_back(File(res.at(i)));
  }
  files.shrink_to_fit();
  return files;
}

/**
 * Initialize file information
 */
void
File::initialize() {
  _path.shrink_to_fit();
  if(::access(_path.data(), F_OK) == 0) { // Test if file exists
    _exists = true;
    struct stat file;
    int res = ::stat(_path.data(), &file);
    if(res >= 0) {
      _directory = S_ISDIR(file.st_mode);
      _size = file.st_size;
      _lastAccess = file.st_atime;
      _lastModification = file.st_mtime;
      _lastStatusChange =file.st_ctime;
      _readable = (::access(_path.data(), R_OK) == 0);
      _writable = (::access(_path.data(), W_OK) == 0);
      _executable = (::access(_path.data(), X_OK) == 0);
    }
  }
}
// Methods -
