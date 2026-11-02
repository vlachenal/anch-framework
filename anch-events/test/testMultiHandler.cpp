#include "events/observable.hpp"
#include "events/eventHandler.hpp"

#include <string>
#include <vector>

#include "ut/unit.hpp"
#include "ut/assert.hpp"

using anch::events::Observable;
using anch::events::EventHandler;


/*!
 * Some event class
 */
class Message {
private:
  std::string _message;
public:
  Message(const std::string& message): _message(message) {}
  inline const std::string& getMessage() const {
    return _message;
  }
};

/*!
 * Event handler interface for \ref Message event.
 */
template<typename D>
class MessageEventHandler: public EventHandler<Message,D> {
public:
  MessageEventHandler(): EventHandler<Message,D>(&D::onMessage) {}
  virtual void onMessage(const anch::events::Event<Message>& event) noexcept = 0;
};

/*!
 * \ref Message event handler
 */
class SingleMsgHandler: public MessageEventHandler<SingleMsgHandler> {
public:
  SingleMsgHandler(): MessageEventHandler<SingleMsgHandler>() {}
  std::vector<std::string> received;
  virtual void onMessage(const anch::events::Event<Message>& event) noexcept {
    received.push_back(event.body.getMessage());
  }
};


/*!
 * \ref string event handler interface.
 */
template<typename D>
class StringEventHandler: public EventHandler<std::string,D> {
public:
  StringEventHandler(): EventHandler<std::string,D>(&D::onString) {}
  virtual void onString(const anch::events::Event<std::string>& event) noexcept = 0;
};

/*!
 * \ref string and \ref Message events handler.
 */
class MultiEventHandler: public MessageEventHandler<MultiEventHandler>, public StringEventHandler<MultiEventHandler> {
public:
  std::vector<std::string> messages;
  std::vector<std::string> strings;
  MultiEventHandler(): MessageEventHandler<MultiEventHandler>(), StringEventHandler<MultiEventHandler>() {}
  virtual void onMessage(const anch::events::Event<Message>& event) noexcept {
    messages.push_back(event.body.getMessage());
  }
  virtual void onString(const anch::events::Event<std::string>& event) noexcept {
    strings.push_back(event.body);
  }
};

void
testMultiHandler() {
  SingleMsgHandler handler;
  MultiEventHandler multi;
  Observable<Message> observable;
  observable.addObserver(handler);
  observable.addObserver(multi);
  Observable<std::string> strObs;
  strObs.addObserver(multi);
  strObs.notifyObservers("tata");
  observable.notifyObservers(Message("toto"));
  anch::ut::assertTrue(handler.received == std::vector<std::string>{"toto"}, "Single handler has not received message");
  anch::ut::assertTrue(multi.messages == std::vector<std::string>{"toto"}, "Multi handler has not received message");
  anch::ut::assertTrue(multi.strings == std::vector<std::string>{"tata"}, "Multi handler has not received string");
  observable.removeObserver(handler);
  observable.removeObserver(multi);
  strObs.removeObserver(multi);
}

void
anch::ut::setup(anch::ut::UnitTests& tests) {
  tests
    .name("AnCH event handlers unit tests")
    .description("Test AnCH event handler with several event types")
    .add("multi-handler", testMultiHandler)
    ;
}
