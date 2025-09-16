#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <random>
#include <array>

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


const std::array<char,72> printable = {
  'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
  '0','1','2','3','4','5','6','7','8','9',
  '@','/','=','+','-','%','$','&','#',';'
}; // 72 chars

// Seed with a real random value, if available
std::random_device RE;
// Choose a random mean between 0 and 71
std::uniform_int_distribution<uint64_t> RP(0, 71);

std::string
copyRandomStr(const std::string& str) {
  std::ostringstream oss;
  for(std::size_t i = 0 ; i < str.size() ; ++i) {
    oss << printable[RP(RE)];
  }
  return oss.str();
}

struct Phone {
  std::string type; // \todo enum
  std::string number;
  Phone(): type(), number() {}
  Phone(const Phone& phone): type(phone.type), number(phone.number) {/*
    type = copyRandomStr(phone.type);
    number = copyRandomStr(phone.number);*/
  }
  Phone(Phone&& phone): type(std::move(phone.type)), number(std::move(phone.number)) {}
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

std::uniform_int_distribution<uint16_t> RZ(1000, 9999);

struct Address {
  std::list<std::string> lines;
  uint16_t zipCode;
  std::string city;
  std::string country;
  Address(): lines(), zipCode(0), city(), country() {}
  Address(const Address& address): lines(address.lines), zipCode(/*RZ(RE)*/address.zipCode), city(address.city), country(address.country) {/*
    for(const std::string& line: lines) {
      lines.push_back(copyRandomStr(line));
    }
    city = copyRandomStr(address.city);
    country = copyRandomStr(address.country);*/
  }
  Address(Address&& address):
    lines(std::move(address.lines)),
    zipCode(address.zipCode),
    city(std::move(address.city)),
    country(std::move(address.country)) {
  }
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
  Person(): firstName(), lastName(), birthDate(), email(), phones(), address() {}
  Person(const Person& person): firstName(person.firstName), lastName(person.lastName), birthDate(person.birthDate), email(person.email), phones(person.phones), address(person.address) {
    /*firstName = copyRandomStr(person.firstName);
    lastName = copyRandomStr(person.lastName);
    birthDate = copyRandomStr(person.birthDate);
    email = copyRandomStr(person.email);
    for(const Phone& phone: person.phones) {
      Phone p(phone);
      phones.push_back(p);
      }*/
    //std::cout << "Copied" << std::endl;
  }
  Person(Person&& person):
    firstName(std::move(person.firstName)),
    lastName(std::move(person.lastName)),
    birthDate(std::move(person.birthDate)),
    email(std::move(person.email)),
    phones(std::move(person.phones)),
    address(std::move(person.address)) {
    //std::cout << "moved" << std::endl;
  }
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
    { .handler = anch::cli::bindStr(opts.input), .sopt = 'i', .lopt = "input-file", .value = true, .name = "FILE", .mandatory = true, .description = "JSON input file" }
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

  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128, .buffer_size = 1024});
  std::list<Person> persons;
  // Read file +
  {
    std::cout << "Parse file " << opts.input << std::endl;
    std::ifstream file(opts.input);
    start = std::chrono::high_resolution_clock::now();
    mapper.deserialize(persons, file);
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
    end = std::chrono::high_resolution_clock::now();
    json = oss.str();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    std::cout << "Person list serialized ; compact size : " << json.size() << " bytes in " << duration.count() << "µs" << std::endl;
  }
  // Serialize result as string -
  // Bench it +
  int64_t totalDeser = 0;
  int64_t totalSer = 0;
  //int nbRun = 10;
  int nbRun = 10000;
  for(int i = 0 ; i < nbRun ; ++i) {
    std::cout << '.' << std::flush;
    std::list<Person> tmp;
    for(const Person& person: persons) {
      tmp.push_back(person); // copy person
    }
    // Serialize +
    std::ostringstream oss;
    start = std::chrono::high_resolution_clock::now();
    mapper.serialize(tmp, oss);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    totalSer += duration.count();
    // Serialize -

    // Deserialize +
    std::list<Person> res;
    std::istringstream iss(oss.str());
    start = std::chrono::high_resolution_clock::now();
    mapper.deserialize(res, iss);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()) - std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch());
    totalDeser += duration.count();
    if(res.size() != tmp.size()) {
      std::cerr << "plop" << std::endl;
    }
    // Deserialize -
  }
  std::cout << std::endl;
  std::cout << "Average time for deserialization: " << (totalDeser / nbRun) << "µs" << std::endl;
  std::cout << "Average time for serialization: " << (totalSer / nbRun) << "µs" << std::endl;
  // Bench it -
  return 0;
}
