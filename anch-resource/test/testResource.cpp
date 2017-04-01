#include <iostream>

#include "resource/resource.hpp"


using anch::resource::Resource;


int
main(void) {
  std::cout << "Enter in test resource" << std::endl;

  std::cout << "Parse file" << std::endl;
  const Resource& res = Resource::getResource("../test/test.ini");
  std::cout << "File has been parsed" << std::endl;

  std::string value;
  bool found = res.getParameter(value,"toto");
  if(found) {
    std::cout << "toto=" << value << std::endl;
  } else {
    std::cout << "toto has not been found" << std::endl;
  }

  found = res.getParameter(value,"toto","TOTO");
  if(found) {
    std::cout << "TOTO/toto=" << value << std::endl;
  } else {
    std::cout << "TOTO/toto has not been found" << std::endl;
  }

  found = res.getParameter(value,"tata");
  if(found) {
    std::cout << "tata=" << value << std::endl;
  } else {
    std::cout << "tata has not been found" << std::endl;
  }


  std::cout << "Exit test resource" << std::endl;
  return 0;
}
