#include "logger/loggerFactory.hpp"

using anch::logger::LoggerFactory;
using anch::logger::Logger;

ANCH_LOGGER_INIT


int
main(void) {
  const Logger& LOG = LoggerFactory::getLogger("anch::logger::Logger");
  LOG.info("Message with default configuration");
  return 0;
}
