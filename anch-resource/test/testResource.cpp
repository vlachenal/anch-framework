#include <iostream>

#include "resource/resource.hpp"


using std::cout;
using std::endl;
using std::string;

using anch::resource::Resource;


int
main(void) {
  cout << "Enter in test resource" << endl;

  cout << "Parse file" << endl;
  const Resource& res = Resource::getResource("../test/test.ini");
  cout << "File has been parsed" << endl;

  string value;
  bool found = res.getParameter(value,"toto");
  if(found) {
    cout << "toto=" << value << endl;
  } else {
    cout << "toto has not been found" << endl;
  }

  found = res.getParameter(value,"toto","TOTO");
  if(found) {
    cout << "TOTO/toto=" << value << endl;
  } else {
    cout << "TOTO/toto has not been found" << endl;
  }

  found = res.getParameter(value,"tata");
  if(found) {
    cout << "tata=" << value << endl;
  } else {
    cout << "tata has not been found" << endl;
  }


  cout << "Exit test resource" << endl;
  return 0;
}
