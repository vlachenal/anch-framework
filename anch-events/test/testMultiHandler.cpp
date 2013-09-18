#include "events/observable.hpp"
#include "events/eventHandler.hpp"
INIT_ANCH_EVENT_BUS;

#include <iostream>

using std::string;
using std::cout;
using std::endl;

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
  virtual void onMessage(const Message& event) noexcept = 0;
};

/*!
 * \ref Message event handler
 */
class SingleMsgHandler: public MessageEventHandler<SingleMsgHandler> {
public:
  SingleMsgHandler(): MessageEventHandler<SingleMsgHandler>() {}
  virtual void onMessage(const Message& event) noexcept {
    cout << event.getMessage() << endl;
  }
};


/*!
 * \ref string event handler interface.
 */
template<typename D>
class StringEventHandler: public EventHandler<string,D> {
public:
  StringEventHandler(): EventHandler<string,D>(&D::onString) {}
  virtual void onString(const std::string& event) = 0;
};

/*!
 * \ref string and \ref Message events handler.
 */
class MultiEventHandler: public MessageEventHandler<MultiEventHandler>, public StringEventHandler<MultiEventHandler> {
public:
  MultiEventHandler(): MessageEventHandler<MultiEventHandler>(), StringEventHandler<MultiEventHandler>() {}
  virtual void onMessage(const Message& event) noexcept {
    cout << "MULTI : " << event.getMessage() << endl;
  }
  virtual void onString(const std::string& event) {
    cout << "MULTI : " << event << endl;
  }
};


int
main(void) {
  cout << "Enter in test" << endl;

  SingleMsgHandler handler;
  MultiEventHandler multi;

  Observable<Message> observable;
  observable.addObserver(handler);
  observable.addObserver(multi);
  Observable<string> strObs;
  strObs.addObserver(multi);

  strObs.notifyObservers("tata");
  observable.notifyObservers(Message("toto"));

  cout << "Exit test" << endl;
  return 0;
}
