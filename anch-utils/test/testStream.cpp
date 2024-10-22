#include <iostream>
#include <sstream>
#include <map>
#include <any>
#include <vector>
#include <list>
#include <forward_list>
#include <deque>
#include <stack>
#include <queue>
#include <set>
#include <unordered_set>

#include "fluent.hpp"
#include "collectors.hpp"

#include "ut/unit.hpp"
#include "ut/assert.hpp"

struct A {
  std::string a;
  A(): a() {}
  A(const A& bis): a(bis.a) {}
};

struct B {
  std::string b;
  B(): b() {}
  //B(const B& bis): b(bis.b) {}
  bool operator==(const B& other) const {
    return b == other.b;
  }
  bool operator!=(const B& other) const {
    return b != other.b;
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

std::list<int> ints = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
std::list<int> ints1 = {0,1,2,3,4,5,6,7,8,9};
std::list<int> ints2 = {10,11,12,13,14,15,16,17,18,19};

std::list<A> as;
std::list<B> expected;

void
beforeAll() {
  for(int i = 0 ; i < 10 ; ++i) {
    A a;
    std::ostringstream oss;
    oss << "Test" << i;
    a.a = oss.str();
    as.push_back(a);
    B b;
    b.b = a.a;
    expected.push_back(b);
  }
}

void
testForeach() {
  std::cout << "Enter in testForeach" << std::endl;
  std::cout << "Values:";
  std::list<int> res;
  anch::Fluent(ints)
    .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
  std::cout << std::endl;
  anch::ut::assert(res == ints, "Unexpected result");
  std::cout << "Exit testForeach" << std::endl;
}

void
testFilter() {
  std::cout << "Enter in testFilter" << std::endl;
  std::cout << "Even values:";
  std::list<int>  res;
  anch::Fluent(ints)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
  std::cout << std::endl;
  anch::ut::assert(res == std::list<int>{0,2,4,6,8,10,12,14,16,18}, "Unexpected result");
  std::cout << "Exit testFilter" << std::endl;
}

void
testSkip() {
  std::cout << "Enter in testSkip" << std::endl;
  std::cout << "Even values skipping three first values:";
  std::list<int> res;
  anch::Fluent(ints)
    .skip(3)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
  std::cout << std::endl;
  anch::ut::assert(res == std::list<int>{4,6,8,10,12,14,16,18}, "Unexpected result");
  std::cout << "Exit testSkip" << std::endl;
}

void
testLimit() {
  std::cout << "Enter in testLimit" << std::endl;
  std::cout << "Even values skipping three first values and treat at most five values:";
  std::list<int> res;
  anch::Fluent(ints)
    .skip(3)
    .limit(5)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
  std::cout << std::endl;
  anch::ut::assert(res == std::list<int>{4,6}, "Unexpected result");
  std::cout << "Exit testLimit" << std::endl;
}

void
testConcat() {
  std::cout << "Enter in testConcat" << std::endl;
  std::cout << "Even values skipping three first concatenated values and treat at most five values:";
  std::list<int> res;
  auto stream2 = anch::Fluent(ints2);
  anch::Fluent(ints1)
    .concat(stream2)
    .skip(3)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .limit(5)
    .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
  std::cout << std::endl;
  anch::ut::assert(res == std::list<int>{4,6,8,10,12}, "Unexpected result");
  std::cout << "Exit testConcat" << std::endl;
}

void
testAllMatch() {
  std::cout << "Enter in testAllMatch" << std::endl;
  std::cout << "Check all even values skipping three first concatenated values and treat at most five values: ";
  auto stream2 = anch::Fluent(ints2);
  bool match = anch::Fluent(ints1)
    .concat(stream2)
    .skip(3)
    .filter([](const int& i) -> bool { return (i % 2) == 0; })
    .limit(5)
    .allMatch([](const int& i) -> bool { return (i % 2) == 0; });
  std::cout << (match ? "true" : "false") << std::endl;
  anch::ut::assertTrue(match);
  std::cout << "Exit testAllMatch" << std::endl;
}

void
testAnyMatch() {
  std::cout << "Enter in testAnyMatch" << std::endl;
  std::cout << "Check if contains values > 15: ";
  auto stream2 = anch::Fluent(ints2);
  bool match = anch::Fluent(ints1)
    .anyMatch([](const int& i) -> bool { return i > 15; });
  std::cout << (match ? "true" : "false") << std::endl;
  anch::ut::assertFalse(match);

  std::cout << "Check if contains values > 15: ";
  match = anch::Fluent(ints1)
    .concat(stream2)
    .anyMatch([](const int& i) -> bool { return i > 15; });
  std::cout << (match ? "true" : "false") << std::endl;
  anch::ut::assertTrue(match);
  std::cout << "Exit testAnyMatch" << std::endl;
}

void
testNoneMatch() {
  std::cout << "Enter in testNoneMatch" << std::endl;
  std::cout << "Check all even values skipping three first concatenated values and treat at most five values: ";
  auto stream2 = anch::Fluent(ints2);
  bool match = anch::Fluent(ints1)
    .concat(stream2)
    .skip(3)
    .noneMatch([](const int& i) -> bool { return i < 100; });
  std::cout << (match ? "true" : "false") << std::endl;
  anch::ut::assertFalse(match);
  std::cout << "Exit testNoneMatch" << std::endl;
}

void
testMap() {
  std::cout << "Enter in testMap" << std::endl;
  std::list<B> res;
  anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .forEach([&res](const B& b) {
      std::cout << "B " << b.b << std::endl;
      res.push_back(b);
    });
  anch::ut::assert(res == expected, "Unexpected result");
  std::cout << "Exit testMap" << std::endl;
}

void
testCollectList() {
  std::cout << "Enter in testCollectList" << std::endl;
  std::list<B> resL = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect(&std::list<B>::push_back);
  for(auto b : resL) {
    std::cout << "ListB " << b.b << std::endl;
  }
  anch::ut::assert(resL == expected, "Unexpected result");
  std::cout << "Exit testCollectList" << std::endl;
}

void
testCollectForwardList() {
  std::cout << "Enter in testCollectForwardList" << std::endl;
  std::forward_list<B> exp(expected.crbegin(), expected.crend());
  std::forward_list<B> resFL = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect(&std::forward_list<B>::push_front);
  for(auto b : resFL) {
    std::cout << "ForwardListB " << b.b << std::endl;
  }
  anch::ut::assert(resFL == exp, "Unexpected result");
  std::cout << "Exit testCollectForwardList" << std::endl;
}

void
testCollectVector() {
  std::cout << "Enter in testCollectVector" << std::endl;
  std::vector<B> exp(expected.cbegin(), expected.cend());
  std::vector<B> resV = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect(&std::vector<B>::push_back);
  for(auto b : resV) {
    std::cout << "VectorB " << b.b << std::endl;
  }
  anch::ut::assert(resV == exp, "Unexpected result");
  std::cout << "Exit testCollectVector" << std::endl;
}

void
testCollectSet() {
  std::cout << "Enter in testCollectSet" << std::endl;
  std::set<B> exp(expected.cbegin(), expected.cend());
  std::set<B> resS = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect<std::set<B>>(&anch::Collectors<B>::insert<std::set<B>>);
  //.collect<std::set<B>>([](std::set<B>& res, const B& b) {res.insert(b);});
  for(auto b : resS) {
    std::cout << "SetB " << b.b << std::endl;
  }
  anch::ut::assert(resS == exp, "Unexpected result");
  std::cout << "Exit testCollectSet" << std::endl;
}

void
testCollectMultiSet() {
  std::cout << "Enter in testCollectMultiSet" << std::endl;
  std::multiset<B> exp(expected.cbegin(), expected.cend());
  std::multiset<B> resMS = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect<std::multiset<B>>(&anch::Collectors<B>::insert<std::multiset<B>>);
  //.collect<std::multiset<B>>([](std::multiset<B>& res, const B& b) {res.insert(b);});
  for(auto b : resMS) {
    std::cout << "MSetB " << b.b << std::endl;
  }
  anch::ut::assert(resMS == exp, "Unexpected result");
  std::cout << "Exit testCollectMultiSet" << std::endl;
}

void
testCollectUnorderedSet() {
  std::cout << "Enter in testCollectUnorderedSet" << std::endl;
  std::unordered_set<B> exp(expected.cbegin(), expected.cend());
  std::unordered_set<B> resUS = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    //.collect<std::unordered_set<B>>(&anch::Collectors<B>::insert<std::unordered_set<B>>);
    //.collect<std::unordered_set<B>>([](std::unordered_set<B>& res, const B& b) {res.insert(b);});
    .collect<std::unordered_set<B>,std::pair<std::unordered_multiset<B>::iterator,bool>>(&std::unordered_set<B>::insert);
  //.collect(&std::unordered_set<B>::insert);
  for(auto b : resUS) {
    std::cout << "USetB " << b.b << std::endl;
  }
  anch::ut::assert(resUS == exp, "Unexpected result");
  std::cout << "Exit testCollectUnorderedSet" << std::endl;
}

void
testCollectUnorderedMultiset() {
  std::cout << "Enter in testCollectUnorderedMultiset" << std::endl;
  std::unordered_multiset<B> exp(expected.cbegin(), expected.cend());
  std::unordered_multiset<B> resUS = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B	 b;
      b.b = a.a;
      return b;
    })
    //.collect<std::unordered_multiset<B>>(&anch::Collectors<B>::insert<std::unordered_multiset<B>>);
    //.collect<std::unordered_multiset<B>>([](std::unordered_multiset<B>& res, const B& b) {res.insert(b);});
    .collect<std::unordered_multiset<B>,std::unordered_multiset<B>::iterator>(&std::unordered_multiset<B>::insert);
  //.collect(&std::unordered_multiset<B>::insert);
  for(auto b : resUS) {
    std::cout << "USetB " << b.b << std::endl;
  }
  anch::ut::assert(resUS == exp, "Unexpected result");
  std::cout << "Exit testCollectUnorderedMultiset" << std::endl;
}

