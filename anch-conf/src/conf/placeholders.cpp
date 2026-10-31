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
#include "conf/placeholders.hpp"

#include <regex>
#include <vector>

#include "conf/resolvers.hpp"

#include "events/observer.hpp"

#include <iostream> // \todo remove

using anch::conf::Placeholders;
using anch::conf::Section;
using anch::conf::Resolvers;
using anch::conf::ConfError;

namespace anch::conf {

  /*!
   * Resolved configuration event
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ResConfEvt {

    /*! Configuration path */
    std::string conf;

    /*! Value */
    std::string val;

  };

  /*!
   * \brief Placeholder items
   *
   * Placeholder items contain the resolver (empty when default value) and argument
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct SubPlaceholder: anch::events::Observer<ResConfEvt> {

    /*! Resolver (empty when default value) */
    std::string res;

    /*! Resolver argument */
    std::string arg;

    /*! The found value (default to empty => not found) */
    std::optional<std::string> val;

    /*!
     * \ref SubPlaceholder default constructor
     */
    SubPlaceholder() noexcept {
      // Nothing to do
    }

    /*!
     * \ref SubPlaceholder desctructor
     */
    virtual ~SubPlaceholder() noexcept {
      // Nothing to do
    }

    /*!
     * Handle \ref anch::conf::ResConfEvt event
     *
     * \param event the event to manage
     */
    virtual void handle([[ maybe_unused ]] const anch::events::Event<ResConfEvt>& event) noexcept {
      // \todo plop
    }

    /*!
     * Placeholder item has been resolved
     *
     * \return \c true when subplaceholder has resolved, \c false otherwise
     */
    bool resvoled() noexcept {
      return val.has_value();
    }

  };

  /*!
   * Sub placeholder
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct SubPlaceholders {

    /*! Parent configuration placeholders */
    ConfPlaceholders& cph;

    /*! Subplaceholders */
    std::vector<anch::conf::SubPlaceholder> subs;

    /*!
     * \ref SubPlaceholders constructor
     *
     * \param cphs the parent \ref ConfPlaceholders
     */
    SubPlaceholders(ConfPlaceholders& cphs) noexcept: cph(cphs), subs() {
      // Nothing to do
    }

    /*!
     * \ref SubPlaceholders destructor
     */
    virtual ~SubPlaceholders() noexcept {
      // Nothing to do
    }

  };

  /*!
   * Configuration placeholders
   *
   * \since 0.1
   *
   * \author Vincent Lachenal
   */
  struct ConfPlaceholders {

    /*! Configuration path */
    std::string path;

    /*! Placeholders for this configuration */
    std::map<std::string, std::vector<anch::conf::SubPlaceholder>> placeholders;

    /*! Placeholders has been resolved flag */
    bool resolved;

    /*! The event bus to use if needed */
    anch::events::EventBus<anch::conf::ResConfEvt>* evtBus;

    /*!
     * \ref ConfPlaceholders default constructor
     */
    ConfPlaceholders(): path(), placeholders(), resolved(false), evtBus(NULL) {
      // Nothing to do
    }

    /*!
     * \ref ConfPlaceholders desstructor
     */
    virtual ~ConfPlaceholders() noexcept {
      // Nothing to do
    }
    /*!
     * Try to resolv configuration placeholders
     *
     * \return \c true when every placeholder has been resolved, \c false otherwise
     */
    bool resolv() {
      // \todo try to resolv configuration path, store it when resolv and fire event
      return false;
    }

    /*!
     * Subplaceholders getter
     *
     * \param key the subplaceholder key
     *
     * \return the subplaceholder
     *
     * \throw ... when not found
     */
    inline
    std::vector<anch::conf::SubPlaceholder>&
    getSubs(const std::string& key) {
      auto iter = placeholders.find(key);
      if(iter == placeholders.end()) {
	// \todo raise error
      }
      return iter->second;
    }

  };

}


const std::regex PH_PATTERN("\\$\\{( )*([^}]+)( )*\\}");
const std::regex RSV_PATTERN("(([^=|]+)( )*=)?(( )*([^|]+))");


