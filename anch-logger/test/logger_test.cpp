#include <iostream>
#include <thread>
#include <chrono>

#include "logger/loggerFactory.hpp"

using std::cout;
using std::endl;
using std::thread;

using anch::logger::LoggerFactory;
using anch::logger::Logger;

void
testMultiThread(const Logger& LOG) {
  for(int i = 0 ; i < 5 ; i++) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG.info("toto");
  }
}


int
main(void) {

  cout << "Begin tests" << endl;

  const Logger& LOG = LoggerFactory::getLogger("anch::logger::Logger");
  LOG.info("info stuff");
  LOG.info("Concatenated"," ","info"," ","stuffs");

  thread th = thread(testMultiThread,LOG);
  for(int i = 0 ; i < 5 ; i++) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG.info("tata");
  }

  th.join();

  cout << "End tests" << endl;

  return 0;
}
