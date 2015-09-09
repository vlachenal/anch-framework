#include <iostream>

#include "file/file.hpp"
#include "date/dateFormatter.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

using anch::file::File;
using anch::date::Date;
using anch::date::DateFormatter;


int
main(void) {
  cout << "Enter in test file" << endl;

  File home("/home/winz");
  if(!home.isDirectory()) {
    cerr << home.getPath() << " should be a directory." << endl;
    return 1;
  }
  cout << "File " << home.getPath() << " has been successfully read" << endl;
  cout << "Readable: " << (home.canRead() ? "true" : "false") << endl;

  cout << "List files in " << home.getPath() << endl;
  vector<File> files;
  home.list(files);
  DateFormatter format("%Y-%m-%d %H:%M:%S");
  cout << "Found " << files.size() << " files in " << home.getPath() << ':' << endl;
  for(const File& file : files) {
    cout << "File: " << file.getPath() << endl;
    cout << "Is regular file: " << (file.isFile() ? "true" : "false") << endl;
    cout << "Size: " << file.getSize() << " bytes" << endl;
    cout << "Readable: " << (file.canRead() ? "true" : "false") << endl;
    cout << "Writable: " << (file.canWrite() ? "true" : "false") << endl;
    cout << "Executable: " << (file.canExecute() ? "true" : "false") << endl;
    cout << "Last access: ";
    format.format(file.getLastAccess(), cout);
    cout << endl;
    cout << "Last modification: ";
    format.format(file.getLastModification(), cout);
    cout << endl;
    cout << "Last status change: ";
    format.format(file.getLastStatusChange(), cout);
    cout << endl;
  }

  cout << "Exit test file" << endl;
  return 0;
}
