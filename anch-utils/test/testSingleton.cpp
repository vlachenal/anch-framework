#include "singleton.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

#include "ut/unit.hpp"
#include "ut/assert.hpp"


class Test: public anch::Singleton<Test> {
  friend anch::Singleton<Test>;
private:
  int nb;
private:
  Test(): nb(0) {++nb;}
  virtual ~Test() {}
public:
  int getNb() const {return nb;}
};

class SlowTest: public anch::Singleton<SlowTest> {
  friend anch::Singleton<SlowTest>;
private:
  int nb;
private:
  SlowTest(): nb(0) {
    ++nb;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  virtual ~SlowTest() {}
public:
  int getNb() const {return nb;}
};

void
testSeqNoWait() {
  std::cout << "Enter in testSeqNoWait" << std::endl;
  Test& test1 = Test::getInstance();
  Test& test2 = Test::getInstance();
  std::cout << "test1: " << test1.getNb() << std::endl;
  std::cout << "test2: " << test2.getNb() << std::endl;
  anch::ut::assertTrue(test1.getNb() == 1);
  anch::ut::assertTrue(test2.getNb() == 1);
  std::cout << "Exit testSeqNoWait" << std::endl;
}

void
testParallelWait() {
  std::cout << "Enter in testParallelWait" << std::endl;
  std::atomic<int> nb = 0;
  for(int i = 0 ; i < 10 ; ++i) {
    std::thread thread([&nb]() { nb += SlowTest::getInstance().getNb(); });
    thread.detach();
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::cout << "Add instances nb: " << nb << std::endl;
  anch::ut::assertTrue(nb == 10);
  std::cout << "Exit testParallelWait" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH singleton unit tests")
    .description("Test AnCH singleton utiliy class")
    .add("seq-no-wait", testSeqNoWait)
    .add("parallel-wait", testParallelWait)
    ;
}
