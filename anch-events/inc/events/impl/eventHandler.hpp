/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

namespace anch::events {

  template<typename T, typename D>
  EventHandler<T,D>::EventHandler(const std::function<void(D&,const anch::events::Event<T>&)>& callbackFunction, bool useEventBus): _callbackFunction(callbackFunction) {
    if(useEventBus) {
      anch::events::EventBus<T>& eventBus = anch::events::EventBus<T>::getInstance();
      eventBus.addObserver(*this);
      _eventBus = &eventBus;
    } else {
      _eventBus = NULL;
    }
  }

  template<typename T, typename D>
  EventHandler<T,D>::EventHandler(const std::function<void(D&,const anch::events::Event<T>&)>& callbackFunction,
				  anch::events::EventBus<T>& eventBus): _callbackFunction(callbackFunction) {
    eventBus.addObserver(*this);
    _eventBus = &eventBus;
  }

  template<typename T, typename D>
  EventHandler<T,D>::~EventHandler() {
    if(_eventBus != NULL) {
      _eventBus->removeObserver(*this);
    }
  }

  template<typename T, typename D>
  void
  EventHandler<T,D>::handle(const anch::events::Event<T>& event) noexcept {
    _callbackFunction(*static_cast<D*>(this), event);
  }

}
