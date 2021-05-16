//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <map>
#include <string>
#include <memory>
#include <tuple>

#include "Option.hh"
#include "parse_args.hh"
#include "usage.hh"

namespace wani::option
{
template<typename... Ts>
class Program
{
  std::map<std::string, Option> _valid_options;
  std::tuple<const Ts...> _usage;

public:
  Program(const Ts&&... u): _usage(std::forward<const std::string>(u)...) {}

  template<typename F>
  Program& option(const std::string& name, F f)
  {
    _valid_options.emplace(name, Option(name, f));
    return *this;
  }

  class Usage
  {
  public:
    template<typename... Args>
    void operator()(const Args&... args)
    {
      wani::option::usage(args...);
    }
  };

  void usage() { std::apply(Usage(), _usage); }

  void args(const args_range_t& rest)
  {
    try
    {
      auto args = parse(rest);
      if(args.empty())
        return;
    }
    catch(const std::exception&)
    {}
    usage();
  }

  std::vector<std::string> args(const args_range_t& rest, int min_args)
  {
    try
    {
      auto args = parse(rest);
      if(args.size() >= min_args)
        return args;
    }
    catch(const std::exception&)
    {}
    usage();
    throw;
  }

  std::vector<std::string> args(const args_range_t& rest, int min_args, int max_args)
  {
    try
    {
      auto args = parse(rest);
      if(args.size() >= min_args && args.size() <= max_args)
        return args;
    }
    catch(const std::exception&)
    {}
    usage();
    throw;
  }

private:
  std::vector<std::string> parse(const args_range_t& rest)
  {
    std::vector<std::string> args;
    Option* current_option = nullptr;
    auto arg = rest.first;
    bool process_options = true;
    while(arg != rest.second)
    {
      if(!process_options)
        args.push_back(*arg);
      else if(current_option)
      {
        // Set the option value and execute it
        current_option->set = true;
        current_option->value = *arg;
        current_option->exec();
        current_option = nullptr;
      }
      else if(auto opt = _valid_options.find(*arg); opt != _valid_options.end())
      {
        // Previous option taking an argument didn't get the argument
        if(current_option && !current_option->set)
          throw argument_error("missing option value: " + current_option->name());
        // If the option takes an argument, set current_option
        if(opt->second.argument())
          current_option = &opt->second;
        else
          opt->second.exec();
      }
      else if(*arg == "--")
        process_options = false;
      else if(!arg->empty() && arg->at(0) == '-')
        throw argument_error("unknown option: " + *arg);
      else
      {
        process_options = false;
        args.push_back(*arg);
      }
      ++arg;
    }
    return args;
  }
};

} // namespace wani::option
