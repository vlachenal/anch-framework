#include "events/observable.hpp"

#include <iostream>

using std::string;
using std::cout;
using std::endl;

using anch::events::Observable;
using anch::events::Observer;


class Evt {

private:
  string _message;

public:
  Evt(const string& message): _message(message) {
    // Nothing to do
  }

  const string& str() const {
    return _message;
  }

};

class StrObserver : public Observer<Evt> {

public:
  StrObserver() {
    // Nothing to do
  }

  virtual ~StrObserver() {
    // Nothing to do
  }

public:
  virtual void handle(const anch::events::Event<Evt>& event) noexcept {
    cout << "Observer receive event: " << event.body.str() << endl;
  }

};


int
main(void) {
  Observable<Evt> handler;

  cout << "Add observer" << endl;
  // Add observers +
  StrObserver obs1;
  handler.addObserver(obs1);
  cout << "Notify" << endl;
  // Send event +
  handler.notifyObservers(Evt("TUTUTUTUTUTU"));
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
  handler.notifyObservers(Evt("TOTOTOTOTOTO"));
  // Send event -

  cout << "Remove observer by reference" << endl;
  handler.removeObserver(obs3);

  cout << "Notify" << endl;
  // Send event +
  handler.notifyObservers(Evt("TITITITITITI"));
  // Send event -

  return 0;
}