// Constructors +
Placeholders::Placeholders(anch::conf::Section& section) noexcept:
  _section(section),
  _values(),
  _evtBus(NULL) {
  // Nothing to do
}
// Constructors -

// Destructor +
Placeholders::~Placeholders() noexcept {
  _values.clear();
  if(_evtBus != NULL) {
    _evtBus->release();
    _evtBus = NULL;
  }
}
// Destructor -

// Methods +
void
Placeholders::resolve() {
  if(_values.empty()) {
    std::cout << "No placeholder has been found" << std::endl;
    return;
  }
  _evtBus = new anch::events::EventBus<ResConfEvt>();
  std::cout << std::endl << "Resolve placeholders" << std::endl;
  for(auto iter = _values.begin() ; iter != _values.end() ; ++iter) {
    std::cout << "Try to resolve placeholder " << iter->first << std::endl;
    // \todo resolve placeholder
  }
}

auto RIT_END = std::sregex_iterator();

void
Placeholders::parsePlaceholder(anch::conf::ConfPlaceholders& cph, const std::string& placeholder) {
  std::cout << "Parse placeholder " << placeholder << " for key " << cph.path << std::endl;
  auto sub = std::sregex_iterator(placeholder.begin(), placeholder.end(), RSV_PATTERN);
  if(sub == RIT_END) {
    return;
  }
  std::vector<anch::conf::SubPlaceholder> subs;
  for(/*std::size_t i = 0*/ ; sub != RIT_END ; ++sub/*, ++i*/) {
    std::cout << "Found: resolver = '" << sub->str(2) << "' ; value = '" << sub->str(6) << '\'' << std::endl;
    anch::conf::SubPlaceholder item;
    item.res = sub->str(2);
    if(item.res == "") { // default value
      item.val = sub->str(6);
    } else if(!Resolvers::getInstance().hasResolver(item.res)) { // check if resolver exists
      std::ostringstream oss;
      oss << "Resolver " << item.res << " has not been found";
      throw ConfError(oss.str(), ConfError::ErrorCode::RES_NOT_FOUND);
    } else {
      item.arg = sub->str(6);
    }
    if(item.res == Resolvers::ENV) { // resolv environment variable
      item.val = std::invoke(Resolvers::getInstance().getResolver(Resolvers::ENV), item.arg, "");
      std::cout << "Found env " << item.arg << ": " << item.val.value_or("not found") << std::endl;
    } else if(item.res == Resolvers::CONF) {
      std::optional<std::string> val = _section.getValue<std::string>(item.arg);
      if(!val.has_value()) {
	continue;
      }
      if(val.has_value() /* && val does not match placeholder ... */) {
	item.val = val.value();
      } else {
	// \todo register placeholder as event's consumer
      }
    }
    subs.push_back(item);
  }
  cph.placeholders[placeholder] = subs;
}

void
Placeholders::collect(Section& section, const std::string& path) {
  std::cout << std::endl << "Parse placeholder in section: " << (path.empty() ? "." : path) << std::endl;
  std::string basePath = path.empty() ? path : path + '.';
  for(auto iter = section.getValues().begin() ; iter != section.getValues().end() ; ++iter) {
    std::string valuePath = basePath + iter->first;
    std::cout << std::endl << "Parse value " << iter->second << " for key " << valuePath << std::endl;
    auto sub = std::sregex_iterator(iter->second.begin(), iter->second.end(), PH_PATTERN);
    if(sub == RIT_END) {
      continue;
    }
    anch::conf::ConfPlaceholders cph;
    cph.path = valuePath;
    cph.evtBus = _evtBus;
    for(/*std::size_t i = 0*/ ; sub != RIT_END ; ++sub/*, ++i*/) { // sub->str(0) matches the complete PH_PATTERN with ${}
      parsePlaceholder(cph, sub->str(2));
    }
    if(!cph.resolv()) {
      _values[valuePath] = cph;
    }
  }
  for(auto iter = section.getSections().begin() ; iter != section.getSections().end() ; ++iter) {
    collect(section.section(iter->first), basePath + iter->first);
  }
}
// Methods -
