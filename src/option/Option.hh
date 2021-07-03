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

#include <exception>
#include <functional>
#include <string>
#include <variant>

#include <fmt/format.h>

#include "overloaded.hh"

using namespace std::literals;

namespace kuri::option
{
///
/// @brief Exception used internally to signal any argument parsing error.
///
class argument_error: public std::exception
{
public:
  ///
  /// @brief Exception constructor.
  ///
  /// @param message The exception message.
  argument_error(const std::string& message): _message(message) {}

  ///
  /// @brief Exception message.
  ///
  virtual const char* what() const noexcept { return _message.c_str(); }

private:
  /// @brief The message string.
  std::string _message;
};

///
/// @brief Represents an option with its name, whether it's a required or
///   optional option, and its callback function.
///
class Option
{
public:
  ///
  /// @brief Constructor for a boolean option.
  ///
  /// @param name The name of the option with the hyphen prefixes.
  /// @param required True if the option is required, false if it's optional.
  /// @param f The callback function.  This is a function returning void taking
  ///   no arguments.
  ///
  Option(const std::string& name, bool required, std::function<void()> f): _name(name), required(required), _fun(f) {}
  ///
  /// @brief Constructor for an option taking a value.
  ///
  /// @param name The name of the option with the hyphen prefixes.
  /// @param required True if the option is required, false if it's optional.
  /// @param f The callback function.  This callback function returns void and
  ///   takes a `const Option&` as its parameter.
  ///
  Option(const std::string& name, bool required, std::function<void(const Option&)> f)
    : _name(name), required(required), _fun(f)
  {}
  ///
  /// @brief Default constructor is deleted.
  ///
  Option() = delete;
  ///
  /// @brief Copy constructor is deleted.
  ///
  Option(const Option&) = delete;
  ///
  /// @brief Default destructor.
  ///
  ~Option() = default;
  ///
  /// @brief Move copy constructor.
  ///
  /// @param option The `Option` object to move.
  ///
  Option(Option&& option) noexcept { *this = std::move(option); }
  ///
  /// @brief The move assignment operator.
  ///
  /// @param option The `Option` object to move assign.
  ///
  Option& operator=(Option&& option)
  {
    if(&option != this)
    {
      required = option.required;
      set = option.set;
      value = std::move(option.value);
      _name = std::move(option._name);
      _fun = std::move(option._fun);
    }
    return *this;
  }

  /// @brief The value of the option in case of an option taking a value.
  std::string value;
  /// @brief True if the option is required, false otherwise
  bool required = false;
  /// @brief Used while parsing to check if the value has been set.
  bool set = false;
  ///
  /// @brief Return the name of the option.
  ///
  /// @return Then the name of the option.
  ///
  std::string name() const noexcept { return _name; }
  ///
  /// @brief Returns true if this option takes a value.
  ///
  /// @return True if the option takes a value.
  ///
  bool argument() const noexcept { return _fun.index() == 1; }
  ///
  /// @brief Executes the callback function.  If the option takes an argument
  ///   the callback function will get this `Option` object as it's parameter.
  ///   The callback function can then check the value and process it in any
  ///   way or assign the value to a variable.
  ///
  void exec() const
  {
    // clang-format off
    std::visit(
      overloaded{
        [](std::function<void()> f) { f(); },
        [this](std::function<void(const Option&)> f) { f(*this); }},
      _fun);
    // clang-format on
  }

  ///
  /// @brief Build the help string.
  ///
  /// @return The help string.
  ///
  std::string help() const
  {
    if(required)
      return fmt::format("{}{}", _name, argument() ? " <value>"s : ""s);
    return fmt::format("[{}{}]", _name, argument() ? " <value>"s : ""s);
  }

private:
  /// @brief The name of the option.
  std::string _name;
  /// @brief The callback function.
  std::variant<std::function<void()>, std::function<void(const Option&)>> _fun;
};

} // namespace kuri::option
