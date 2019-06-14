#include "uuid.hpp"

#include <iostream>
#include <chrono>
#include <list>
#include <cstring>

using anch::UUID;

int
main(int argc, char** argv) {

  bool perf = (argc == 2 && strcmp("perf", argv[1]) == 0);

  if(perf) {
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

  } else {
    UUID random = UUID::random();
    std::cout << "Random UUID:                   " << random << std::endl;
    std::string randomStr = static_cast<std::string>(random);
    std::cout << "UUID random conversion:        " << randomStr << std::endl;
    std::cout << "UUID random string conversion: " << UUID(randomStr) << std::endl;
  }
  return 0;
}
