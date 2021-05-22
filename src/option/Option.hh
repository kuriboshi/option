//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <exception>
#include <functional>
#include <string>
#include <variant>

#include <fmt/format.h>

#include <util/overloaded.hh>

using namespace std::literals;

namespace wani::option
{
class argument_error: public std::exception
{
public:
  argument_error(const std::string& message): _message(message) {}

  virtual const char* what() const noexcept { return _message.c_str(); }

private:
  std::string _message;
};

class Option
{
public:
  Option(const std::string& name, bool required, std::function<void()> f): _name(name), required(required), _fun(f) {}
  Option(const std::string& name, bool required, std::function<void(const Option&)> f)
    : _name(name), required(required), _fun(f)
  {}
  Option() = delete;
  Option(const Option&) = delete;
  ~Option() = default;
  Option(Option&& option) noexcept { *this = std::move(option); }
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

  std::string name() const { return _name; }
  bool argument() const { return _fun.index() == 1; }
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
  bool required = false;
  bool set = false;
  std::string value;

  std::string help() const
  {
    if(required)
      return fmt::format("{}{}", _name, argument() ? " <value>"s : ""s);
    return fmt::format("[{}{}]", _name, argument() ? " <value>"s : ""s);
  }

private:
  std::string _name;
  std::variant<std::function<void()>, std::function<void(const Option&)>> _fun;
};

} // namespace wani::option
