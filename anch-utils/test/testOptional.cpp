#include <iostream>

#include "optional.hpp"

int
main(void) {
  std::cout << "Enter in testOptional" << std::endl;

  {
    std::cout << "Test functions optional utilities" << std::endl;
    std::optional<int> val(2);
    std::optional<int> noval;
    anch::applyIfValue<int>(val, [](const int& v) { std::cout << "Found value " << v << std::endl; });
    anch::applyIfValue<int>(noval, [](const int& v) { std::cout << "Found value " << v << std::endl; });
    anch::applyIfValueOrElse<int>(noval, [](const int& v) { std::cout << "Found value " << v << std::endl; }, []() { std::cout << "No value ..." << std::endl; });
  }

  {
    std::cout << "Test anch::Optional utilities" << std::endl;
    anch::Optional<int> val(2);
    anch::Optional<int> noval;
    val.ifValue([](const int& v) { std::cout << "Found value " << v << std::endl; });
    noval.ifValue([](const int& v) { std::cout << "Found value " << v << std::endl; });
    noval.ifValueOrElse([](const int& v) { std::cout << "Found value " << v << std::endl; }, []() { std::cout << "No value ..." << std::endl; });
  }

  std::cout << "Exit testOptional" << std::endl;
}
