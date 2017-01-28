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
using std::shared_ptr;
using std::make_shared;

using anch::file::File;
using anch::date::Date;


// Class static members initialization +
#ifdef ANCH_POSIX
const char File::SEP = '/';
#elif defined ANCH_WINDOWS
const char File::SEP = '\\';
#endif
// Class static members initialization -


// Constructors +
File::File(const string& path, bool init) : _path(path) {
  if(init) {
    initialize();
  }
  string parentPath;
  size_t pos = path.find_last_of(SEP);
  if(pos == string::npos && init) {
#ifdef ANCH_POSIX
    char* parent_path = ::get_current_dir_name();
#elif defined ANCH_WINDOWS
    // \todo use Windows implementation
#endif
    if(parent_path != nullptr) {
      parentPath = parent_path;
      delete parent_path;
    }

  } else {
    parentPath = path.substr(0,pos);
  }
  if(parentPath != "" && parentPath != _path) {
    _parent = make_shared<File>(parentPath, false);
  } else {
    _parent = shared_ptr<File>(nullptr);
  }
}

File::File(const string& parent, const string& name) : _path(parent + SEP + name) {
  initialize();
  _parent = make_shared<File>(parent, false);
}

File::File(const File& parent, const string& name) : _path(parent._path + SEP + name) {
  initialize();
  _parent = make_shared<File>(parent);
}

File::File(std::shared_ptr<File> parent, const string& name) : _path(parent->_path + SEP + name) {
  initialize();
  _parent = shared_ptr<File>(parent);
}

File::File(const File& file) : _path(file._path),
			       _exists(file._exists),
			       _directory(file._directory),
			       _readable(file._readable),
			       _writable(file._writable),
			       _executable(file._executable),
			       _size(file._size),
			       _lastAccess(file._lastAccess),
			       _lastModification(file._lastModification),
			       _lastStatusChange(file._lastStatusChange) {
  _parent = shared_ptr<File>(file._parent);
}
// Constructors -


// Destructor +
File::~File() {
  // Nothing to do
}
// Destructor -


// Methods +
void
File::createFile() throw(FileException) {
  ofstream out;
  createFile(out);
  out.close();
}

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

void
File::createDirectory(bool/* parents*/) throw(FileException) {
  // \todo create directory recursively according to input parameter
  if(!_exists && _parent->isDirectory()) {
    int status = ::mkdir(_path.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status == -1) {
      throw FileException(string("Can not create directory") + _path, errno);
    }

  } else {
    throw FileException(_path + " already exists or its parent is not a directory");
  }
}

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

void
File::list(vector<string>& files) throw(FileException) {
  if(_directory && _readable) {
    DIR* dir;
    struct dirent* entry;
    if((dir = opendir(_path.data())) == nullptr) {
      throw FileException(string("Can not open directory ") + _path, errno);
    }
    while((entry = ::readdir(dir)) != nullptr) {
      string fileName = entry->d_name;
      if(fileName != "." && fileName != "..") {
	files.push_back(fileName);
      }
    }
    files.shrink_to_fit();
    ::closedir(dir);

  } else {
    throw FileException(_path + " is not a directory or is not readable.");
  }
}

void
File::list(vector<File>& files) throw(FileException) {
  vector<string> res;
  list(res);
  shared_ptr<File> copy = make_shared<File>(*this);
  for(const string& path : res) {
    files.push_back(File(copy, path));
  }
  files.shrink_to_fit();
}

void
File::initialize() {
  _path.shrink_to_fit();
  if(::access(_path.data(), F_OK) == 0) { // Test if file exists
    _exists = true;
    struct stat file;
    int res = ::stat(_path.data(), &file);
    if(res >= 0) {
      _directory = S_ISDIR(file.st_mode);
      _size = static_cast<uint64_t>(file.st_size);
      _lastAccess = Date(file.st_atime);
      _lastModification = Date(file.st_mtime);
      _lastStatusChange = Date(file.st_ctime);
      _readable = (::access(_path.data(), R_OK) == 0);
      _writable = (::access(_path.data(), W_OK) == 0);
      _executable = (::access(_path.data(), X_OK) == 0);
    }
  }
}
// Methods -
