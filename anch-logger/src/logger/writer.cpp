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
#include <fstream>
#include <sstream>

#include <stdio.h>

#include "logger/writer.hpp"


using std::string;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::ios_base;
using std::ostringstream;
using std::cerr;
using std::endl;
using std::cout;

using anch::logger::Writer;
using anch::logger::Level;
using anch::logger::formatter::MessageFormatter;


Writer::Writer(const string& fileName,
	       const string& linePattern,
	       unsigned int maxSize,
	       int maxIndex):  _formatter(linePattern),
			       _fileName(fileName),
			       _maxSize(maxSize),
			       _maxIndex(maxIndex),
			       _fileIndex(0) {
  try {
    _output = new ofstream(fileName,ios_base::app);
    // Retrieve current file index +
    for(int i = 1 ; i < _maxIndex ; i++) {
      ostringstream ostr;
      ostr << fileName << '.' << i;
      ifstream file(ostr.str());
      if(file) {
	_fileIndex = i;
	file.close();
      } else {
	break;
      }
    }
    // Retrieve current file index -

  } catch(const std::exception& e) {
    cerr << "Unable to open file. Use standard output." << endl;
    _output = static_cast<ostream*>(&cout);
    _fileName = "";
    _maxSize = 0;
    _maxIndex = 0;
  }
}

Writer::Writer(ostream* output,
	       const string& linePattern): _output(output),
					   _formatter(linePattern),
					   _fileName(""),
					   _maxSize(0),
					   _maxIndex(0),
					   _fileIndex(0) {
  // Nothing to do
}

Writer::~Writer() {
  _output->flush();
  if(_fileName != "") {
    static_cast<ofstream*>(_output)->close();
    delete _output;
  } // else do not delete cout ...
}

void
Writer::write(const string& category,
	      const Level& level,
	      const string& message) {
  *_output << _formatter.formatMessage(category, level, message) << "\n";
  if(rotate()) {
    rotateFiles();
  }
}

void
Writer::rotateFiles() {
  _output->flush();
  static_cast<ofstream*>(_output)->close();
  delete _output;
  ostringstream ostr;
  // Remove older file if max index file has been reached +
  if(_fileIndex == _maxIndex) {
    ostr << _fileName << "." << _fileIndex;
    remove(ostr.str().data());
    _fileIndex--;
  }
  // Remove older file if max index file has been reached -
  // Rename every log files +
  for(int i = _fileIndex ; i > 0 ; i--) {
    ostr.str("");
    ostr << _fileName << '.' << i;
    string oldName = ostr.str();
    ostr.str("");
    ostr << _fileName << "." << (i + 1);
    string newName = ostr.str();
    rename(oldName.data(), newName.data());
  }
  rename(_fileName.data(), string(_fileName + ".1").data());
  // Rename every log files -
  _output = new ofstream(_fileName);
  _fileIndex++;
}
