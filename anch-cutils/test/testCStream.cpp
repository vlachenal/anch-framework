#include "cutils/iostream.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

#include "json/json.hpp"
#include "ut/unit.hpp"
#include "ut/assert.hpp"

using anch::cutils::cbuffer;
using anch::cutils::CIStream;
using anch::cutils::COStream;
using anch::cutils::CIOStream;

struct Test {
  std::string toto;
  int titi;
};

template<>
void
anch::json::registerObject<Test>(anch::json::ObjectMapper<Test>& mapper) {
  mapper
    .registerField("toto", &Test::toto)
    .registerField("titi", &Test::titi);
}

const std::string JSON("{\"toto\":\"toto\",\"titi\":42}");

// Write function which collects data (at most maxChunk bytes per call)
struct Sink {
  std::string data;
  std::size_t maxChunk;
  std::size_t calls = 0;

  explicit Sink(std::size_t chunk = std::string::npos): data(), maxChunk(chunk) {}

  std::size_t write(const char* buffer, std::size_t size) {
    ++calls;
    std::size_t nb = std::min(size, maxChunk);
    data.append(buffer, nb);
    return nb;
  }
};

// Read function which provides data (at most maxChunk bytes per call)
struct Source {
  std::string data;
  std::size_t offset = 0;
  std::size_t maxChunk;

  explicit Source(std::string content = "", std::size_t chunk = std::string::npos): data(std::move(content)), maxChunk(chunk) {}

  std::size_t read(char* buffer, std::size_t size) {
    std::size_t nb = std::min({size, maxChunk, data.size() - offset});
    data.copy(buffer, nb, offset);
    offset += nb;
    return nb;
  }
};

cbuffer
makeBuffer(std::size_t size, Source* source, Sink* sink) {
  cbuffer buffer;
  buffer.size = size;
  if(source != nullptr) {
    buffer.read = std::bind_front(&Source::read, source);
  }
  if(sink != nullptr) {
    buffer.write = std::bind_front(&Sink::write, sink);
  }
  return buffer;
}

void
testSerialize(std::size_t size) {
  Test test = {.toto = "toto", .titi = 42};
  Sink sink;
  {
    COStream cos(makeBuffer(size, nullptr, &sink));
    anch::json::serialize(test, cos);
  } // pending data are sent on destruction
  anch::ut::assertThat(sink.data == JSON, "Serialized data is '{}'", sink.data);
}

void
testDeserialize(std::size_t size) {
  Source source(JSON);
  CIStream cis(makeBuffer(size, &source, nullptr));
  Test test;
  anch::json::deserialize(test, cis);
  anch::ut::assertThat(test.toto == "toto" && test.titi == 42, "Deserialized object is {}/{}", test.toto, test.titi);
}

// CU1: in input-output mode, reading overwrote written data which had not been sent
void
testRequestResponse(std::size_t size) {
  Sink sink;
  Source source("REPLY-FROM-PEER");
  std::string sentBeforeRead;
  cbuffer buffer = makeBuffer(size, nullptr, &sink);
  buffer.read = [&source, &sink, &sentBeforeRead](char* data, std::size_t len) {
    if(sentBeforeRead.empty()) {
      sentBeforeRead = sink.data;
    }
    return source.read(data, len);
  };
  CIOStream io(buffer);
  io << "HELLO"; // no flush
  std::string word;
  io >> word;
  io.flush();
  anch::ut::assertThat(word == "REPLY-FROM-PEER", "Read word is '{}'", word);
  anch::ut::assertThat(sink.data == "HELLO", "Sent data is '{}'", sink.data);
  anch::ut::assertThat(sentBeforeRead == "HELLO", "Request has not been sent before reading response ('{}')", sentBeforeRead);
}

void
testInOutJson(std::size_t size) {
  Sink sink;
  Source source(JSON);
  CIOStream io(makeBuffer(size, &source, &sink));
  Test test = {.toto = "toto", .titi = 42};
  anch::json::serialize(test, io);
  io.flush();
  Test read;
  anch::json::deserialize(read, io);
  anch::ut::assertThat(sink.data == JSON, "Serialized data is '{}'", sink.data);
  anch::ut::assertThat(read.toto == "toto" && read.titi == 42, "Deserialized object is {}/{}", read.toto, read.titi);
}

// CU2: partial writes were lost
void
testPartialWrites() {
  Sink sink(3);
  std::string expected(100, 'x');
  for(std::size_t i = 0 ; i < expected.size() ; ++i) {
    expected[i] = static_cast<char>('a' + i % 26);
  }
  {
    COStream cos(makeBuffer(16, nullptr, &sink));
    cos << expected;
    cos.flush();
    anch::ut::assertTrue(cos.good(), "Stream is in error");
  }
  anch::ut::assertThat(sink.data == expected, "Written data is '{}'", sink.data);
}

