#include "threadPool.hpp"

#include <iostream>

#include "ut/unit.hpp"

using anch::ThreadPool;

class Test {
  friend class ThreadPool;
public:
  void test() {
    ThreadPool pool(4);
    // Wait infinitily +
    std::cout << "----------WAIT INFINITELY----------" << std::endl;
    for(int i = 0 ; i < 10 ; ++i) {
      std::cout << "Test: Add a new thread in queue " << i << std::endl;
      pool.add(&Test::sleep, this, i);
      std::cout << "Test: New thread has been added " << i << std::endl;
    }
    pool.start();
    pool.awaitTermination(std::chrono::seconds(-1));
    // Wait infinitily -
    // Wait a time long enough to execute threads +
    std::cout << "----------WAIT 10s----------" << std::endl;
    for(int i = 0 ; i < 10 ; ++i) {
      std::cout << "Test: Add a new thread in queue " << i << std::endl;
      pool.add(&Test::sleep, this, i);
      std::cout << "Test: New thread has been added " << i << std::endl;
    }
    pool.start();
    pool.awaitTermination(std::chrono::seconds(10));
    // Wait a time long enough to execute threads -
    // Wait short time ... +
    std::cout << "----------WAIT 1s----------" << std::endl;
    for(int i = 0 ; i < 20 ; ++i) {
      std::cout << "Test: Add a new thread in queue " << i << std::endl;
      pool.add(&Test::sleep, this, i);
      std::cout << "Test: New thread has been added " << i << std::endl;
    }
    pool.start();
    pool.awaitTermination(std::chrono::seconds(1));
    // Wait short time ... -
  }

private:
  void sleep(int index) {
    std::cout << "Test: Begin thread " << index << " execution in thread " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Test: End thread " << index << " execution in thread " << std::this_thread::get_id() << std::endl;
  }

};

void
sleepFor2s(int index) {
  std::cout << "Begin thread " << index << " execution in thread " << std::this_thread::get_id() << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "End thread " << index << " execution in thread " << std::this_thread::get_id() << std::endl;
}

// Anch unit tests +
void
testSleep2s() {
  std::cout << "Enter in testSleep2s test" << std::endl;
  ThreadPool pool(4);
  pool.start();
  for(int i = 0 ; i < 10 ; ++i) {
    std::cout << "Add a new thread in queue " << i << std::endl;
    pool.add(sleepFor2s, i);
    std::cout << "New thread has been added " << i << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::seconds(5));
  pool.stop();
  std::cout << "Exit testSleep2s test" << std::endl;
}

void
testClass() {
  std::cout << "Enter in testClass" << std::endl;
  std::cout << "Test for class method" << std::endl;
  Test test;
  test.test();
  std::cout << "Exit testClass" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH TheadPool unit tests")
    .description("Test AnCH TheadPool utiliy class")
    .add("thread-pool-sleep2s", testSleep2s)
    .add("thread-pool-class", testClass)
    ;
}
// Anch unit tests -
