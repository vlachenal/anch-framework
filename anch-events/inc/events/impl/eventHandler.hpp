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

  template<typename Evt, typename D>
  EventHandler<Evt,D>::EventHandler(const std::function<void(D&,const Evt&)>& callbackFunction, bool useEventBus): _callbackFunction(callbackFunction) {
    if(useEventBus) {
      anch::events::EventBus<Evt>& eventBus = anch::events::EventBus<Evt>::getInstance();
      eventBus.addObserver(*this);
      _eventBus = &eventBus;
    } else {
      _eventBus = NULL;
    }
  }

  template<typename Evt, typename D>
  EventHandler<Evt,D>::EventHandler(const std::function<void(D&,const Evt&)>& callbackFunction,
				    anch::events::EventBus<Evt>& eventBus): _callbackFunction(callbackFunction) {
    eventBus.addObserver(*this);
    _eventBus = &eventBus;
  }

  template<typename Evt, typename D>
  EventHandler<Evt,D>::~EventHandler() {
    if(_eventBus != NULL) {
      _eventBus->removeObserver(*this);
    }
  }

  template<typename Evt, typename D>
  void
  EventHandler<Evt,D>::notify(const Evt& event) noexcept {
    _callbackFunction(*static_cast<D*>(this), event);
  }

}
