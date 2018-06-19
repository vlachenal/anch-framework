#include <iostream>
#include <list>
#include <sstream>

#include "stream.hpp"

struct A {
  std::string a;
  A(): a() {}
  A(const A& bis): a(bis.a) {}
};

struct B {
  std::string b;
  B(): b() {}
  B(const B& bis): b(bis.b) {}
  bool operator==(const B& other) const {
    return b == other.b;
  }
  bool operator<(const B& other) const {
    return b < other.b;
  }
};

namespace std {
  template<>
  class hash<::B> {
  public:
    size_t operator()(const B& b) const {
      return hash<string>()(b.b);
    }
  };
}

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

  std::list<A> as;
  for(int i = 0 ; i < 10 ; ++i) {
    A a;
    std::ostringstream oss;
    oss << "Test" << i;
    a.a = oss.str();
    std::cout << "A " << a.a << std::endl;
    as.push_back(a);
  }
  anch::Stream(as)
    .map<B>([](const A& a) -> B {
	      B b;
	      b.b = a.a;
	      return b;
	    })
    .forEach([](const B& b) {
	       std::cout << "B " << b.b << std::endl;
	     });

  std::list<B> resL = anch::Stream(as)
    .map<B>([](const A& a) -> B {
	      B b;
	      b.b = a.a;
	      return b;
	    })
    .collect<std::list>(&anch::Collectors<B>::toList);
  for(auto b : resL) {
    std::cout << "ListB " << b.b << std::endl;
  }

  std::set<B> resS = anch::Stream(as)
    .map<B>([](const A& a) -> B {
	      B b;
	      b.b = a.a;
	      return b;
	    })
    .collect<std::set>(&anch::Collectors<B>::toSet);
  for(auto b : resS) {
    std::cout << "SetB " << b.b << std::endl;
  }

  std::vector<B> resV = anch::Stream(as)
    .map<B>([](const A& a) -> B {
	      B b;
	      b.b = a.a;
	      return b;
	    })
    .collect<std::vector>(&anch::Collectors<B>::toVector);
  for(auto b : resV) {
    std::cout << "VectorB " << b.b << std::endl;
  }

  std::multiset<B> resMS = anch::Stream(as)
    .map<B>([](const A& a) -> B {
	      B b;
	      b.b = a.a;
	      return b;
	    })
    .collect<std::multiset>(&anch::Collectors<B>::toMultiset);
  for(auto b : resMS) {
    std::cout << "MSetB " << b.b << std::endl;
  }

  std::unordered_set<B> resUS = anch::Stream(as)
    .map<B>([](const A& a) -> B {
	      B b;
	      b.b = a.a;
	      return b;
	    })
    .collect<std::unordered_set>(&anch::Collectors<B>::toUnorderedSet);
  for(auto b : resUS) {
    std::cout << "USetB " << b.b << std::endl;
  }

  std::cout << "Exit testStream" << std::endl;
  return 0;
}
