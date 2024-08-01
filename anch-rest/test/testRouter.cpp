#include <iostream>
#include <map>
#include <functional>
#include <csignal>
#include <sstream>
#include <fstream>

#include "rest/router.hpp"
#include "rest/response.hpp"
#include "rest/constants.hpp"
#include "json/json.hpp"
#include "uuid.hpp"

#include "ut/assert.hpp"
#include "ut/unit.hpp"

using anch::rest::Router;
using anch::rest::Request;
using anch::rest::Response;
using anch::rest::Verb;
using anch::rest::StatusCode;
using anch::rest::MediaType;
using anch::json::ObjectMapper;
using anch::UUID;


auto&
anch::rest::castMapper(const std::string& contentType, std::any& mapper) {
  if(contentType == anch::rest::MediaType::APP_JSON) {
    return std::any_cast<anch::json::JSONMapper&>(mapper);
  }
  std::ostringstream out;
  out << contentType << " has not been defined in mapper caster function";
  throw anch::rest::MapperException(out.str());
}

void
noCoredumpOnAbort(int) {
  std::_Exit(EXIT_FAILURE);
}


std::map<std::string, std::function<void()>> TEST_SUITE;
Router router;

Response
GET_api(const Request&) {
  std::cout << "GET_api" << std::endl;
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).body("GET_api").build();
}

void
testNoParam() {
  std::cout << "Enter in testNoParam" << std::endl;
  Request req;
  req.setPath("/api");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::APP_JSON });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  std::cout << "Exit testNoParam" << std::endl;
}

Response
GET_api_id(const Request&, std::string id) {
  std::ostringstream oss;
  oss << "GET_api_" << id;
  std::string body(oss.str());
  std::cout << body << std::endl;
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).body(body.data()).build();
}

void
testPathParam() {
  std::cout << "Enter in testPathParam" << std::endl;
  Request req;
  req.setPath("/api/plop");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::APP_JSON });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  std::cout << "Exit testPathParam" << std::endl;
}

Response
GET_api_int_id(const Request&, int id) {
  std::ostringstream oss;
  oss << "GET_api_int_" << id;
  std::string body(oss.str());
  std::cout << body << std::endl;
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).body(body.data()).build();
}

void
testPathIntParam() {
  std::cout << "Enter in testPathIntParam" << std::endl;
  Request req = Request::builder()
    .verb(Verb::GET).path("/api/int/10")
    .accept({ MediaType::APP_JSON })
    .build();
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  std::cout << "Exit testPathIntParam" << std::endl;
}

void
testPathIntParamKO() {
  std::cout << "Enter in testPathIntParamKO" << std::endl;
  Request req;
  req.setPath("/api/int/plop");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::APP_JSON });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::BAD_REQUEST));
  std::cout << "Exit testPathIntParamKO" << std::endl;
}

Response
GET_api_view_id(const Request&, std::string_view id) {
  std::ostringstream oss;
  oss << "GET_api_view_" << id;
  std::string body(oss.str());
  std::cout << body << std::endl;
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).body(body.data()).build();
}

void
testPathViewParam() {
  std::cout << "Enter in testPathViewParam" << std::endl;
  Request req;
  req.setPath("/api/view/10");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::APP_JSON });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  std::cout << "Exit testPathViewParam" << std::endl;
}

Response
GET_api_error_id(const Request&, std::string_view id) {
  std::ostringstream oss;
  oss << "GET_api_errors_" << id;
  std::string body(oss.str());
  std::cout << body << std::endl;
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).body(body.data()).build();
}

void
testPathErrorMethod() {
  std::cout << "Enter in testPathErrorMethod" << std::endl;
  Request req;
  req.setPath("/api/errors/10");
  req.setVerb(Verb::POST);
  req.setAccept({ MediaType::APP_JSON });
  req.setContentType({ MediaType::APP_JSON });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::METHOD_NOT_ALLOWED));
  std::cout << "Exit testPathErrorMethod" << std::endl;
}

void
testPathErrorAccept() {
  std::cout << "Enter in testPathErrorAccept" << std::endl;
  Request req;
  req.setPath("/api/errors/10");
  req.setVerb(Verb::GET);
  req.setAccept({ "image/png" });
  req.setContentType({ MediaType::APP_JSON });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::NOT_ACCEPTABLE));
  std::cout << "Exit testPathErrorAccept" << std::endl;
}