void
testCollectDeque() {
  std::cout << "Enter in testCollectDeque" << std::endl;
  std::deque<B> exp(expected.cbegin(), expected.cend());
  std::deque<B> resDQ = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect(&std::deque<B>::push_back);
  for(auto b : resDQ) {
    std::cout << "DequeB " << b.b << std::endl;
  }
  anch::ut::assert(resDQ == exp, "Unexpected result");
  std::cout << "Exit testCollectDeque" << std::endl;
}

void
testCollectStack() {
  std::cout << "Enter in testCollectStack" << std::endl;
  std::stack<B> exp;
  for(const B& b : expected) {
    exp.push(b);
  }
  std::stack<B> resSt = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect(&std::stack<B>::push);
  anch::ut::assert(resSt == exp, "Unexpected result");
  while(!resSt.empty()) {
    std::cout << "StackB " << resSt.top().b << std::endl;
    resSt.pop();
  }
  std::cout << "Exit testCollectStack" << std::endl;
}

void
testCollectQueue() {
  std::cout << "Enter in testCollectQueue" << std::endl;
  std::queue<B> exp;
  for(const B& b : expected) {
    exp.push(b);
  }
  std::queue<B> resQ = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect(&std::queue<B>::push);
  anch::ut::assert(resQ == exp, "Unexpected result");
  while(!resQ.empty()) {
    std::cout << "QueueB " << resQ.front().b << std::endl;
    resQ.pop();
  }
  std::cout << "Exit testCollectQueue" << std::endl;
}

