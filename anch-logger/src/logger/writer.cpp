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


/**
 * {@link Writer} constructor
 *
 * @param fileName The file name
 * @param linePattern The line pattern
 * @param maxSize The file maximum size before file rotation
 * @param maxIndex The maximum number of log files to keep
 */
Writer::Writer(const string& fileName,
	       const string& linePattern,
	       int maxSize,
	       int maxIndex):  _fileName(fileName),
			       _maxSize(maxSize),
			       _maxIndex(maxIndex),
			       _fileIndex(0),
			       _mutex(),
			       _formatter(linePattern) {
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
    _output = (ostream*)(&cout);
    _fileName = "";
    _maxSize = 0;
    _maxIndex = 0;
  }
}

/**
 * {@link Writer} constructor
 *
 * @param output The output to use
 * @param linePattern The line pattern
 */
Writer::Writer(ostream* output,
	       const string& linePattern): _output(output),
					   _fileName(""),
					   _maxSize(0),
					   _maxIndex(0),
					   _fileIndex(0),
					   _mutex(),
					   _formatter(linePattern) {
  // Nothing to do
}

/**
 * {@link Writer} destructor
 */
Writer::~Writer() {
  _output->flush();
  if(_fileName != "") {
    ((ofstream*)_output)->close();
  }
}

/**
 * Write message in the file
 *
 * @param category The logger category
 * @param level The message level
 * @param message Message to write
 */
void
Writer::write(const string& category,
	      const Level& level,
	      const string& message) {
  _mutex.lock();
  string line;
  *_output << _formatter.formatMessage(category, level, message) << "\n";
  if(_maxSize > 0 && _output->tellp() >= _maxSize) {
    rotateFiles();
  }
  _mutex.unlock();
}

/**
 * Rotate files when current reachs the maximum file length.
 */
void
Writer::rotateFiles() {
  _output->flush();
  ((ofstream*)_output)->close();
  delete _output;
  ostringstream ostr;
  // Remove older file if max index file has been reached +
  if(_fileIndex == _maxIndex) {
    ostr << _fileName << "." << _fileIndex;
    remove(ostr.str().c_str());
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
    rename(oldName.c_str(), newName.c_str());
  }
  rename(_fileName.c_str(), string(_fileName + ".1").c_str());
  // Rename every log files -
  _output = new ofstream(_fileName);
  _fileIndex++;
}
