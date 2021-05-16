//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <exception>
#include <functional>
#include <string>
#include <variant>

#include <util/overloaded.hh>

namespace wani::option
{
class argument_error: public std::exception
{
public:
  virtual const char* what() const noexcept { return "bad argument"; }
};

class Option
{
public:
  Option(const std::string& name, std::function<void()> f): _name(name), _fun(f) {}
  Option(const std::string& name, std::function<void(const Option&)> f): _name(name), _fun(f) {}
  Option() = delete;
  Option(const Option&) = delete;
  ~Option() = default;
  Option(Option&& option) noexcept { *this = std::move(option); }
  Option& operator=(Option&& option)
  {
    if(&option != this)
    {
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
  bool set = false;
  std::string value;

private:
  std::string _name;
  std::variant<std::function<void()>, std::function<void(const Option&)>> _fun;
};

} // namespace wani::option
