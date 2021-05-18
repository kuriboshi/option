//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <tuple>

#include "Option.hh"
#include "parse_args.hh"
#include "usage.hh"

namespace wani::option
{
struct Group
{
  std::map<std::string, Option> valid_options;
};

class Arguments
{
public:
  Arguments() = default;
  Arguments(const args_range_t& range) : range(range) {}
  args_range_t range;
};

template<typename... Ts>
class Program
{
  std::string _name;
  using usage_t = std::tuple<const Ts...>;
  usage_t _usage;
  std::vector<Group> _groups;
  Group _group;

public:
  Program(Arguments args, const std::string& name): _name(name), _args(args) {}
  Program(const Ts&&... u): _usage(std::forward<const std::string>(u)...) {}

  template<typename F>
  Program& required(const std::string& name, F f)
  {
    _group.valid_options.emplace(name, Option(name, true, f));
    return *this;
  }

  template<typename F>
  Program& optional(const std::string& name, F f)
  {
    _group.valid_options.emplace(name, Option(name, false, f));
    return *this;
  }

  template<typename F>
  Program& option(const std::string& name, F f)
  {
    _group.valid_options.emplace(name, Option(name, false, f));
    return *this;
  }

  Program& group()
  {
    _groups.push_back(std::move(_group));
    return *this;
  }

  class Usage
  {
  public:
    template<typename... Args>
    void operator()(const Args&... args)
    {
      if constexpr(sizeof...(args) > 0)
        wani::option::usage(args...);
    }
  };

  void usage()
  {
    if constexpr(std::tuple_size<usage_t>() > 0)
      std::apply(Usage(), _usage);
    else
    {
      std::vector<std::string> help_strings;
      for(auto& g: _groups)
      {
        std::string help{_name};
        for(auto& [key, o]: g.valid_options)
          help += " " + o.help();
        help_strings.push_back(help);
      }
      std::ostringstream os;
      option::usage(help_strings);
    }
  }

  void args()
  {
    try
    {
      auto begin = parse();
      if(begin == _args.range.second)
        return;
    }
    catch(const std::exception&)
    {}
    usage();
  }

  args_range_t args(int min_args)
  {
    try
    {
      auto begin = parse();
      if(std::distance(begin, _args.range.second) >= min_args)
        return {begin, _args.range.second};
    }
    catch(const std::exception&)
    {}
    usage();
    throw;
  }

  args_range_t args(int min_args, int max_args)
  {
    try
    {
      auto begin = parse();
      if(auto d = std::distance(begin, _args.range.second); d >= min_args && d <= max_args)
        return {begin, _args.range.second};
    }
    catch(const std::exception&)
    {}
    usage();
    throw;
  }

  void args(const args_range_t& rest)
  {
    try
    {
      auto args = parse(rest);
      if(args == rest.second)
        return;
    }
    catch(const std::exception&)
    {}
    usage();
  }

  args_range_t args(const args_range_t& rest, int min_args)
  {
    try
    {
      auto args = parse(rest);
      if(std::distance(args, rest.second) >= min_args)
        return {args, rest.second};
    }
    catch(const std::exception&)
    {}
    usage();
    throw;
  }

  args_range_t args(const args_range_t& rest, int min_args, int max_args)
  {
    try
    {
      auto args = parse(rest);
      if(auto d = std::distance(args, rest.second); d >= min_args && d <= max_args)
        return {args, rest.second};
    }
    catch(const std::exception&)
    {}
    usage();
    throw;
  }

private:
  Arguments _args;

  args_t::iterator parse()
  {
    std::vector<std::string> errors;
    _groups.push_back(std::move(_group));
    for(auto& group: _groups)
    {
      try
      {
        return parse(_args.range, group);
      }
      catch(const argument_error& e)
      {
        errors.push_back(e.what());
      }
    }
    std::cout << *errors.begin() << std::endl;
    usage();
    throw;
  }

  args_t::iterator exec(args_t::iterator args, std::vector<Option*>& options)
  {
    for(const auto* o: options)
      o->exec();
    return args;
  }

  args_t::iterator parse(const args_range_t& range, Group& group)
  {
    std::vector<Option*> options;
    Option* current_option = nullptr;
    auto arg = range.first;
    for(;arg != range.second; ++arg)
    {
      if(current_option)
      {
        // Set the option value and execute it
        current_option->value = *arg;
        current_option->set = true;
        options.push_back(current_option);
        current_option = nullptr;
      }
      else if(auto opt = group.valid_options.find(*arg); opt != group.valid_options.end())
      {
        // Previous option taking an argument didn't get the argument
        if(current_option)
          throw argument_error("missing option value: " + current_option->name());
        // If the option takes an argument, set current_option
        if(opt->second.argument())
          current_option = &opt->second;
        else
        {
          options.push_back(&opt->second);
          opt->second.set = true;
        }
      }
      else if(*arg == "--")
        return exec(++arg, options);
      else if(!arg->empty() && arg->at(0) == '-')
        throw argument_error("unknown option: " + *arg);
      else
        return exec(arg, options);
    }
    for(auto& o: group.valid_options)
      if(o.second.required && !o.second.set)
        throw argument_error("missing required argument: " + o.second.name());
    return exec(arg, options);
  }

  args_t::iterator parse(const args_range_t& rest)
  {
    _groups.push_back(std::move(_group));
    for(auto& group: _groups)
    {
      return parse(rest, group);
    }
    usage();
    throw;
  }
};

} // namespace wani::option
