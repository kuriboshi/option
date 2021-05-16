//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <functional>
#include <string>
#include <map>

#include "parse_args.hh"
#include "usage.hh"

namespace wani::option
{
template<typename T>
class Commands
{
public:
  using function_t = std::function<void(T&, args_range_t)>;

  Commands& command(const std::string& name, function_t cmd)
  {
    _commands.emplace(name, cmd);
    return *this;
  }

  void parse(T& wanikani, const args_range_t& args)
  {
    if(args.first == args.second)
      usage();
    auto c = _commands.find(*args.first);
    if(c == _commands.end())
      usage();
    c->second(wanikani, {args.first + 1, args.second});
  }

private:
  std::map<std::string, function_t> _commands;
};

} // namespace wani::option
