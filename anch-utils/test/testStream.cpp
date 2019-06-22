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

#include "stream.hpp"
#include "collectors.hpp"

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

int
main(int argc, char** argv) {
  std::cout << "Enter in testStream" << std::endl;

  std::set<std::string> args;
  bool all = false;
  if(argc > 1) {
    for(int i = 1 ; i < argc ; ++i) {
      args.insert(argv[i]);
    }
  } else {
    all = true;
  }

  std::list<int> ints = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
  std::list<int> ints1 = {0,1,2,3,4,5,6,7,8,9};
  std::list<int> ints2 = {10,11,12,13,14,15,16,17,18,19};

  if(all || args.find("foreach") != args.end()) {
    std::cout << "Values:";
    std::list<int> res;
    anch::Stream(ints)
      .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
    std::cout << std::endl;
    if(res != ints) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("filter") != args.end()) {
    std::cout << "Even values:";
    std::list<int>  res;
    anch::Stream(ints)
      .filter([](const int& i) -> bool { return (i % 2) == 0; })
      .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
    std::cout << std::endl;
    if(res != std::list<int>{0,2,4,6,8,10,12,14,16,18}) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("skip") != args.end()) {
    std::cout << "Even values skipping three first values:";
    std::list<int> res;
    anch::Stream(ints)
      .skip(3)
      .filter([](const int& i) -> bool { return (i % 2) == 0; })
      .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
    std::cout << std::endl;
    if(res != std::list<int>{4,6,8,10,12,14,16,18}) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("limit") != args.end()) {
    std::cout << "Even values skipping three first values and treat at most five values:";
    std::list<int> res;
    anch::Stream(ints)
      .skip(3)
      .limit(5)
      .filter([](const int& i) -> bool { return (i % 2) == 0; })
      .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
    std::cout << std::endl;
    if(res != std::list<int>{4,6}) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("concat") != args.end()) {
    std::cout << "Even values skipping three first concatenated values and treat at most five values:";
    std::list<int> res;
    auto stream2 = anch::Stream(ints2);
    anch::Stream(ints1)
      .concat(stream2)
      .skip(3)
      .filter([](const int& i) -> bool { return (i % 2) == 0; })
      .limit(5)
      .forEach([&res](int& i) { std::cout << " " << i; res.push_back(i); });
    std::cout << std::endl;
    if(res != std::list<int>{4,6,8,10,12}) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("allMatch") != args.end()) {
    std::cout << "Check all even values skipping three first concatenated values and treat at most five values: ";
    auto stream2 = anch::Stream(ints2);
    bool match = anch::Stream(ints1)
      .concat(stream2)
      .skip(3)
      .filter([](const int& i) -> bool { return (i % 2) == 0; })
      .limit(5)
      .allMatch([](const int& i) -> bool { return (i % 2) == 0; });
    std::cout << (match ? "true" : "false") << std::endl;
    if(!match) {
      return 1;
    }
  }

  if(all || args.find("anyMatch") != args.end()) {
    std::cout << "Check if contains values > 15: ";
    auto stream2 = anch::Stream(ints2);
    bool match = anch::Stream(ints1)
      .anyMatch([](const int& i) -> bool { return i > 15; });
    std::cout << (match ? "true" : "false") << std::endl;
    if(match) {
      return 1;
    }

    std::cout << "Check if contains values > 15: ";
    match = anch::Stream(ints1)
      .concat(stream2)
      .anyMatch([](const int& i) -> bool { return i > 15; });
    std::cout << (match ? "true" : "false") << std::endl;
    if(!match) {
      return 1;
    }
  }

  if(all || args.find("noneMatch") != args.end()) {
    std::cout << "Check all even values skipping three first concatenated values and treat at most five values: ";
    auto stream2 = anch::Stream(ints2);
    bool match = anch::Stream(ints1)
      .concat(stream2)
      .skip(3)
      .noneMatch([](const int& i) -> bool { return i < 100; });
    std::cout << (match ? "true" : "false") << std::endl;
    if(match) {
      return 1;
    }
  }

  std::list<A> as;
  std::list<B> expected;
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
  if(all || args.find("map") != args.end()) {
    std::list<B> res;
    anch::Stream(as)
      .map<B>([](const A& a) -> B {
		B b;
		b.b = a.a;
		return b;
	      })
      .forEach([&res](const B& b) {
		 std::cout << "B " << b.b << std::endl;
		 res.push_back(b);
	       });
    if(res != expected) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_list") != args.end()) {
    std::list<B> resL = anch::Stream(as)
      .map<B>([](const A& a) -> B {
		B b;
		b.b = a.a;
		return b;
	      })
      .collect(&std::list<B>::push_back);
    for(auto b : resL) {
      std::cout << "ListB " << b.b << std::endl;
    }
    if(resL != expected) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_forward_list") != args.end()) {
    std::forward_list<B> exp(expected.crbegin(), expected.crend());
    std::forward_list<B> resFL = anch::Stream(as)
      .map<B>([](const A& a) -> B {
		B b;
		b.b = a.a;
		return b;
	      })
      .collect(&std::forward_list<B>::push_front);
    for(auto b : resFL) {
      std::cout << "ForwardListB " << b.b << std::endl;
    }
    if(resFL != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_vector") != args.end()) {
    std::vector<B> exp(expected.cbegin(), expected.cend());
    std::vector<B> resV = anch::Stream(as)
      .map<B>([](const A& a) -> B {
		B b;
		b.b = a.a;
		return b;
	      })
      .collect(&std::vector<B>::push_back);
    for(auto b : resV) {
      std::cout << "VectorB " << b.b << std::endl;
    }
    if(resV != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_set") != args.end()) {
    std::set<B> exp(expected.cbegin(), expected.cend());
    std::set<B> resS = anch::Stream(as)
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
    if(resS != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_multiset") != args.end()) {
    std::multiset<B> exp(expected.cbegin(), expected.cend());
    std::multiset<B> resMS = anch::Stream(as)
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
    if(resMS != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_unordered_set") != args.end()) {
    std::unordered_set<B> exp(expected.cbegin(), expected.cend());
    std::unordered_set<B> resUS = anch::Stream(as)
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
    if(resUS != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_unordered_multiset") != args.end()) {
    std::unordered_multiset<B> exp(expected.cbegin(), expected.cend());
    std::unordered_multiset<B> resUS = anch::Stream(as)
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
    if(resUS != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_deque") != args.end()) {
    std::deque<B> exp(expected.cbegin(), expected.cend());
    std::deque<B> resDQ = anch::Stream(as)
      .map<B>([](const A& a) -> B {
		B b;
		b.b = a.a;
		return b;
	      })
      .collect(&std::deque<B>::push_back);
    for(auto b : resDQ) {
      std::cout << "DequeB " << b.b << std::endl;
    }
    if(resDQ != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_stack") != args.end()) {
    std::stack<B> exp;
    for(const B& b : expected) {
      exp.push(b);
    }
    std::stack<B> resSt = anch::Stream(as)
      .map<B>([](const A& a) -> B {
		B b;
		b.b = a.a;
		return b;
	      })
      .collect(&std::stack<B>::push);
    if(resSt != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
    while(!resSt.empty()) {
      std::cout << "StackB " << resSt.top().b << std::endl;
      resSt.pop();
    }
  }

  if(all || args.find("collect_queue") != args.end()) {
    std::queue<B> exp;
    for(const B& b : expected) {
      exp.push(b);
    }
    std::queue<B> resQ = anch::Stream(as)
      .map<B>([](const A& a) -> B {
		B b;
		b.b = a.a;
		return b;
	      })
      .collect(&std::queue<B>::push);
    if(resQ != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
    while(!resQ.empty()) {
      std::cout << "QueueB " << resQ.front().b << std::endl;
      resQ.pop();
    }
  }

  if(all || args.find("collect_priority_queue") != args.end()) {
    std::priority_queue<B> exp;
    for(const B& b : expected) {
      exp.push(b);
    }
    std::priority_queue<B> resPQ = anch::Stream(as)
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
    if(!ok) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  if(all || args.find("collect_map") != args.end()) {
    std::map<std::size_t,B> exp;
    std::size_t idx = 0;
    for(const B& b : expected) {
      exp[idx++] = b;
    }
    idx = 0;
    std::map<std::size_t,B> resM = anch::Stream(as)
      .map<std::pair<std::size_t,B>>([&idx](const A& a) -> auto {
				       B b;
				       b.b = a.a;
				       return std::pair(idx++,b);
				     })
      .collect<std::map<std::size_t,B>>([](std::map<std::size_t,B>& res, const std::pair<std::size_t,B>& p) {res.insert(p);});
    for(auto item : resM) {
      std::cout << "Map: " << item.first << '=' << item.second.b << std::endl;
    }
    if(resM != exp) {
      std::cerr << "Unexpected result" << std::endl;
      return 1;
    }
  }

  std::cout << "Exit testStream" << std::endl;
  return 0;
}
