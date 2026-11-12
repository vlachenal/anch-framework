/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
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
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "conf/placeholders.hpp"

#include <algorithm>
#include <deque>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "conf/resolvers.hpp"

using anch::conf::ConfError;
using anch::conf::Placeholders;
using anch::conf::Resolvers;
using anch::conf::Section;

namespace {

  /*! Placeholder alternative: resolver and argument, or default value (empty resolver) */
  struct Alternative {
    std::string resolver;
    std::string argument;
  };

  /*! Placeholder: ${alternative|alternative|...} */
  struct Placeholder {
    /*! Placeholder text for error messages */
    std::string text;
    /*! Alternatives, in priority order */
    std::vector<Alternative> alternatives;
    /*! Resolved value */
    std::optional<std::string> value;
    /*! Configuration path being waited */
    std::optional<std::string> waiting;
  };

  /*! Configuration value with placeholders: literal parts and placeholders */
  struct Node {
    std::string path;
    Section* section = nullptr;
    std::string key;
    std::vector<std::variant<std::string, Placeholder>> parts;
    bool resolved = false;
  };

  /*! "Resolved" event: configuration path and resolution chain which led to it */
  struct Resolved {
    std::string path;
    std::vector<std::string> chain;
  };

  std::string_view
  trim(std::string_view value) noexcept {
    const std::size_t first = value.find_first_not_of(" \t");
    if(first == std::string_view::npos) {
      return std::string_view();
    }
    return value.substr(first, value.find_last_not_of(" \t") - first + 1);
  }

  std::string
  join(const std::vector<std::string>& chain) {
    std::string res;
    for(const std::string& path : chain) {
      if(!res.empty()) {
	res += " -> ";
      }
      res += path;
    }
    return res;
  }

  /*! Resolution of one configuration */
  class Resolution {
  private:
    Section& _root;
    std::size_t _maxDepth;
    std::map<std::string, Node> _nodes;
    std::map<std::string, std::vector<std::string>> _dependents;
    std::deque<Resolved> _queue;

  public:
    Resolution(Section& root, std::size_t maxDepth): _root(root), _maxDepth(maxDepth), _nodes(), _dependents(), _queue() {
    }

    std::size_t
    run() {
      collect(_root, "");
      // Values without pending dependency
      for(auto& [path, node] : _nodes) {
	if(evaluate(node)) {
	  complete(node, {path});
	}
      }
      // Propagation
      std::size_t events = 0;
      while(!_queue.empty()) {
	Resolved event = std::move(_queue.front());
	_queue.pop_front();
	if(++events > _nodes.size()) { // each value is resolved once: should never happen
	  throw ConfError("Placeholders resolution watchdog: too many events", ConfError::ErrorCode::CYCLE);
	}
	auto dependents = _dependents.find(event.path);
	if(dependents == _dependents.end()) {
	  continue;
	}
	for(const std::string& path : dependents->second) {
	  Node& node = _nodes.at(path);
	  if(node.resolved) {
	    continue;
	  }
	  std::vector<std::string> chain = event.chain;
	  chain.push_back(path);
	  if(chain.size() > _maxDepth) {
	    throw ConfError("Placeholders resolution chain is longer than " + std::to_string(_maxDepth) + ": " + join(chain),
			    ConfError::ErrorCode::CYCLE);
	  }
	  if(evaluate(node)) {
	    complete(node, std::move(chain));
	  }
	}
      }
      checkUnresolved();
      return _nodes.size();
    }

  private:
    void
    collect(Section& section, const std::string& path) {
      const std::string base = path.empty() ? path : path + '.';
      for(const auto& [key, value] : section.getValues()) {
	if(value.find("${") == std::string::npos) {
	  continue;
	}
	Node node;
	node.path = base + key;
	node.section = &section;
	node.key = key;
	node.parts = parse(value, node.path);
	_nodes.emplace(node.path, std::move(node));
      }
      for(const auto& [name, sub] : section.getSections()) {
	collect(section.section(name), base + name);
      }
    }

