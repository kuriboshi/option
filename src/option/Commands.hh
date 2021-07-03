//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <functional>
#include <map>
#include <string>

#include "parse_args.hh"
#include "usage.hh"

namespace kuri::option
{
///
/// @brief The Commands class wraps multiple commands initiated by a single
///   word.
///
template<typename Context>
class Commands
{
public:
  ///
  /// @brief Type of the command callback function.  It is passed the context
  ///   and the range of arguments still to be processed.
  ///
  using function_t = std::function<void(Context&, args_range_t)>;

  ///
  /// @brief Registers a command string and a callback function.
  ///
  /// @param name The name of the command.
  /// @param callback The callback function which is called when dispatching a
  ///   command.
  ///
  Commands& command(const std::string& name, function_t callback)
  {
    _command_list.push_back(name);
    _commands.emplace(name, callback);
    return *this;
  }

  ///
  /// @brief Parse the arguments.
  ///
  /// @param context The context is passed as the first argument of the
  ///   callback function.
  /// @param args The range of arguments as a pair of iterators.
  ///
  void parse(Context& context, const args_range_t& args)
  {
    if(args.first == args.second)
      usage();
    auto c = _commands.find(*args.first);
    if(c == _commands.end())
      usage();
    c->second(context, {args.first + 1, args.second});
  }

private:
  ///
  /// @brief Called when the argument parsing fails.
  ///
  void usage()
  {
    option::usage(_command_list);
  }
  
  /// @brief List of commands in alphabetic order.  Used for the usage message.
  std::vector<std::string> _command_list;
  /// @brief Map of commands to callback functions.
  std::map<std::string, function_t> _commands;
};

} // namespace kuri::option
