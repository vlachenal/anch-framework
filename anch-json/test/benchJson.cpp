#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>

#include "json/json.hpp"

#include "cli/args.hpp"
#include "cli/utils.hpp"



using anch::cli::App;
using anch::cli::Arg;
using anch::cli::ArgHandler;
using anch::json::ObjectMapper;
using anch::json::Factory;
using anch::json::MappingError;
using anch::json::ErrorCode;
using anch::json::JSONMapper;

struct Phone {
  std::string type; // \todo enum
  std::string number;
};

template<>
void
anch::json::registerObject(ObjectMapper<Phone>& mapper) {
  std::cout << "Register Phone fields" << std::endl;
  mapper
    .registerField("type", &Phone::type)
    .registerField("number", &Phone::number)
    ;
  std::cout << "Phone fields registered" << std::endl;
}

struct Address {
  std::list<std::string> lines;
  uint16_t zipCode;
  std::string city;
  std::string country;
};

template<>
void
anch::json::registerObject(ObjectMapper<Address>& mapper) {
  std::cout << "Register Address fields" << std::endl;
  mapper
    .registerField<std::string>("lines", &Address::lines)
    .registerField("zip_code", &Address::zipCode)
    .registerField("city", &Address::city)
    .registerField("country", &Address::country)
    ;
  std::cout << "Address fields registered" << std::endl;
}

struct Person {
  std::string firstName;
  std::string lastName;
  std::string birthDate; // \todo use date ...
  std::string email;
  std::list<Phone> phones;
  Address address;
};

template<>
void
anch::json::registerObject(ObjectMapper<Person>& mapper) {
  std::cout << "Register Person fields" << std::endl;
  mapper
    .registerField("first_name", &Person::firstName)
    .registerField("last_name", &Person::lastName)
    .registerField("birth_date", &Person::birthDate)
    .registerField("email", &Person::email)
    .registerField<Phone>("phones", &Person::phones)
    .registerField("address", &Person::address)
    ;
  std::cout << "Person fields registered" << std::endl;
}


struct Options {
  std::string input;
};

void
parseArgs(int argc, char** argv, Options& opts) {
  App application = {
    .name = "AnCH JSON bench",
    .version = "0.1",
    .author = "Vincent Lachenal",
    .copyright = "No copyrigth - it is a fuking unit test",
    .licence = "WTFPL 2.0",
    .banner = "Ceci est une bannière",
    .bannerPath = "/etc/hostname"
  };
  std::vector<anch::cli::Arg> args = {
    {.handler = anch::cli::bindStr(opts.input), .sopt = 'i', .lopt = "input-file", .value = true, .name = "FILE", .multi = true, .description = "JSON input file"}
  };
  ArgHandler handler(application, args);
  handler.printBanner(std::cout);
  handler.handle(argc, argv);
}

int
main(int argc, char** argv) {
  // Parse options +
  Options opts;
  parseArgs(argc, argv, opts);
  // Parse options -

  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  std::chrono::microseconds duration;

  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
  std::vector<Person> persons;
  // Read file +
  {
    std::cout << "Parse file " << opts.input << std::endl;
    std::ifstream file(opts.input);
    start = std::chrono::high_resolution_clock::now();
    mapper.deserialize<Person>(persons, file);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    std::cout << "File " << opts.input << " parsed ; nb persons = " << persons.size() << " in " << duration.count() << "µs" << std::endl;
  }
  // Read file -
  std::string json;
  // Serialize result as string +
  {
    std::cout << "Serialize person list" << std::endl;
    std::ostringstream oss;
    start = std::chrono::high_resolution_clock::now();
    mapper.serialize(persons, oss);
    json = oss.str();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    std::cout << "Person list serialized ; compact size : " << json.size() << " bytes in " << duration.count() << "µs" << std::endl;
  }
  // Serialize result as string -
  // Bench it +
  int64_t totalDeser = 0;
  int64_t totalSer = 0;
  int nbRun = 10000;
  for(int i = 0 ; i < nbRun ; ++i) {
    std::cout << '.';// << std::flush;
    // Deserialize +
    std::istringstream iss(json);
    std::vector<Person> tmp;
    start = std::chrono::high_resolution_clock::now();
    mapper.deserialize<Person>(tmp, iss);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    totalDeser += duration.count();
    // Deserialize -

    // Serialize +
    std::ostringstream oss;
    start = std::chrono::high_resolution_clock::now();
    mapper.serialize(tmp, oss);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    totalSer += duration.count();
    // Serialize -
  }
  std::cout << std::endl;
  std::cout << "Average time for deserialization: " << (totalDeser / nbRun) << "µs" << std::endl;
  std::cout << "Average time for serialization: " << (totalSer / nbRun) << "µs" << std::endl;
  // Bench it -
  return 0;
}
