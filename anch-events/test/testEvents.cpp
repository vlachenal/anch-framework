#include "events/observable.hpp"

#include <iostram>

using std::string;
using std::cout;

using anch::events::Observable;
using anch::events::Observer;

class Event : public Observer<Event> {

private:
  /** Message */
  string _message;

public:
  Event(const String& message) {
    // Nothing to do
  };

  virtual void notify(const Event& event) {
    cout << "Receive event: " << message << endl;
  };

}

int
main(void) {
  Observable<Event> handler;
  Observer<Event> obs;
  handler.addObserver(obs);
  handler.notify(Event("TOTOTOTOTOTO"));
  return 0;
}