    static std::vector<std::variant<std::string, Placeholder>>
    parse(std::string_view value, const std::string& path) {
      std::vector<std::variant<std::string, Placeholder>> parts;
      std::size_t pos = 0;
      while(pos < value.size()) {
	const std::size_t open = value.find("${", pos);
	if(open == std::string_view::npos) {
	  parts.emplace_back(std::string(value.substr(pos)));
	  break;
	}
	if(open > pos) {
	  parts.emplace_back(std::string(value.substr(pos, open - pos)));
	}
	const std::size_t close = value.find('}', open);
	if(close == std::string_view::npos) {
	  throw ConfError("Unclosed placeholder in " + path, ConfError::ErrorCode::PARSING_ERROR);
	}
	std::string_view content = value.substr(open + 2, close - open - 2);
	Placeholder placeholder;
	placeholder.text = std::string(value.substr(open, close - open + 1));
	if(content.find("${") != std::string_view::npos) {
	  throw ConfError("Nested placeholders are not supported: " + placeholder.text + " in " + path,
			  ConfError::ErrorCode::PARSING_ERROR);
	}
	while(true) {
	  const std::size_t bar = content.find('|');
	  std::string_view alternative = trim(content.substr(0, bar));
	  if(alternative.empty()) {
	    throw ConfError("Empty alternative in " + placeholder.text + " in " + path, ConfError::ErrorCode::PARSING_ERROR);
	  }
	  const std::size_t equal = alternative.find('=');
	  if(equal == std::string_view::npos) {
	    placeholder.alternatives.push_back(Alternative{std::string(), std::string(alternative)});
	  } else {
	    Alternative alt{std::string(trim(alternative.substr(0, equal))), std::string(trim(alternative.substr(equal + 1)))};
	    if(!Resolvers::getInstance().hasResolver(alt.resolver)) {
	      throw ConfError("Resolver " + alt.resolver + " has not been found (" + placeholder.text + " in " + path + ')',
			      ConfError::ErrorCode::RES_NOT_FOUND);
	    }
	    placeholder.alternatives.push_back(std::move(alt));
	  }
	  if(bar == std::string_view::npos) {
	    break;
	  }
	  content = content.substr(bar + 1);
	}
	parts.emplace_back(std::move(placeholder));
	pos = close + 1;
      }
      return parts;
    }

    /*! Evaluate pending placeholders: true when every placeholder is resolved */
    bool
    evaluate(Node& node) {
      bool complete = true;
      for(auto& part : node.parts) {
	Placeholder* placeholder = std::get_if<Placeholder>(&part);
	if(placeholder != nullptr && !placeholder->value && !evaluate(node, *placeholder)) {
	  complete = false;
	}
      }
      return complete;
    }

    bool
    evaluate(Node& node, Placeholder& placeholder) {
      placeholder.waiting.reset();
      for(const Alternative& alt : placeholder.alternatives) {
	if(alt.resolver.empty()) { // default value
	  placeholder.value = alt.argument;
	  return true;
	}
	if(alt.resolver == Resolvers::CONF) {
	  auto dependency = _nodes.find(alt.argument);
	  if(dependency != _nodes.end() && !dependency->second.resolved) { // wait for "resolved" event
	    placeholder.waiting = alt.argument;
	    std::vector<std::string>& dependents = _dependents[alt.argument];
	    if(std::find(dependents.begin(), dependents.end(), node.path) == dependents.end()) {
	      dependents.push_back(node.path);
	    }
	    return false;
	  }
	  placeholder.value = Section::getValue(alt.argument, _root);
	} else {
	  placeholder.value = std::invoke(Resolvers::getInstance().getResolver(alt.resolver), alt.argument, std::nullopt);
	}
	if(placeholder.value) {
	  return true;
	}
      }
      throw ConfError("Unable to resolve " + placeholder.text + " in " + node.path, ConfError::ErrorCode::NOT_RES);
    }

    /*! Write resolved value and fire "resolved" event */
    void
    complete(Node& node, std::vector<std::string> chain) {
      std::string value;
      for(const auto& part : node.parts) {
	if(const std::string* literal = std::get_if<std::string>(&part)) {
	  value += *literal;
	} else {
	  value += *std::get<Placeholder>(part).value;
	}
      }
      node.section->putValue(node.key, value);
      node.resolved = true;
      _queue.push_back(Resolved{node.path, std::move(chain)});
    }

    /*! Values still waiting reference each other: report the cycle */
    void
    checkUnresolved() const {
      for(const auto& [path, node] : _nodes) {
	if(node.resolved) {
	  continue;
	}
	std::vector<std::string> chain;
	const Node* current = &node;
	while(std::find(chain.begin(), chain.end(), current->path) == chain.end()) {
	  chain.push_back(current->path);
	  current = &_nodes.at(waitingOn(*current));
	}
	// Keep the cycle only (values leading to it are not part of it)
	chain.erase(chain.begin(), std::find(chain.begin(), chain.end(), current->path));
	chain.push_back(current->path);
	throw ConfError("Placeholders resolution cycle: " + join(chain), ConfError::ErrorCode::CYCLE);
      }
    }

    static const std::string&
    waitingOn(const Node& node) {
      for(const auto& part : node.parts) {
	const Placeholder* placeholder = std::get_if<Placeholder>(&part);
	if(placeholder != nullptr && placeholder->waiting) {
	  return *placeholder->waiting;
	}
      }
      throw ConfError("Unresolved value " + node.path, ConfError::ErrorCode::NOT_RES); // should not happen
    }
  };

}

// Constructors +
Placeholders::Placeholders(Section& section, std::size_t maxDepth) noexcept: _section(section), _maxDepth(maxDepth) {
  // Nothing to do
}
// Constructors -

// Destructor +
Placeholders::~Placeholders() noexcept {
  // Nothing to do
}
// Destructor -

// Methods +
std::size_t
Placeholders::resolve() {
  Resolution resolution(_section, _maxDepth);
  return resolution.run();
}
// Methods -
