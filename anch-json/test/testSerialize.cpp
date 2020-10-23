#include <iostream>
#include <sstream>
#include <fstream>

#include "json/json.hpp"


using anch::json::ObjectMapper;
using anch::json::Factory;


struct Tata {
  std::string ploum;
  std::string_view view;
  std::list<Tata> selfList;
  std::vector<std::string> strVect;
  std::set<uint64_t> numSet;
  const std::string_view& getView() const {
    return view;
  }
};

const std::string PLOP_TOTO("Toto");

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
anch::json::registerObject(ObjectMapper<Toto>& mapper) {
  std::cout << "Register Toto fields" << std::endl;
  mapper
    .registerField("plop", &Toto::plop)
    .registerField<std::string>("plip", &Toto::plip)
    .registerField("plap", &Toto::plap)
    .registerField("plup", &Toto::plup)
    .registerField("tata", &Toto::tata)
    .registerField("plep", &Toto::plep)
    .registerField("plyp", &Toto::plyp)
    .registerField("lplyp", &Toto::lplyp)
    .registerField("self", &Toto::self)
    .registerField<std::string>("invisible", &Toto::empty)
    .registerField("ptr", &Toto::ptr)
    .registerField("null", &Toto::null)
    .registerField("class", std::function<std::string(const Toto&)>(&Toto::getClass))
    ;
  std::cout << "Toto fields registered" << std::endl;
}

template<>
void
anch::json::registerObject(ObjectMapper<Tata>& mapper) {
  std::cout << "Register Tata fields" << std::endl;
  mapper
    .registerField("ploum", &Tata::ploum)
    .registerField("view", std::function<const std::string_view&(const Tata&)>(&Tata::getView))
    .registerField<uint64_t>("num_set", &Tata::numSet)
    .registerField<std::string>("str_vector", &Tata::strVect)
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
  inline void setValue(const std::string& value) {
    _value = value;
  }
};

template<>
void
anch::json::registerObject(ObjectMapper<Test>& mapper) {
  mapper
    .registerField("id", &Test::_id)
    //.registerField("value", std::function<std::string(const Test&)>(&Test::getValue))
    .registerField("value", std::function<std::string(const Test&)>(&Test::getValue), std::function<void(Test&, const std::string&)>(&Test::setValue))
    //.registerField("value", std::function<void(Test&, const std::string&)>(&Test::setValue))
    .registerField<int32_t>("nums", &Test::_nums)
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
    toto.plep = 2.2f;
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
  {
    Test test = {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    };
    std::ostringstream oss;
    anch::json::serialize(test, oss);
    std::cout << "Serialized test: " << oss.str() << std::endl;
    std::string res = anch::json::serialize(test);
    std::cout << "Serialized test as string: " << res << std::endl;
    //std::cout << "Serialized test iomanip: " << anch::json::json_format << test << std::endl;
  }
  {
    Test test = {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    };
    std::vector<Test> tests = {test, {
	._id = "44666aab-0b63-47a1-80bb-ae84bc844289",
	._value = "this is another value",
	._nums = {5,6,7,8}
      }
    };
    //std::ostringstream oss;
    std::cout << "Serialized test array: ";
    anch::json::serialize(tests, std::cout);
    std::cout << std::endl;
    //std::cout << "Serialized test array: " << oss.str() << std::endl;
    std::string res = anch::json::serialize(tests);
    std::cout << "Serialized test array as string: " << res << std::endl;
    //std::cout << "Serialized test iomanip: " << anch::json::json_format << test << std::endl;
  }
  {
    Test test;
    std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"nums\":[1,2,3,4]}";
    std::istringstream iss(json);
    try {
      anch::json::deserialize(test, iss);
      //Factory<Test>::getInstance().deserialize(test, iss);
      std::cout << "id=" << test._id << " ; value=" << test._value << std::endl;
    } catch(const int& code) {
      std::cerr << "Fail with code " << code << std::endl;
      return 1;
    }
  }
  {
    std::string json = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"lplyp\":4.4,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"lplyp\":7.7,\"ptr\":\"self\"},\"ptr\":\"plop\"}";
    std::istringstream iss(json);
    Toto toto;
    try {
      anch::json::deserialize(toto, iss);
      //std::cout << "id=" << test._id << " ; value=" << test._value << std::endl;
    } catch(const std::bad_cast& e) {
      std::cerr << "Bad cast " << e.what() << std::endl;
      return 1;
    } catch(const int& code) {
      std::cerr << "Fail with code " << code << std::endl;
      return 1;
    }
  }
  {
    std::ifstream iss("toto.json");
    //Toto toto;
    try {
      Toto toto = anch::json::deserialize<Toto>(iss);
      //std::cout << "id=" << test._id << " ; value=" << test._value << std::endl;
    } catch(const std::bad_cast& e) {
      std::cerr << "Bad cast " << e.what() << std::endl;
      return 1;
    } catch(const int& code) {
      std::cerr << "Fail with code " << code << std::endl;
      return 1;
    }
  }
  {
    std::ifstream iss("totos.json");
    //Toto toto;
    try {
      std::vector<Test> tests;
      anch::json::deserialize<Test>(tests, iss);
      //Factory<Test>::getInstance().deserialize(tests, iss);
      //Toto toto = anch::json::deserialize<Toto>(iss);
      //std::cout << "id=" << test._id << " ; value=" << test._value << std::endl;
    } catch(const std::bad_cast& e) {
      std::cerr << "Bad cast " << e.what() << std::endl;
      return 1;
    } catch(const int& code) {
      std::cerr << "Fail with code " << code << std::endl;
      return 1;
    }
  }
  std::cout << "Exit serialization tests" << std::endl;
  return 0;
}
