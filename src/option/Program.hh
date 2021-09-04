// Copyright 2021 Krister Joas
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "Option.hh"
#include "parse_args.hh"
#include "string_functions.hh"
#include "usage.hh"

namespace kuri::option
{
///
/// @brief Handles parsing of options for an argument list.
///
/// @details
///   Options can be either required or optional.  There can be groups of
///   options.  The number of arguments after processing options may optionally
///   be constrained to a minimum and maximum.
///
class Program
{
public:
  ///
  /// @brief Creates a program argument parser.
  ///
  /// @param name
  ///   The name of the program and is used in the usage string.  This
  ///   parameter is optional and if not specified a program name will not be
  ///   included in the usage string.
  ///
  Program(std::optional<std::string> program_name = {})
    : _program_name(program_name) {}

  ///
  /// @brief Add a required option to the program.
  ///
  /// @tparam F
  ///   Callback function type.
  /// @param name
  ///   The name of the option which should include the double hyphens that are
  ///   part of the option string.
  /// @param f
  ///   A callback function which is called when the the option is found.  The
  ///   function can either be a function taking no arguments or a function
  ///   taking a 'const Option&' argument.  In the former case it represents a
  ///   boolean option and in the latter case it's an option taking one value.
  ///
  template<typename F>
  Program& required(const std::string& name, F f)
  {
    _group.valid_options.emplace(name, Option(name, true, f));
    return *this;
  }

  ///
  /// @brief Add an optional option to the program.
  ///
  /// @tparam F
  ///   Callback function type.
  /// @param name
  ///   The name of the option which should include the double hyphens that are
  ///   part of the option string.
  /// @param f
  ///   A callback function which is called when the the option is found.  The
  ///   function can either be a function taking no arguments or a function
  ///   taking a 'const Option&' argument.  In the former case it represents a
  ///   boolean option and in the latter case it's an option taking one value.
  ///
  template<typename F>
  Program& optional(const std::string& name, F f)
  {
    _group.valid_options.emplace(name, Option(name, false, f));
    return *this;
  }

  ///
  /// @brief Start a new group of options.
  ///
  Program& group()
  {
    _groups.push_back(std::move(_group));
    return *this;
  }

  ///
  /// @brief Creates a group representing the arguments after all options.
  ///
  /// @details
  ///   The two arguments defines how many arguments are accepted.  We have the
  ///   following possible scenarios.
  ///   
  /// @code
  ///   min, max | Description                 | Help string
  ///   ---------|-----------------------------|--------------------
  ///   {}, {}   | No arguments                | -
  ///   0, {}    | Any number of arguments     | [<arg>...]
  ///   1, {}    | At least one argument       | <arg> [<arg>...]
  ///   1, 1     | Exactly one argument        | <arg>
  ///   2, 3     | Two or three arguments      | <arg> <arg> [<arg>]
  ///   0, 2     | Zero, one, or two arguments | [<arg> [<arg>]]
  /// @endcode
  ///
  /// @param min_args
  ///   Minimum number of arguments.  The default is none.
  /// @param max_args
  ///   Maximum number of arguments.  A value of none (the default) means there
  ///   is no maximum and any number of arguments is accepted.
  ///
  Program& args(std::optional<int> min_args = {}, std::optional<int> max_args = {})
  {
    if(!min_args && max_args)
      throw std::runtime_error("Program::args: max_args without min_args");
    if(max_args && min_args && (*min_args > *max_args))
      throw std::runtime_error("Program::args: min_args > max_args");
    _group.min_args = min_args;
    _group.max_args = max_args;
    return group();
  }

