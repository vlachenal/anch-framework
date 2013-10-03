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
testMultiThread(const Logger& log) {
  for(int i = 0 ; i < 200000 ; i++) {
    log.info("Message n° ", i, " from the second thread");
  }
}


int
main(void) {

  const Logger& LOG = LoggerFactory::getLogger("anch::logger::Logger");

  LOG.info(_ANCH_LOGGER_CONFIG_FILE_);
  thread th = thread(testMultiThread,LOG);
  for(int i = 0 ; i < 200000 ; i++) {
    //    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG.info("Message n° ", i, " from main thread");
  }

  th.join();

  return 0;
}