void
testPathErrorContent() {
  std::cout << "Enter in testPathErrorContent" << std::endl;
  Request req;
  req.setPath("/api/errors/10");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::APP_JSON });
  req.setContentType({ "image/png" });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::UNSUPPORTED_MEDIA_TYPE));
  std::cout << "Exit testPathErrorContent" << std::endl;
}

void
testPathErrorNotFound() {
  std::cout << "Enter in testPathErrorNotFound" << std::endl;
  Request req;
  req.setPath("/api/errors/plop/toto");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::APP_JSON });
  req.setContentType({ MediaType::APP_JSON });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::NOT_FOUND));
  std::cout << "Exit testPathErrorNotFound" << std::endl;
}

Response
GET_api_uuid(const Request&, UUID id) {
  std::ostringstream oss;
  oss << "GET_api_uuid_" << id;
  std::string body(oss.str());
  std::cout << body << std::endl;
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).body(body.data()).build();
}

void
testPathUUID() {
  std::cout << "Enter in testPathUUID" << std::endl;
  Request req;
  req.setPath("/api/uuid/a8b8bfcb-de16-4abf-a291-9d550a2317bc");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::NONE });
  req.setContentType({ MediaType::NONE });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  std::cout << "Exit testPathUUID" << std::endl;
}

void
testPathUUIDko() {
  std::cout << "Enter in testPathUUIDko" << std::endl;
  Request req;
  req.setPath("/api/uuid/plop");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::NONE });
  req.setContentType({ MediaType::NONE });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::BAD_REQUEST));
  std::cout << "Exit testPathUUIDko" << std::endl;
}

Response
GET_api_multi_params(const Request&, std::string_view str, uint32_t id, UUID uuid) {
  std::ostringstream oss;
  oss << "GET_api_multi_params_" << str << '_' << id << '_' << uuid;
  std::string body(oss.str());
  std::cout << body << std::endl;
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).body(body.data()).build();
}

void
testPathMultiOK() {
  std::cout << "Enter in testPathMultiOK" << std::endl;
  Request req;
  req.setPath("/api/multi/view/plop/int/10/uuid/700e1f7c-ff97-434c-8810-18c0ded1cad9");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::NONE });
  req.setContentType({ MediaType::NONE });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  std::cout << "Exit testPathMultiOK" << std::endl;
}

void
testPathMultiLambdaOK() {
  std::cout << "Enter in testPathMultiLambdaOK" << std::endl;
  Request req;
  req.setPath("/api/lambda/view/plip/int/42/uuid/700e1f7c-ff97-434c-8810-18c0ded1cad9");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::NONE });
  req.setContentType({ MediaType::NONE });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  std::cout << "Exit testPathMultiLambdaOK" << std::endl;
}

struct Toto {
  std::string message;
};

template<>
void
anch::json::registerObject(ObjectMapper<Toto>& mapper) {
  std::cout << "Register Toto fields" << std::endl;
  mapper
    .registerField("message", &Toto::message)
    ;
  std::cout << "Toto fields registered" << std::endl;
}

Response
GET_api_json(const Request&) {
  Toto toto = {
    .message = "GET_api_json"
  };
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).contentType(MediaType::APP_JSON).body(toto).build();
}

void
testResponseOKJSON() {
  std::cout << "Enter in testResponseOKJSON" << std::endl;
  Request req;
  req.setPath("/api/res-json/plop");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::APP_JSON });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  res.writeBody(std::cout);
  std::cout << std::endl;
  std::cout << "Exit testResponseOKJSON" << std::endl;
}

Response
GET_api_istream(const Request&) {
  std::ifstream is("/home/winz/projects/anch-framework/anch-rest/build/Makefile");
  return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).contentType(MediaType::TXT_PLAIN).body(std::move(is)).build();
}

void
testResponseOKistream() {
  std::cout << "Enter in testResponseOKistream" << std::endl;
  Request req;
  req.setPath("/api/res-istream");
  req.setVerb(Verb::GET);
  req.setAccept({ MediaType::ALL });
  std::cout << "Request initialized" << std::endl;
  Response res = router.execute(req);
  std::cout << "Method executed: " << res.getStatus() << std::endl;
  anch::ut::assertTrue(res.getStatus() == static_cast<uint16_t>(StatusCode::OK));
  res.writeBody(std::cout);
  std::cout << std::endl;
  std::cout << "Exit testResponseOKistream" << std::endl;
}