  ///
  /// @brief Parse the arguments.
  ///
  /// @details
  ///   This function tries each group in sequence and if successful returns
  ///   the iterator of first argument which is not an option.  Any group which
  ///   can't be parsed, because there is an illegal option for example, is
  ///   skipped.  At the end, if no group is selected the first error
  ///   encountered is reported and a usage exception is thrown.
  ///
  /// @param first, last
  ///   The range of elements to parse.
  /// @return Returns the first iterator which is not an option.
  ///
  args_t::iterator parse(args_t::iterator first, args_t::iterator last)
  {
    _groups.push_back(std::move(_group));
    for(auto& group: _groups)
    {
      try
      {
        return parse(first, last, group);
      }
      catch(const argument_error& e)
      {
        _errors.push_back(e.what());
      }
    }
    usage();
    throw;
  }

  ///
  /// @brief Construct the help string.
  ///
  /// @return The return value is a vector of strings, one help string for each
  ///   group.  No consideration is given to the width of the generated help
  ///   strings.
  ///
  std::vector<std::string> help()
  {
    std::vector<std::string> help_strings;
    for(auto& g: _groups)
    {
      std::string help;
      bool first = true;
      if(_program_name)
      {
        first = false;
        help = *_program_name;
      }
      for(auto& [key, o]: g.valid_options)
      {
        if(first)
          first = false;
        else
          help += " ";
        help += o.help();
      }
      if(g.min_args)
      {
        auto min_args = *g.min_args;
        for(auto i = 1; i <= min_args; ++i)
          help += " <arg>";
        if(g.max_args)
        {
          if(*g.max_args > min_args)
          {
            help += " [";
            int first = 0;
            for(auto i = *g.min_args; i < *g.max_args; ++i)
            {
              if(first > 0)
                help += " [";
              ++first;
              help += "<arg>";
            }
            help += std::string(first, ']');
          }
        }
        else
          help += " [<arg>...]";
      }
      help_strings.push_back(help);
    }
    return help_strings;
  }

  ///
  /// @brief Construct the usage string based on what groups there are and what
  ///   options there are in each group.
  ///
  void usage()
  {
    if(_errors.empty())
      option::usage(help());
    option::usage(Error(_errors), help());
  }

private:
  ///
  /// @brief A Group represents a group of options which can optionally take a
  ///   number of arguments after the sequence of options.
  ///
  class Group
  {
  public:
    ///
    /// @brief Creates an empty group.
    ///
    Group() = default;
    ///
    /// @brief Destroys the object.
    ///
    ~Group() = default;
    ///
    /// @brief No copying allowed.
    ///
    Group(const Group&) = delete;
    ///
    /// @brief Move constructor which is used when building the list of groups.
    ///
    /// @param g The group to move.
    ///
    Group(Group&& g) : min_args(g.min_args), max_args(g.max_args), valid_options(std::move(g.valid_options))
    {
      // Default move constructor doesn't reset these members.
      g.min_args = {};
      g.max_args = {};
    }
    ///
    /// @brief Move assignment operator.
    ///
    /// @param g The group to move assign.
    ///
    Group& operator=(Group&& g)
    {
      // For good measure, define the move assignment operator.
      if(this != &g)
      {
        min_args = g.min_args;
        max_args = g.max_args;
        valid_options = std::move(g.valid_options);
        g.min_args = {};
        g.max_args = {};
      }
      return *this;
    }

    /// @brief Minimum number of arguments required after all options have been
    ///   processed.
    std::optional<int> min_args;
    /// @brief Maximum number of arguments, or zero which means no limit.
    std::optional<int> max_args;
    using valid_options_t = std::map<std::string, Option>;
    /// @brief Map of option strings (with the double hyphen prefix) to Option
    ///   objects.
    valid_options_t valid_options;
  };

  /// @brief The optional name of the program.  Used in the usage string.
  std::optional<std::string> _program_name;
  /// @brief List of groups to consider when parsing.
  std::vector<Group> _groups;
  /// @brief The current group.
  Group _group;
  /// @brief List of errors while processing groups.  There may be up to the
  ///   number of groups number of errors in this list.
  std::vector<std::string> _errors;

