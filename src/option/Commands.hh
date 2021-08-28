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
  /// @brief Commands constructor.
  ///
  /// @param program_name
  ///   The name of the program.  This will be included in the usage string.
  ///
  Commands(const std::optional<std::string> program_name = {})
    : _program_name(program_name)
  {
  }
  ///
  /// @brief Type of the command callback function.
  ///
  /// @details
  ///   The function is passed the context and two iterators representing the
  ///   range of arguments still to be processed.
  ///
  /// @param context
  ///   The context.  This can be anything and is passed to the callback
  ///   function as is.
  /// @param first, last
  ///   The range of command line arguments to parse.
  ///
  using function_t = std::function<void(Context& context, args_t::iterator first, args_t::iterator last)>;

  ///
  /// @brief Registers a command string and a callback function.
  ///
  /// @param name
  ///   The name of the command.
  /// @param callback
  ///   The callback function which is called when dispatching a command.
  ///
  Commands& command(const std::string& name, function_t callback)
  {
    _command_list.push_back(_program_name ? cat(*_program_name, name) : name);
    _commands.emplace(name, callback);
    return *this;
  }

  ///
  /// @brief Parse the arguments.
  ///
  /// @param context
  ///   The context is passed as the first argument of the callback function.
  /// @param first, last
  ///   The range of arguments to parse.
  ///
  void parse(Context& context, args_t::iterator first, args_t::iterator last)
  {
    if(first == last)
      usage();
    auto c = _commands.find(*first);
    if(c == _commands.end())
      usage();
    c->second(context, first + 1, last);
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
  /// @brief Name of the program.
  std::optional<std::string> _program_name;

  ///
  /// @brief Catenate an arbitrary number of strings.
  ///
  /// @tparam T
  ///   Type of the last item.
  /// @param last
  ///   The last item.
  ///
  /// @return The catenated string.
  ///
  template<typename T>
  std::string cat(const T& last)
  {
    return last;
  }

  ///
  /// @brief Catenate an arbitrary number of strings.
  ///
  /// @tparam T
  ///   Type of the first item.
  /// @tparam As
  ///   Type of the rest of the items.
  /// @param first, rest
  ///   The first item and rest of the parameter pack.
  ///
  /// @return The catenated string.
  ///
  template<typename T, typename... As>
  std::string cat(const T& first, const As&... rest)
  {
    return first + ' ' + cat(rest...);
  }
};

} // namespace kuri::option
