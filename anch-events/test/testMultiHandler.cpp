#include "events/observable.hpp"
#include "events/eventHandler.hpp"

#include <iostream>

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
  virtual void onMessage(const anch::events::Event<Message>& event) noexcept {
    std::cout << event.body.getMessage() << std::endl;
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
  MultiEventHandler(): MessageEventHandler<MultiEventHandler>(), StringEventHandler<MultiEventHandler>() {}
  virtual void onMessage(const anch::events::Event<Message>& event) noexcept {
    std::cout << "MULTI : " << event.body.getMessage() << std::endl;
  }
  virtual void onString(const anch::events::Event<std::string>& event) noexcept {
    std::cout << "MULTI : " << event.body << std::endl;
  }
};


int
main(void) {
  std::cout << "Enter in test" << std::endl;

  SingleMsgHandler handler;
  MultiEventHandler multi;

  Observable<Message> observable;
  observable.addObserver(handler);
  observable.addObserver(multi);
  Observable<std::string> strObs;
  strObs.addObserver(multi);

  strObs.notifyObservers("tata");
  observable.notifyObservers(Message("toto"));

  std::cout << "Exit test" << std::endl;
  return 0;
}
