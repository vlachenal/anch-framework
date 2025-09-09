#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>

#include "json/json.hpp"
#include "ut/assert.hpp"
#include "ut/unit.hpp"


using anch::json::ObjectMapper;
using anch::json::Factory;
using anch::json::MappingError;
using anch::json::ErrorCode;
using anch::json::JSONMapper;


template<typename T>
std::ostream&
operator<<(std::ostream& os, const std::vector<T>& array) {
  os << '[';
  bool first = true;
  for(const T& item: array) {
    if(!first) {
      os << ", " << item;
    } else {
      os << item;
      first = false;
    }
  }
  os << ']';
  return os;
}

template<typename T>
std::ostream&
operator<<(std::ostream& os, const std::list<T>& array) {
  os << '[';
  bool first = true;
  for(const T& item: array) {
    if(!first) {
      os << ", " << item;
    } else {
      os << item;
      first = false;
    }
  }
  os << ']';
  return os;
}

template<typename T>
std::ostream&
operator<<(std::ostream& os, const std::set<T>& array) {
  os << '[';
  bool first = true;
  for(const T& item: array) {
    if(!first) {
      os << ", " << item;
    } else {
      os << item;
      first = false;
    }
  }
  os << ']';
  return os;
}

template<typename T>
std::ostream&
operator<<(std::ostream& os, const std::optional<T>& opt) {
  if(!opt.has_value()) {
    os << "std::optional->empty";
  } else {
    os << opt.value();
  }
  return os;
}

struct Tata {
  std::string ploum;
  std::string_view view;
  std::list<Tata> selfList;
  std::vector<std::string> strVect;
  std::set<uint64_t> numSet;
  const std::string_view& getView() const {
    return view;
  }
  bool operator==(const Tata& tata) const {
    return  anch::ut::equals("Tata::ploum", ploum, tata.ploum)
      && anch::ut::equals("Tata::selfList", selfList, tata.selfList)
      && anch::ut::equals("Tata::strVect", strVect, tata.strVect)
      && anch::ut::equals("Tata::numSet", numSet, tata.numSet)
      ;
  }
};

std::ostream&
operator<<(std::ostream& os, const Tata& tata) {
    os << "{ploum=" << tata.ploum
       << ",view=" << tata.view
       << ",selfList=" << tata.selfList
       << ",strVect=" << tata.strVect
       << ",numSet=" << tata.numSet << '}'
      ;
    return os;
}

const std::string PLOP_TOTO("Toto");

struct Toto {
  std::string plop;
  std::optional<std::string> plip;
  uint64_t plap;
  bool plup;
  Tata tata;
  float plep;
  double plyp;
  //long double lplyp;
  std::shared_ptr<Toto> self = NULL;
  std::optional<std::string> empty;
  std::shared_ptr<std::string> ptr;
  std::string* null = NULL;
  std::string getClass() const {
    return "Toto";
  }
  bool operator==(const Toto& toto) const {
    return anch::ut::equals("Toto::plop", plop, toto.plop)
      && anch::ut::equals("Toto::plip", plip, toto.plip)
      && anch::ut::equals("Toto::plap", plap, toto.plap)
      && anch::ut::equals("Toto::plup", plup, toto.plup)
      && anch::ut::equals("Toto::plep", plep, toto.plep)
      && anch::ut::equals("Toto::plyp", plyp, toto.plyp)
      //&& anch::ut::equals("Toto::lplyp", lplyp, toto.lplyp)
      && anch::ut::equals("Toto::self", self, toto.self)
      && anch::ut::equals("Toto::ptr", ptr, toto.ptr)
      && anch::ut::equalsPtr("Toto::null", null, toto.null)
      && anch::ut::equals("Toto::empty", empty, toto.empty)
      && anch::ut::equals("Toto::tata", tata, toto.tata)
      ;
  }
};

