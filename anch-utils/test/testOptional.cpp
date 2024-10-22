#include "optional.hpp"

#include <iostream>

#include "ut/unit.hpp"
#include "ut/assert.hpp"

struct A {
  std::string a;
};

struct B {
  std::string b;
};

void
testOptionalFunc() {
  std::cout << "Test functions optional utilities" << std::endl;
  std::optional<int> val(2);
  std::optional<int> noval;
  anch::applyIfValue<int>(val, [](const int& v) { std::cout << "Found value " << v << std::endl; });
  anch::applyIfValue<int>(noval, [](const int& v) { std::cout << "Found value " << v << std::endl; });
  anch::applyIfValueOrElse<int>(noval, [](const int& v) { std::cout << "Found value " << v << std::endl; }, []() { std::cout << "No value ..." << std::endl; });
}

void
testOptionalClass() {
  std::cout << "Test anch::Optional utilities" << std::endl;
  anch::Optional<int> val(2);
  anch::Optional<int> noval;
  val.ifValue([](const int& v) { std::cout << "Found value " << v << std::endl; });
  noval.ifValue([](const int& v) { std::cout << "Found value " << v << std::endl; });
  noval.ifValueOrElse([](const int& v) { std::cout << "Found value " << v << std::endl; }, []() { std::cout << "No value ..." << std::endl; });

  std::cout << "Test mapping features" << std::endl;
  A a1;
  a1.a = "test mapping";
  anch::Optional<A> opt1(a1);
  anch::Optional<B> optM = opt1.map<B>([](const A& a) {
    B b;
    b.b = a.a;
    return b;
  });
  optM.ifValue([](const B& b) { std::cout << "Found mapped value: " << b.b << std::endl; });
  anch::Optional<A> opt2;
  optM = opt2.map<B>([](const A& a) {
    B b;
    b.b = a.a;
    return b;
  });
  optM.ifValueOrElse([](const B& b) { std::cout << "Found mapped value: " << b.b << std::endl; }, []() { std::cout << "No value ..." << std::endl; });
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH optioanl unit tests")
    .description("Test AnCH optional utiliy class and functions")
    .add("opt-func", testOptionalFunc)
    .add("opt-class", testOptionalClass)
    ;
}
