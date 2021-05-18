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
  class Group
  {
  public:
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

  args_range_t _range;
  std::string _name;
  std::vector<Group> _groups;
  Group _group;

public:
  //
  // Create a program with the arguments in 'range'.  The name of the program
  // is in 'name'.
  //
  Program(const args_range_t& range, const std::string& name): _range(range), _name(name) {}

  //
  // Add a required option to the program.  The function 'f' can either be a
  // function taking no arguments or a function taking a 'const Option&'.  In
  // the former case it represents a boolean option and in the latter case it's
  // an option taking one value.
  //
  template<typename F>
  Program& required(const std::string& name, F f)
  {
    _group.valid_options.emplace(name, Option(name, true, f));
    return *this;
  }

  //
  // Add an optional option to the program.  The function 'f' is the same as
  // 'f' in 'required'.
  template<typename F>
  Program& optional(const std::string& name, F f)
  {
    _group.valid_options.emplace(name, Option(name, false, f));
    return *this;
  }

  //
  // Start a new group of options.
  //
  Program& group()
  {
    _groups.push_back(std::move(_group));
    return *this;
  }

  //
  // Alternative syntax starting a new group.  It means that the group
  // preceding the new group takes no more arguments.
  //
  Program& args()
  {
    return group();
  }

  //
  // Starts a new group setting the minimum number of argruments of the
  // previous group to 'min_args'.
  //
  Program& args(int min_args)
  {
    _group.min_args = min_args;
    _group.max_args = std::numeric_limits<int>::max();
    return group();
  }

  //
  // Starts a new group and sets both the min and max number of arguments
  // accepted.
  //
  Program& args(int min_args, int max_args)
  {
    _group.min_args = min_args;
    _group.max_args = max_args;
    return group();
  }

  //
  // Parse the arguments.  It tries each group in sequence and if successul
  // returns the range of arguments left after processing all options.  Any
  // group which can't be parsed, because there is an illegal option for
  // example, is skipped.  At the end, if no group is selected the first error
  // encountered is reported and an usage exception is thrown.
  //
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
    usage(*errors.begin());
    throw;
  }

  //
  // Construct the usage string based on what groups and what options there are
  // in each group.
  //
  void usage(const std::optional<std::string> error = {})
  {
    std::vector<std::string> help_strings;
    if(error)
      help_strings.push_back(*error);
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

private:
  //
  // Parse the range of arguments against the option group.  Throws an
  // 'argument_error' if there is anything wrong such as illegal option,
  // missing option parameter.
  //
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

  //
  // Verifies that the number of arguments in 'args' satisfies the group
  // criteria for min and max number of arguements.  If within range each
  // option is processed.  Finally, the range if remaining arguments is
  // returned to the called.
  //
  args_range_t exec(args_t::iterator args, Group& group, std::vector<Option*>& options)
  {
    auto distance = std::distance(args, _range.second);
    if(distance < group.min_args || distance > group.max_args)
      usage();
    for(const auto* o: options)
      o->exec();
    return {args, _range.second};
  }
};

} // namespace wani::option
