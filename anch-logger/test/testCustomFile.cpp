#include <iostream>

#include "logger/loggerFactory.hpp"

using std::cout;
using std::endl;

using anch::logger::LoggerFactory;
using anch::logger::Logger;


int
main(void) {

  const Logger& LOG = LoggerFactory::getLogger("anch::logger::Logger");

  LOG.info(_ANCH_LOGGER_CONFIG_FILE_);
  for(int i = 0 ; i < 200 ; i++) {
    LOG.info("Message n° ", i, " from main thread");
  }

  return 0;
}