std::ostream&
operator<<(std::ostream& os, const Toto& toto) {
  std::ostringstream selfOss;
  if(toto.self == NULL) {
    selfOss << "NULL";
  } else {
    selfOss << *toto.self;
  }
  os << "{plop=" << toto.plop
     << ",plip=" << toto.plip
     << ",plap=" << toto.plap
     << ",plup=" << toto.plup
     << ",plep=" << toto.plep
     << ",plyp=" << toto.plyp
    //<< ",lplyp=" << toto.lplyp
     << ",self=" << selfOss.str()
     << ",ptr=" << (toto.ptr != NULL ? *toto.ptr : std::string("NULL"))
     << ",null=" << (toto.null != NULL ? *toto.null : std::string("NULL"))
     << ",empty=" << toto.empty
     << ",tata=" << toto.tata << '}'
    ;
  return os;
}

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
    //.registerField("lplyp", &Toto::lplyp)
    .registerField<Toto>("self", &Toto::self)
    .registerField<std::string>("invisible", &Toto::empty)
    .registerField<std::string>("ptr", &Toto::ptr)
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
  bool operator==(const Test& test) const {
    return anch::ut::equals("Test::id", _id, test._id)
      && anch::ut::equals("Test::value", _value, test._value)
      && anch::ut::equals("Test::nums", _nums, test._nums)
      ;
    // return _id == test._id
    //   && _value == test._value
    //   && _nums == test._nums
    //   ;
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

namespace plop {
  struct json_proxy {
    explicit json_proxy(std::ostream & os):os(os){}

    template<typename T>
    friend std::ostream & operator<<(const json_proxy& q,
				     const T& rhs) {
      anch::json::serialize(rhs, q.os);
      return q.os;
    }

  private:
    std::ostream & os;
  };

  struct json_creator { } jsonify;
  json_proxy operator<<(std::ostream& os, json_creator) {
    return json_proxy(os);
  }
}

Toto toto;
//Toto* self = NULL;
std::string VIEW = "VIEW";
//const std::string totoRes = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"view\":\"VIEW\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"lplyp\":4.4,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"view\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"lplyp\":7.7,\"ptr\":\"self\",\"class\":\"Toto\"},\"ptr\":\"plop\",\"class\":\"Toto\"}";
const std::string totoRes = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"view\":\"VIEW\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"view\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"ptr\":\"self\",\"class\":\"Toto\"},\"ptr\":\"plop\",\"class\":\"Toto\"}";

Test res = {
  ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
  ._value = "",
  ._nums = {1,2,3,4}
};

void
beforeAll() {
  std::cout << "Enter in JSON mapper unit tests" << std::endl << std::endl;
  {
    std::shared_ptr<Toto> self = std::make_shared<Toto>();
    self->plop = "self";
    self->plip = std::optional<std::string>("self_plip");
    self->plap = 24;
    self->plup = true;
    self->plep = static_cast<float>(5.5);
    self->plyp = 6.6;
    //self->lplyp = 7.7;
    self->empty = std::optional<std::string>();
    self->self = NULL;
    self->ptr = std::make_shared<std::string>(self->plop);
    self->null = NULL;

    toto.plop = "plop";
    toto.plip = std::optional<std::string>("plip");
    toto.plap = 42;
    toto.plup = false;
    toto.plep = 2.2f;
    toto.plyp = 3.3;
    //toto.lplyp = 4.4;
    toto.empty = std::optional<std::string>();
    toto.ptr = std::make_shared<std::string>(toto.plop);
    toto.null = NULL;
    toto.self = self;
    Tata tata;
    tata.ploum = "ploum";
    tata.view = std::string_view(VIEW.data());
    tata.numSet = {1, 2, 3};
    tata.strVect = {"4","5","6"};
    toto.tata = tata;
  }
}

void
testFullSerDefaultOptions() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  std::ostringstream oss;
  std::cout << "Serialize Toto" << std::endl;
  mapper.serialize(toto, oss);
  std::string res1 = oss.str();
  std::cout << "Serialized toto: " << res1 << std::endl;
  std::cout << "Serialize Toto as string" << std::endl;
  std::string res2 = mapper.serialize(toto);
  std::cout << "Serialized toto as string: " << res2 << std::endl;
  anch::ut::assert(res1 == res2, "Serialization results should be equals");
  anch::ut::assert(totoRes == res1, "Result should be equals to: {}", totoRes);
}

void
testDeserList() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assert(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testDeserUnknownFieldStructFail() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":1234,\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown field and default mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);
    std::ostringstream oss;
    oss << "This test should have failed" << std::endl;
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    if(error.getErrorCode() == ErrorCode::UNEXPECTED_FIELD) {
      std::cout << "Fail OK with error " << error.what() << std::endl;
    } else {
      std::ostringstream oss;
      oss << "Fail with " << error.what() << std::endl;
      anch::ut::fail(oss.str());
    }
  }
}

void
testDeserUnknownFieldStructOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":{\"tot\":{}},\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown object and custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assert(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testDeserUnknownFieldOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":\"toto\",\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown string and custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assert(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testDeserUnknownFieldIntOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":1234,\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown integer and custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assert(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  std::cout << "OK" << std::endl;
}

void
testDeserUnknownFieldEmptyColOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[],\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown array and custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assert(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  std::cout << "OK" << std::endl;
}

void
testDeserUnknownFielIntColOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[1],\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown array and with raw values custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assert(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  std::cout << "OK" << std::endl;
}

