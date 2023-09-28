#include "logger/loggerFactory.hpp"

using anch::logger::LoggerFactory;
using anch::logger::Logger;


int
main(void) {
  auto LOG = LoggerFactory::getLogger("anch::logger::Logger");
  LOG.info("Message with default configuration");
  return 0;
}