void
testCollectPriorityQueue() {
  std::cout << "Enter in testCollectPriorityQueue" << std::endl;
  std::priority_queue<B> exp;
  for(const B& b : expected) {
    exp.push(b);
  }
  std::priority_queue<B> resPQ = anch::Fluent(as)
    .map<B>([](const A& a) -> B {
      B b;
      b.b = a.a;
      return b;
    })
    .collect(&std::priority_queue<B>::push);
  bool ok = true;
  while(!resPQ.empty()) {
    std::cout << "PQueueB " << resPQ.top().b << std::endl;
    ok &= (resPQ.top().b == exp.top().b);
    resPQ.pop();
    exp.pop();
  }
  anch::ut::assertTrue(ok, "Unexpected result");
  std::cout << "Exit testCollectPriorityQueue" << std::endl;
}

void
testCollectMap() {
  std::cout << "Enter in testCollectMap" << std::endl;
  std::map<std::size_t,B> exp;
  std::size_t idx = 0;
  for(const B& b : expected) {
    exp[idx++] = b;
  }
  idx = 0;
  std::map<std::size_t,B> resM = anch::Fluent(as)
    .map<std::pair<std::size_t,B>>([&idx](const A& a) -> auto {
      B b;
      b.b = a.a;
      return std::pair(idx++,b);
    })
    .collect<std::map<std::size_t,B>>([](std::map<std::size_t,B>& res, const std::pair<std::size_t,B>& p) {res.insert(p);});
  for(auto item : resM) {
    std::cout << "Map: " << item.first << '=' << item.second.b << std::endl;
  }
  anch::ut::assert(resM == exp, "Unexpected result");
  std::cout << "Exit testCollectMap" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH fluent API unit tests")
    .description("Test AnCH fluent API utiliy class")
    .initialize(beforeAll)
    .add("foreach", testForeach)
    .add("filter", testFilter)
    .add("skip", testSkip)
    .add("limit", testLimit)
    .add("concat", testConcat)
    .add("all-match", testAllMatch)
    .add("any-match", testAnyMatch)
    .add("none-match", testNoneMatch)
    .add("map", testMap)
    .add("collect-list", testCollectList)
    .add("collect-forward-list", testCollectForwardList)
    .add("collect-vector", testCollectVector)
    .add("collect-set", testCollectSet)
    .add("collect-multiset", testCollectMultiSet)
    .add("collect-unordered-set", testCollectUnorderedSet)
    .add("collect-unordered-multiset", testCollectUnorderedMultiset)
    .add("collect-deque", testCollectDeque)
    .add("collect-stack", testCollectStack)
    .add("collect-queue", testCollectQueue)
    .add("collect-priority-queue", testCollectPriorityQueue)
    .add("collect-map", testCollectMap)
    ;
}
