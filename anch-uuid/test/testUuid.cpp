#include "uuid.hpp"

#include <iostream>
#include <chrono>
#include <list>
#include <cstring>
#include <set>

#include "ut/unit.hpp"

using anch::UUID;

void
testUUIDPerf() {
  std::cout << "Enter in UUID performance test" << std::endl;
  UUID::random(); // Register random UUID for performance mesures => register random UUID provider and initialize seed
  std::list<UUID> uuids;
  double totalTime = 0;
  for(std::size_t i = 0 ; i < 1000 ; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    UUID random = UUID::random();
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dur = stop - start;
    totalTime += dur.count();
    uuids.push_back(random);
  }
  std::cout << "Average generation time: " << totalTime / 1000 << "s" << std::endl;
  totalTime = 0;
  std::list<std::string> strUuids;
  for(UUID& uuid : uuids) {
    auto start = std::chrono::high_resolution_clock::now();
    std::string randomStr = static_cast<std::string>(uuid);
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dur = stop - start;
    totalTime += dur.count();
    strUuids.push_back(randomStr);
  }
  std::cout << "Average string from UUID convertion time: " << totalTime / 1000 << "s" << std::endl;
  totalTime = 0;
  for(std::string uuid : strUuids) {
    auto start = std::chrono::high_resolution_clock::now();
    UUID plop = UUID(uuid);
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dur = stop - start;
    totalTime += dur.count();
  }
  std::cout << "Average UUID from string convertion time: " << totalTime / 1000 << "s" << std::endl;

}

void
testUUIDCollision() {
  std::set<std::string> uuids;
  for(std::size_t i = 0 ; i < 1000000 ; ++i) {
    std::string random = UUID::random().toString();
    if(uuids.find(random) != uuids.end()) {
      std::cerr << "UUID " << random << " has already been generated" << std::endl;
    } else {
      uuids.insert(random);
    }
  }
  std::cout << uuids.size() << " different UUIDs have been generated" << std::endl;

}

void
tesUUIDSingle() {
  UUID random = UUID::random();
  std::cout << "Random UUID:                   " << random << std::endl;
  std::string randomStr = static_cast<std::string>(random);
  std::cout << "UUID random conversion:        " << randomStr << std::endl;
  std::cout << "UUID random string conversion: " << UUID(randomStr) << std::endl;
}

void
tesUUIDRadomTime() {
  UUID random = UUID::randomTime();
  std::cout << "Random time UUID:              " << random << std::endl;
  std::string randomStr = static_cast<std::string>(random);
  std::cout << "UUID random conversion:        " << randomStr << std::endl;
  std::cout << "UUID random string conversion: " << UUID(randomStr) << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH UUID unit tests")
    .description("Test AnCH UUID library")
    .add("uuid-perf", testUUIDPerf)
    .add("uuid-collision", testUUIDCollision)
    .add("uuid-random", tesUUIDSingle)
    .add("uuid-random-time", tesUUIDRadomTime)
    ;
}
