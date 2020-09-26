#include <iostream>
#include <sstream>

#include "json/json.hpp"


using anch::json::JSONMapper;
using anch::json::JSONFactory;


struct Tata {
  std::string ploum;
  std::string_view view;
  std::list<Tata> selfList;
  std::vector<std::string> strVect;
  std::set<uint64_t> numSet;
};

struct Toto {
  std::string plop;
  std::optional<std::string> plip;
  uint64_t plap;
  bool plup;
  Tata tata;
  float plep;
  double plyp;
  long double lplyp;
  Toto* self;
  std::optional<std::string> empty;
  std::string* ptr;
  std::string* null;
  std::string getClass() const {
    return "Toto";
  }
};

template<>
void
anch::json::registerFields(JSONMapper<Toto>& mapper) {
  std::cout << "Register Toto fields" << std::endl;
  mapper
    .registerField<>("plop", &Toto::plop)
    .registerField<std::optional<std::string>, std::string>("plip", &Toto::plip)
    .registerField<>("plap", &Toto::plap)
    .registerField<>("plup", &Toto::plup)
    .registerField<>("tata", &Toto::tata)
    .registerField<>("plep", &Toto::plep)
    .registerField<>("plyp", &Toto::plyp)
    .registerField<>("lplyp", &Toto::lplyp)
    .registerField<>("self", &Toto::self)
    .registerField<std::optional<std::string>, std::string>("invisible", &Toto::empty)
    .registerField<>("ptr", &Toto::ptr)
    .registerField<>("null", &Toto::null)
    .registerField<>("class", std::function<std::string(const Toto&)>(&Toto::getClass))
    ;
  std::cout << "Toto fields registered" << std::endl;
}

template<>
void
anch::json::registerFields(JSONMapper<Tata>& mapper) {
  std::cout << "Register Tata fields" << std::endl;
  mapper
    .registerField<>("ploum", &Tata::ploum)
    .registerField<>("view", &Tata::view)
    .registerField<std::set<uint64_t>,uint64_t>("num_set", &Tata::numSet)
    .registerField<std::vector<std::string>,std::string>("str_vector", &Tata::strVect)
    ;
  std::cout << "Tata fields registered" << std::endl;
}

struct Test {
  std::string _id;
  std::string _value;
  std::vector<int32_t> _nums;
  inline const std::string& getValue() const {
    return _value;
  }
};

template<>
void
anch::json::registerFields(JSONMapper<Test>& mapper) {
  mapper
    .registerField<>("id", &Test::_id)
    .registerField<>("value", std::function<const std::string&(const Test&)>(&Test::getValue))
    .registerField<std::vector<int32_t>, int32_t>("nums", &Test::_nums)
    ;
}

int
main(void) {
  std::cout << "Enter in serialization tests" << std::endl;
  {
    std::string VIEW = "VIEW";
    Toto self;
    self.plop = "self";
    self.plip = std::optional<std::string>("self_plip");
    self.plap = 24;
    self.plup = true;
    self.plep = static_cast<float>(5.5);
    self.plyp = 6.6;
    self.lplyp = 7.7;
    self.empty = std::optional<std::string>();
    self.ptr = &self.plop;
    self.null = NULL;
    self.self = NULL;
    Toto toto;
    toto.plop = "plop";
    toto.plip = std::optional<std::string>("plip");
    toto.plap = 42;
    toto.plup = false;
    toto.plep = static_cast<float>(2.2);
    toto.plyp = 3.3;
    toto.lplyp = 4.4;
    toto.empty = std::optional<std::string>();
    toto.ptr = &toto.plop;
    toto.null = NULL;
    toto.self = &self;
    Tata tata;
    tata.ploum = "ploum";
    tata.view = std::string_view(VIEW.data());
    tata.numSet = {1, 2, 3};
    tata.strVect = {"4","5","6"};
    toto.tata = tata;
    std::ostringstream oss;
    std::cout << "Serialize Toto" << std::endl;
    anch::json::serialize(toto, oss);
    std::cout << "Serialized toto: " << oss.str() << std::endl;
    std::cout << "Serialize Toto as string" << std::endl;
    std::string res = anch::json::serialize(toto);
    std::cout << "Serialized toto as string: " << res << std::endl;
  }
  /*JSONParser parser;
    std::string json("{}");
    std::istringstream iss(json);
    parser.parse<int>(iss);*/
  {
    Test test;
    test._id = "deb94ebc-be28-4899-981a-29199b7a487d";
    test._value = "this is a value";
    test._nums = {1,2,3,4};
    std::ostringstream oss;
    anch::json::serialize(test, oss);
    std::cout << "Serialized test: " << oss.str() << std::endl;
    std::string res = anch::json::serialize(test);
    std::cout << "Serialized test as string: " << res << std::endl;
  }
  std::cout << "Exit serialization tests" << std::endl;
  return 0;
}