  ///
  /// @brief Find an option in a group.
  ///
  /// @param arg The option argument to search for.
  /// @param group The option group to search.
  ///
  /// @return An optional pair of an iterator to the Option found as well as an
  ///   optional option value.
  ///
  std::optional<std::pair<Group::valid_options_t::iterator, std::optional<std::string>>> find_option(
    const std::string& arg, Group& group)
  {
    auto o = [](std::optional<std::string> s = {}) { return s; };
    auto opt = group.valid_options.find(arg);
    if(opt != group.valid_options.end())
      return std::make_pair(opt, o());
    auto pos = arg.find_first_of('=');
    if(pos == std::string::npos)
      return {};
    auto left = arg.substr(0, pos);
    auto right = arg.substr(pos + 1);
    opt = group.valid_options.find(left);
    if(opt != group.valid_options.end())
      return std::make_pair(opt, o(right));
    return {};
  }

  ///
  /// @brief Parse the range of arguments against the option group.
  ///
  /// @param first, last
  ///   The range of elements to parse.
  /// @param group
  ///   Group of options to consider.
  /// @throws Throws an 'argument_error' exception if there is anything wrong
  ///   such as illegal option, missing option parameter.
  ///
  /// @return The remaning unprocessed arguments.
  ///
  args_t::iterator parse(args_t::iterator first, args_t::iterator last, Group& group)
  {
    std::vector<Option*> options;
    Option* current_option = nullptr;
    for(;first != last; ++first)
    {
      if(current_option)
      {
        // Set the option value
        current_option->value = *first;
        current_option->set = true;
        options.push_back(current_option);
        current_option = nullptr;
      }
      else if(auto opt = find_option(*first, group); opt)
      {
        // Previous option taking an argument didn't get the argument
        if(current_option)
          throw argument_error("missing option value: " + current_option->name());
        // If the option takes an argument, set current_option
        if(opt->first->second.argument())
        {
          if(opt->second)
          {
            opt->first->second.set = true;
            opt->first->second.value = *opt->second;
            options.push_back(&opt->first->second);
          }
          else
            current_option = &opt->first->second;
        }
        else if(opt->second)
          throw argument_error("illegal option value: " + *first + "=" + *opt->second);
        else
        {
          options.push_back(&opt->first->second);
          opt->first->second.set = true;
        }
      }
      else if(*first == "--")
        return exec(++first, last, group, options);
      else if(!first->empty() && first->at(0) == '-')
        throw argument_error("unknown option: " + *first);
      else
        return exec(first, last, group, options);
    }
    for(auto& o: group.valid_options)
      if(o.second.required && !o.second.set)
        throw argument_error("missing required argument: " + o.second.name());
    // Last option taking an argument didn't get the argument
    if(current_option)
      throw argument_error("missing option value: " + current_option->name());
    return exec(first, last, group, options);
  }

  ///
  /// @brief Executes the functions associated with the option if all critera
  /// are met.
  ///
  /// @details
  ///   Verifies that the number of arguments in the range satisfies the group
  ///   criteria for min and max number of arguments.  If within range each
  ///   option is processed, meaning the callback function associated with each
  ///   option is called.  Finally, the first iterator to the first non-option
  ///   argument is returned to the caller.
  ///
  /// @param first, last
  ///   The range of arguments.
  /// @param group
  ///   The group being processed.
  /// @param options
  ///   The list of options given on the command line with any option values.
  ///
  /// @return The remaining list of arguments.
  ///
  args_t::iterator exec(args_t::iterator first, args_t::iterator last, Group& group, std::vector<Option*>& options)
  {
    auto distance = std::distance(first, last);
    if(group.min_args)
    {
      auto min_args = *group.min_args;
      if(distance < min_args)
        usage();
      if(group.max_args)
      {
        auto max_args = *group.max_args;
        if(distance > max_args)
          usage();
      }
    }
    else if(distance > 0)
      usage();
    for(const auto* o: options)
      o->exec();
    return first;
  }
};

} // namespace kuri::option
