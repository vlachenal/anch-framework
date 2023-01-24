#include "events/eventBus.hpp"
#include "events/eventHandler.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

using anch::events::Observer;
using anch::events::EventBus;


class Evt {
private:
  string _message;
public:
  Evt(const string& message): _message(message) {}
  inline const string& getMessage() const {
    return _message;
  }
};

class EventObserver: public Observer<Evt> {
public:
  EventObserver() {}
  virtual void handle(const anch::events::Event<Evt>& event) noexcept override {
    cout << event.body.getMessage() << endl;
  }
};


template<typename D>
class EventHandlerIface: public anch::events::EventHandler<Evt,D> {
public:
  EventHandlerIface(bool useEventBus = true): anch::events::EventHandler<Evt,D>(&D::onMessage,useEventBus) {}
  virtual void onMessage(const anch::events::Event<Evt>& event) noexcept = 0;
};

class EventHandler: public EventHandlerIface<EventHandler> {
public:
  EventHandler(): EventHandlerIface<EventHandler>() {}
  virtual void onMessage(const anch::events::Event<Evt>& event) noexcept override {
    cout << "HANDLER : " << event.body.getMessage() << endl;
  }
};


int
main(void) {
  cout << "Enter in EventBus test" << endl;

  EventObserver obs;
  EventHandler handler;
  EventBus<Evt>& eventBus = EventBus<Evt>::getInstance();
  eventBus.addObserver(obs);
  eventBus.addObserver(handler);

  eventBus.fireEvent(Evt("titi"));
  for(int i  = 0 ; i < 10 ; i++) {
    eventBus.scheduleDeferred(Evt("toto"));
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(200)); // to receive deferred messages

  cout << "Exit EventBus test" << endl;
  return 0;
}
