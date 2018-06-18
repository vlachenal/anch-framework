#include <iostream>
#include <list>

#include "stream.hpp"

int
main(void) {
  std::cout << "Enter in testStream" << std::endl;

  std::list<int> ints = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
  std::cout << "Values:";
  anch::Stream(ints)
    .forEach([](int& i) { std::cout << " " << i; });
  std::cout << std::endl;

  std::cout << "Even values:";
  anch::Stream(ints)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .forEach([](int& i) { std::cout << " " << i; });
  std::cout << std::endl;

  std::cout << "Even values skipping three first values:";
  anch::Stream(ints)
    .skip(3)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .forEach([](int& i) { std::cout << " " << i; });
  std::cout << std::endl;

  std::cout << "Even values skipping three first values and treat at most five values:";
  anch::Stream(ints)
    .skip(3)
    .limit(5)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .forEach([](int& i) { std::cout << " " << i; });
  std::cout << std::endl;

  std::cout << "Even values skipping three first concatenated values and treat at most five values:";
  std::list<int> ints1 = {0,1,2,3,4,5,6,7,8,9};
  std::list<int> ints2 = {10,11,12,13,14,15,16,17,18,19};
  auto stream2 = anch::Stream(ints2);
  anch::Stream(ints1)
    .concat(stream2)
    .skip(3)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .limit(5)
    .forEach([](int& i) { std::cout << " " << i; });
  std::cout << std::endl;

  std::cout << "Check all even values skipping three first concatenated values and treat at most five values: ";
  bool match = anch::Stream(ints1)
    .concat(stream2)
    .skip(3)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .limit(5)
    .allMatch([](const int& i) -> bool { return (i % 2) == 0; });
  std::cout << (match ? "true" : "false") << std::endl;

  std::cout << "Check if contains values > 15: ";
  match = anch::Stream(ints1)
    .anyMatch([](const int& i) -> bool { return i > 15; });
  std::cout << (match ? "true" : "false") << std::endl;

  std::cout << "Check if contains values > 15: ";
  match = anch::Stream(ints1)
    .concat(stream2)
    .anyMatch([](const int& i) -> bool { return i > 15; });
  std::cout << (match ? "true" : "false") << std::endl;

  std::cout << "Exit testStream" << std::endl;
  return 0;
}
