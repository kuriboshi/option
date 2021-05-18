//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include "Option.hh"
#include "parse_args.hh"
#include "usage.hh"

namespace wani::option
{
class Program
{
  struct Group
  {
    Group() = default;
    ~Group() = default;
    Group(const Group& g) = delete;
    Group(Group&& g) : min_args(g.min_args), max_args(g.max_args), valid_options(std::move(g.valid_options))
    {
      g.min_args = 0;
      g.max_args = 0;
    }
    Group& operator=(Group&& g)
    {
      if(this != &g)
      {
        min_args = g.min_args;
        max_args = g.max_args;
        valid_options = std::move(g.valid_options);
        g.min_args = 0;
        g.max_args = 0;
      }
      return *this;
    }

    int min_args = 0;
    int max_args = 0;
    std::map<std::string, Option> valid_options;
  };
  std::string _name;
  std::vector<Group> _groups;
  Group _group;

public:
  Program(const args_range_t& range, const std::string& name): _name(name), _range(range) {}

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

  Program& group()
  {
    _groups.push_back(std::move(_group));
    return *this;
  }

  void usage()
  {
    std::vector<std::string> help_strings;
    for(auto& g: _groups)
    {
      std::string help{_name};
      for(auto& [key, o]: g.valid_options)
        help += " " + o.help();
      for(auto i = 1; i <= g.min_args; ++i)
        help += " <arg>";
      if(g.min_args < g.max_args)
      {
        help += " [<arg>";
        if(g.max_args == std::numeric_limits<int>::max())
          help += "...]";
        else
          help += "]";
      }
      help_strings.push_back(help);
    }
    std::ostringstream os;
    option::usage(help_strings);
  }

  Program& args()
  {
    return group();
  }

  Program& args(int min_args)
  {
    _group.min_args = min_args;
    _group.max_args = std::numeric_limits<int>::max();
    return group();
  }

  Program& args(int min_args, int max_args)
  {
    _group.min_args = min_args;
    _group.max_args = max_args;
    return group();
  }

  args_range_t parse()
  {
    _groups.push_back(std::move(_group));
    std::vector<std::string> errors;
    for(auto& group: _groups)
    {
      try
      {
        return parse(_range, group);
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

private:
  args_range_t _range;

  args_range_t exec(args_t::iterator args, Group& group, std::vector<Option*>& options)
  {
    auto distance = std::distance(args, _range.second);
    if(distance < group.min_args || distance > group.max_args)
      usage();
    for(const auto* o: options)
      o->exec();
    return {args, _range.second};
  }

  args_range_t parse(const args_range_t& range, Group& group)
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
        return exec(++arg, group, options);
      else if(!arg->empty() && arg->at(0) == '-')
        throw argument_error("unknown option: " + *arg);
      else
        return exec(arg, group, options);
    }
    for(auto& o: group.valid_options)
      if(o.second.required && !o.second.set)
        throw argument_error("missing required argument: " + o.second.name());
    return exec(arg, group, options);
  }
};

} // namespace wani::option
