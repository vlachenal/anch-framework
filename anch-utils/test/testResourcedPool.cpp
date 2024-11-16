#include "resourcePool.hpp"

#include <iostream>
#include <random>
#include <thread>

#include "ut/unit.hpp"

class TestConfig {
public:
  virtual ~TestConfig() {
  }
};

class Test {
private:
  static std::default_random_engine randNgin;
  static std::uniform_int_distribution<int> rand;
  static std::atomic<int> globalIdx;
  int _idx;
  const TestConfig& _config;
  bool _valid;
public:
  Test(const TestConfig& config): _idx(globalIdx.fetch_add(1)), _config(config), _valid(true) {
    std::cout << "Build Test " << _idx << std::endl;
  }

  virtual ~Test() {
    std::cout << "Destroy Test " << _idx << std::endl;
  }

  void doStuff() {
    std::cout << "Stuff " << _idx << std::endl;
    int nb = Test::rand(Test::randNgin);
    if(nb == 4) {
      _valid = false;
    }
  }

  inline int getIdx() const {
    return _idx;
  }

  inline bool isValid() const {
    return _valid;
  }
};

std::random_device rDev;
std::default_random_engine Test::randNgin(rDev());
std::uniform_int_distribution<int> Test::rand(1,4);
std::atomic<int> Test::globalIdx;

using TestPool = anch::ResourcePool<Test, TestConfig>;

void
doStuff(TestPool* const pool) {
  try {
    auto res = pool->borrowResource();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    res.get().doStuff();
  } catch(const anch::TimeoutException& e) {
    std::cerr << "Timeout error: " << e.what() << " ; timeout=" << e.getTimeout().count() << "ms" << std::endl;
  }
}

class Test2 {
private:
  static std::default_random_engine randNgin;
  static std::uniform_int_distribution<int> rand;
  static std::atomic<int> globalIdx;
  int _idx;
  const TestConfig& _config;
  bool _valid;
  std::mutex _mutex;
public:
  Test2(const TestConfig& config): _idx(globalIdx.fetch_add(1)), _config(config), _valid(true), _mutex() {
    std::cout << "Build Test2 " << _idx << std::endl;
  }

  virtual ~Test2() {
    std::cout << "Destroy Test2 " << _idx << std::endl;
  }

  void doStuff() {
    if(_mutex.try_lock()) {
      std::cout << "Stuff2 " << _idx << std::endl;
      int nb = Test2::rand(Test2::randNgin);
      if(nb == 4) {
	_valid = false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      _mutex.unlock();
    } else {
      std::cerr << "Test2 " << _idx << " is already in use ..." << std::endl;
    }
  }

  inline int getIdx() const {
    return _idx;
  }

  inline bool isValid() const {
    return _valid;
  }
};

std::default_random_engine Test2::randNgin(rDev());
std::uniform_int_distribution<int> Test2::rand(1,4);
std::atomic<int> Test2::globalIdx;

using Test2Pool = anch::ResourcePool<Test2, TestConfig>;

void
doStuff2(Test2Pool* const pool) {
  try {
    pool->borrowResource().get().doStuff();
  } catch(const anch::TimeoutException& e) {
    std::cerr << "Timeout error: " << e.what() << " ; timeout=" << e.getTimeout().count() << "ms" << std::endl;
  }
}

// Anch unit tests +
void
testPool1() {
  std::cout << "Enter in testPool1 test" << std::endl;
  TestConfig config;
  TestPool pool(config, 10, 5);
  for(int i = 0 ; i < 15 ; ++i) {
    std::thread t(doStuff, &pool);
    t.detach();
  }
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "Exit testPool1 test" << std::endl;
}

void
testPool2() {
  std::cout << "Enter in testPool2 test" << std::endl;
  TestConfig config;
  Test2Pool pool2(config, 10, 5);
  for(int i = 0 ; i < 15 ; ++i) {
    std::thread t(doStuff2, &pool2);
    t.detach();
  }
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "Exit testPool2 test" << std::endl;
}

/*
class Notvalid {
};

void
testPoolNotValid() {
  std::cout << "Enter in testPoolNotValid test" << std::endl;
  anch::ResourcePool<Notvalid, TestConfig> pool;
  std::cout << pool << std::endl;
  std::cout << "Exit testPoolNotValid test" << std::endl;
}
*/

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH resource pool unit tests")
    .description("Test AnCH resource pool utiliy class")
    .add("res-pool1", testPool1)
    .add("res-pool2", testPool2)
    ;
}
// Anch unit tests -