// CU2: write errors were ignored
void
testWriteError() {
  cbuffer buffer;
  buffer.size = 4;
  buffer.write = [](const char*, std::size_t) -> std::size_t { return 0; };
  COStream cos(buffer);
  cos << "hello world";
  cos.flush();
  anch::ut::assertTrue(cos.bad(), "Write error has not been reported");
}

void
testWriteException() {
  cbuffer buffer;
  buffer.size = 4;
  buffer.write = [](const char*, std::size_t) -> std::size_t { throw std::runtime_error("network error"); };
  COStream cos(buffer);
  cos << "hello world";
  cos.flush();
  anch::ut::assertTrue(cos.bad(), "Write exception has not been reported");
}

void
testReadEnd() {
  Source source("a b", 1);
  CIStream cis(makeBuffer(2, &source, nullptr));
  std::string a;
  std::string b;
  std::string c;
  cis >> a >> b;
  anch::ut::assertTrue(a == "a" && b == "b", "Unexpected words");
  cis >> c;
  anch::ut::assertTrue(cis.eof() && c.empty(), "End of stream has not been reached");
}

// Check that function raises std::invalid_argument with message
template<typename F>
void
checkInvalid(F function, const std::string& message) {
  try {
    function();
    anch::ut::fail("No error has been raised (expected '" + message + "')");
  } catch(const std::invalid_argument& e) {
    anch::ut::assertThat(message == e.what(), "Error message is '{}' instead of '{}'", e.what(), message);
  }
}

// CU5: setBuffer messages were inverted ; size 0 used to loop forever
void
testInvalidBuffers() {
  Sink sink;
  Source source;
  checkInvalid([&]() { CIStream cis(makeBuffer(4, nullptr, &sink)); }, "input stream and null read function");
  checkInvalid([&]() { COStream cos(makeBuffer(4, &source, nullptr)); }, "output stream and null write function");
  checkInvalid([&]() { CIOStream cios(makeBuffer(4, &source, nullptr)); }, "output stream and null write function");
  checkInvalid([&]() { COStream cos(makeBuffer(0, nullptr, &sink)); }, "buffer size has to be greater than 0");
}

// Stream which sets its buffer after construction (as TcpStream)
class LateStream: public COStream {
public:
  LateStream(): COStream() {}
  void set(const cbuffer& buffer) {
    setBuffer(buffer);
  }
};

// CU4: default constructor left the buffer uninitialized ; a second setBuffer leaked the first buffer
void
testSetBufferTwice() {
  Sink first;
  Sink second;
  {
    LateStream stream;
    stream << "lost"; // no buffer: nothing written, stream in error
    anch::ut::assertTrue(stream.bad(), "Write without buffer has not been reported");
    stream.set(makeBuffer(8, nullptr, &first));
    stream << "one";
    stream.set(makeBuffer(8, nullptr, &second)); // pending data are sent with the first function
    stream << "two";
  }
  anch::ut::assertThat(first.data == "one", "First sink contains '{}'", first.data);
  anch::ut::assertThat(second.data == "two", "Second sink contains '{}'", second.data);
}

void
testProvidedData() {
  char data[4];
  Source source(JSON);
  cbuffer buffer = makeBuffer(sizeof(data), &source, nullptr);
  buffer.data = data;
  CIStream cis(buffer);
  Test test;
  anch::json::deserialize(test, cis);
  anch::ut::assertThat(test.toto == "toto" && test.titi == 42, "Deserialized object is {}/{}", test.toto, test.titi);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH CStream unit tests")
    .description("Test AnCH CStream unit tests library")
    .add("cstream-output-jsonser", std::bind(&testSerialize, 4))
    .add("cstream-output-jsondeser", std::bind(&testDeserialize, 4))
    .add("cstream-output-jsonser-1char", std::bind(&testSerialize, 1))
    .add("cstream-output-jsondeser-1char", std::bind(&testDeserialize, 1))
    .add("cstream-output-jsonser-1500char", std::bind(&testSerialize, 1500))
    .add("cstream-output-jsondeser-1500char", std::bind(&testDeserialize, 1500))
    .add("cstream-output-jsonser-25char", std::bind(&testSerialize, 25))
    .add("cstream-output-jsondeser-25char", std::bind(&testDeserialize, 25))
    .add("cstream-inout-json-1500char", std::bind(&testInOutJson, 1500))
    .add("cstream-inout-json-1char", std::bind(&testInOutJson, 1))
    .add("cstream-inout-json-4char", std::bind(&testInOutJson, 4))
    .add("cstream-request-response-1char", std::bind(&testRequestResponse, 1))
    .add("cstream-request-response-4char", std::bind(&testRequestResponse, 4))
    .add("cstream-request-response-1500char", std::bind(&testRequestResponse, 1500))
    .add("cstream-partial-writes", testPartialWrites)
    .add("cstream-write-error", testWriteError)
    .add("cstream-write-exception", testWriteException)
    .add("cstream-read-end", testReadEnd)
    .add("cstream-invalid-buffers", testInvalidBuffers)
    .add("cstream-set-buffer-twice", testSetBufferTwice)
    .add("cstream-provided-data", testProvidedData)
    ;
}