void
beforeAll() {
  std::cout << "Enter in router unit tests" << std::endl;

  try {
    anch::rest::GlobalMapperRegistry::getInstance()
      .registerMapper(MediaType::APP_JSON, anch::json::JSONMapper(anch::json::DEFAULT_MAPPING_OPTIONS));
    router
      .add(std::function<Response(const Request&)>(GET_api), "/api", Verb::GET, {}, { MediaType::APP_JSON })
      .add(std::function<Response(const Request&, std::string)>(GET_api_id), "/api/{}", Verb::GET, {}, { MediaType::APP_JSON })
      .add(std::function<Response(const Request&, int)>(GET_api_int_id), "/api/int/{}", Verb::GET, {}, { MediaType::APP_JSON })
      .add(std::function<Response(const Request&, std::string_view)>(GET_api_view_id), "/api/view/{}", Verb::GET, {}, { MediaType::APP_JSON })
      .add(std::function<Response(const Request&, std::string_view)>(GET_api_error_id), "/api/errors/{}", Verb::GET, { MediaType::APP_JSON }, { MediaType::APP_JSON })
      .add(std::function<Response(const Request&, UUID)>(GET_api_uuid), "/api/uuid/{}", Verb::GET, {}, {})
      .add(std::function<Response(const Request&, std::string_view, uint32_t, UUID)>(GET_api_multi_params), "/api/multi/view/{}/int/{}/uuid/{}", Verb::GET, {}, {})
      .add(std::function<Response(const Request&, std::string_view, uint32_t, UUID)>([](const Request&, std::string_view str, uint32_t id, UUID uuid) -> Response {
      	std::ostringstream oss;
      	oss << "GET_api_lambda_params_" << str << '_' << id << '_' << uuid;
      	std::string body(oss.str());
      	std::cout << body << std::endl;
      	return Response::builder().status(static_cast<uint16_t>(StatusCode::OK)).body(body.data()).build();
      }), "/api/lambda/view/{}/int/{}/uuid/{}", Verb::GET, {}, {})
      .add(std::function<Response(const Request&)>(GET_api_json), "/api/res-json/plop", Verb::GET, {}, { MediaType::APP_JSON })
      .add(std::function<Response(const Request&)>(GET_api_istream), "/api/res-istream", Verb::GET, {}, { MediaType::TXT_PLAIN })
      ;
  } catch(const std::exception& e) {
    std::ostringstream oss;
    oss << "Error while registering routes: " << e.what();
    anch::ut::fail(oss.str());
  } catch(...) {
    anch::ut::fail("Error while registering routes");
  }

  auto prevHandler = std::signal(SIGABRT, noCoredumpOnAbort);
  if(prevHandler == SIG_ERR) {
    std::cerr << "Setup failed\n";
    std::_Exit(EXIT_FAILURE);
  }
}

void
afterAll() {
  std::cout << "Exit router unit tests" << std::endl;
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH router unit tests")
    .description("Test AnCH router library")
    .initialize(beforeAll)
    .uninitialize(afterAll)
    .add("test_0_param_no_body_ok", testNoParam)
    .add("test_1_param_no_body_ok", testPathParam)
    .add("test_1_int_param_no_body_ok", testPathIntParam)
    .add("test_1_int_param_no_body_ko", testPathIntParamKO)
    .add("test_1_view_param_no_body_ok", testPathViewParam)
    .add("test_error_405", testPathErrorMethod)
    .add("test_error_406", testPathErrorAccept)
    .add("test_error_415", testPathErrorContent)
    .add("test_error_404", testPathErrorNotFound)
    .add("test_uuid_ok", testPathUUID)
    .add("test_uuid_ko", testPathUUIDko)
    .add("test_multi_ok", testPathMultiOK)
    .add("test_lambda_ok", testPathMultiLambdaOK)
    .add("test_res_json_ok", testResponseOKJSON)
    .add("test_res_istream_ok", testResponseOKistream)
    ;
}