void
testDeserUnknownFielWTFColOK() {
  anch::json::JSONMapper mapper({.deserialize_ignore_unknown_field = true});
  Test test;
  std::string json = "{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"unknown\":[1,{},\"plop\"],\"nums\":[1,2,3,4]}";
  std::cout << "Deserialize with unknown array and with values custom mapper " << json << std::endl;
  std::istringstream iss(json);
  try {
    mapper.deserialize(test, iss);

    std::ostringstream numsTestOss;
    numsTestOss << test._nums;
    std::ostringstream numsResOss;
    numsResOss << res._nums;
    anch::ut::assert(test == res, "Test has been unexpecting deserialized: id {} / {} instead of {} / {}", test._id, numsTestOss.str(), res._id, numsResOss.str());

  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
  std::cout << "OK" << std::endl;
}

void
testFullDeserDefaultOptions() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  std::string json = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"ptr\":\"self\"},\"ptr\":\"plop\"}";
  //  std::string json = "{\"plop\":\"plop\",\"plip\":\"plip\",\"plap\":42,\"plup\":false,\"tata\":{\"ploum\":\"ploum\",\"num_set\":[1,2,3],\"str_vector\":[\"4\",\"5\",\"6\"]},\"plep\":2.2,\"plyp\":3.3,\"lplyp\":4.4,\"self\":{\"plop\":\"self\",\"plip\":\"self_plip\",\"plap\":24,\"plup\":true,\"tata\":{\"ploum\":\"\",\"num_set\":[],\"str_vector\":[]},\"plep\":5.5,\"plyp\":6.6,\"lplyp\":7.7,\"ptr\":\"self\"},\"ptr\":\"plop\"}";
  // {"plop":"plop","plip":"plip","plap":42,"plup":false,"tata":{"ploum":"ploum","num_set":[1,2,3],"str_vector":["4","5","6"]},"plep":2.2,"plyp":3.3,"lplyp":4.4,"self":{"plop":"self","plip":"self_plip","plap":24,"plup":true,"tata":{"ploum":"","num_set":[],"str_vector":[]},"plep":5.5,"plyp":6.6,"lplyp":7.7,"ptr":"self"},"ptr":"plop"}
  // {"plop":"plop","plip":"plip","plap":42,"plup":false,"tata":{"ploum":"ploum","num_set":[1,2,3],"str_vector":["4","5","6"]},"plep":2.2,"plyp":3.3,"lplyp":4.4,"self":{"plop":"self","plip":"self_plip","plap":24,"plup":true,"tata":{"ploum":"","num_set":[],"str_vector":[]},"plep":5.5,"plyp":6.6,"lplyp":7.7,"ptr":"self","class":"Toto"},"ptr":"plop","class":"Toto"}
  Toto expected = toto;

  //std::string& json = totoRes;
  std::cout << "Deserialize " << json << std::endl;
  std::istringstream iss(json);
  Toto totoDeser;
  try {
    anch::json::Factory<Toto>::getInstance();
    std::cout << "Instance OK" << std::endl;
    mapper.deserialize(totoDeser, iss);
    //std::cout << totoDeser.tata.ploum << std::endl;

    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Result:   " << totoDeser << std::endl;
    anch::ut::assert(expected == totoDeser, "NOT EQUALS");

  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testFulDeserDiscard128() {
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
  std::ifstream iss("toto.json");
  std::cout << "Deserialize toto.json" << std::endl;
  try {
    Toto deserToto = mapper.deserialize<Toto>(iss);
    std::cout << deserToto << std::endl;
    anch::ut::assert(toto == deserToto, "NOT EQUALS");
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast  " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testFulDeserDiscard128Col() {
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
  std::ifstream iss("totos.json");
  std::vector<Test> expected = {
    {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    },
    {
      ._id = "44666aab-0b63-47a1-80bb-ae84bc844289",
      ._value = "this is another value",
      ._nums = {5,6,7,8}
    }
  };
  std::cout << "Deserialize totos.json" << std::endl;
  try {
    std::vector<Test> tests;
    mapper.deserialize<Test>(tests, iss);
    anch::ut::assert(expected == tests, "NOT EQUALS");
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testFulDeserDiscard128ColBuf2() {
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128, .buffer_size = 2});
  std::ifstream iss("totos.json");
  std::vector<Test> expected = {
    {
      ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
      ._value = "this is a value",
      ._nums = {1,2,3,4}
    },
    {
      ._id = "44666aab-0b63-47a1-80bb-ae84bc844289",
      ._value = "this is another value",
      ._nums = {5,6,7,8}
    }
  };
  std::cout << "Deserialize totos.json" << std::endl;
  try {
    std::vector<Test> tests;
    mapper.deserialize<Test>(tests, iss);
    anch::ut::assert(expected == tests, "NOT EQUALS");
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
testIOManip() {
  Test test = {
    ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
    ._value = "this is a value",
    ._nums = {1,2,3,4}
  };
  std::cout << "As iomanip: " << plop::jsonify << test << std::endl;
}

struct TestMap {
  std::map<std::string,std::string> _strMap;
  std::map<std::string,bool> _boolMap;
  std::map<std::string,int64_t> _intMap;
  std::map<std::string,double> _doubleMap;
  std::map<std::string,Test> _testMap;
};

template<>
void
anch::json::registerObject(ObjectMapper<TestMap>& mapper) {
  mapper
    .registerField<std::string>("strMap", &TestMap::_strMap)
    .registerField<bool>("boolMap", &TestMap::_boolMap)
    .registerField<int64_t>("intMap", &TestMap::_intMap)
    .registerField<double>("doubleMap", &TestMap::_doubleMap)
    .registerField<Test>("testMap", &TestMap::_testMap)
    ;
}

void
testSerializeMap() {
  anch::json::JSONMapper mapper(anch::json::DEFAULT_MAPPING_OPTIONS);
  TestMap test = {
    ._strMap = {
      {"a","b"},
      {"1","2"},
      {"toto","tata"},
      {"plip","plop"}
    },
    ._boolMap = {
      {"a",true},
      {"1",false},
      {"toto",true},
      {"plip",false}
    },
    ._intMap = {
      {"a",123},
      {"1",456},
      {"toto",789},
      {"plip",123456789}
    },
    ._doubleMap = {
      {"a",3.14158},
      {"1",0.01234},
      {"toto",123.456},
      {"plip",789.123}
    },
    ._testMap = {
      {"test1", {
	  ._id = "deb94ebc-be28-4899-981a-29199b7a487d",
	  ._value = "this is a value",
	  ._nums = {1,2,3,4}
	}
      },
      {"test2", {
	  ._id = "toto",
	  ._value = "this is another value",
	  ._nums = {5,6,7,8}
	}
      }
    }
  };
  std::ostringstream oss;
  std::cout << "Serialize TestMap" << std::endl;
  mapper.serialize(test, oss);
  std::cout << "Serialized test: " << oss.str() << std::endl;
  std::cout << "Serialize TestMap as string" << std::endl;
  std::string res = mapper.serialize(test);
  std::cout << "Serialized test as string: " << res << std::endl;
}

void
testDeserializeMap() {
  std::string json("{\"strMap\":{\"1\":\"2\",\"a\":\"b\",\"plip\":\"plop\",\"toto\":\"tata\"},\"boolMap\":{\"1\":false,\"a\":true,\"plip\":false,\"toto\":true},\"intMap\":{\"1\":456,\"a\":123,\"plip\":123456789,\"toto\":789},\"doubleMap\":{\"1\":0.01234,\"a\":3.14158,\"plip\":789.123,\"toto\":123.456},\"testMap\":{\"test1\":{\"id\":\"deb94ebc-be28-4899-981a-29199b7a487d\",\"value\":\"this is a value\",\"nums\":[1,2,3,4]},\"test2\":{\"id\":\"toto\",\"value\":\"this is another value\",\"nums\":[5,6,7,8]}}}");
  anch::json::JSONMapper mapper({.deserialize_max_discard_char = 128});
  std::cout << "Deserialize TestMap: " << json << std::endl;
  try {
    auto iss = std::istringstream(json);
    TestMap toto = mapper.deserialize<TestMap>(iss);
    std::ostringstream oss;
    mapper.serialize(toto, oss);
    std::cout << "Serialized TestMap:  " << oss.str() << std::endl;
  } catch(const std::bad_cast& e) {
    std::ostringstream oss;
    oss << "Bad cast  " << e.what();
    anch::ut::fail(oss.str());
  } catch(const MappingError& error) {
    std::ostringstream oss;
    oss << "Fail with " << error.what();
    anch::ut::fail(oss.str());
  }
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH JSON mapper unit tests")
    .description("Test AnCH JSON mapper library")
    .initialize(beforeAll)
    .add("json-ser-full", testFullSerDefaultOptions)
    .add("json-deser-list", testDeserList)
    .add("json-deser-unf-field", testDeserUnknownFieldStructFail)
    .add("json-deser-unf-str", testDeserUnknownFieldStructOK)
    .add("json-deser-unf", testDeserUnknownFieldOK)
    .add("json-deser-unf-int", testDeserUnknownFieldIntOK)
    .add("json-deser-unf-empty-col", testDeserUnknownFieldEmptyColOK)
    .add("json-deser-unf-int-col", testDeserUnknownFielIntColOK)
    .add("json-deser-unf-wtf-col", testDeserUnknownFielWTFColOK)
    .add("json-full-deser-default", testFullDeserDefaultOptions)
    .add("json-deser-discard128", testFulDeserDiscard128)
    .add("json-deser-discard128-col", testFulDeserDiscard128Col)
    .add("json-deser-discard128-col-buf2", testFulDeserDiscard128ColBuf2)
    .add("json-iomanip", testIOManip)
    .add("json-ser-map", testSerializeMap)
    .add("json-deser-map", testDeserializeMap)
    ;
}
