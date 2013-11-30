#include "events/observable.hpp"

#include <iostream>

using std::string;
using std::cout;
using std::endl;

using anch::events::Observable;
using anch::events::Observer;


class Event {

private:
  string _message;

public:
  Event(const string& message): _message(message) {
    // Nothing to do
  }

  const string& str() const {
    return _message;
  }

};

class StrObserver : public Observer<Event> {

public:
  StrObserver() {
    // Nothing to do
  }

  virtual ~StrObserver() {
    // Nothing to do
  }

public:
  virtual void notify(const Event& event) noexcept {
    cout << "Observer receive event: " << event.str() << endl;
  }

};


int
main(void) {
  Observable<Event> handler;

  cout << "Add observer" << endl;
  // Add observers +
  StrObserver obs1;
  handler.addObserver(obs1);
  cout << "Notify" << endl;
  // Send event +
  handler.notifyObservers(Event("TUTUTUTUTUTU"));
  // Send event -

  cout << "Add observer" << endl;
  // Add observers +
  StrObserver obs2;
  handler.addObserver(obs2);
  StrObserver obs3;
  handler.addObserver(obs3);
  StrObserver obs4;
  handler.addObserver(obs4);
  StrObserver obs5;
  handler.addObserver(obs5);
  StrObserver obs6;
  handler.addObserver(obs6);
  // Add observers -

  cout << "Notify" << endl;
  // Send event +
  handler.notifyObservers(Event("TOTOTOTOTOTO"));
  // Send event -

  cout << "Remove observer by reference" << endl;
  handler.removeObserver(obs3);

  cout << "Notify" << endl;
  // Send event +
  handler.notifyObservers(Event("TITITITITITI"));
  // Send event -

  return 0;
}
