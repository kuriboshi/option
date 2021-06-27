//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>

namespace kuri::option
{
class Error
{
public:
  Error() {}
  Error(std::vector<std::string> error) : error(error) {}
  std::vector<std::string> error;
};

class usage_error: public std::exception
{
public:
  usage_error(const Error& error, const std::string& usage): _error(error), _usage(usage)
  {
    if(_error.error.empty())
      _what = usage;
    else
    {
      _what = *_error.error.begin();
      _what += "\n";
      _what += _usage;
    }
  }

  virtual const char* what() const noexcept override
  {
    return _what.c_str();
  }
  const Error& error() const { return _error; }
  const std::string& usage() const { return _usage; }

private:
  Error _error;
  std::string _usage;
  std::string _what;
};

// This function must be declared by the user of this library in the namespace
// kuri::option.  It should return the name of the program which is to appear
// in any usage message.
std::string program_name();

namespace detail
{
// When the usage message is printed only the first line will contain the
// string "usage:'.  This helper function takes care of that.
inline void usage_prefix(bool prefix, std::ostringstream& os)
{
  if(prefix)
    os << "usage: ";
  else
    os << "       ";
  os << program_name() << ' ';
}

// A special overload when we have a vector of strings, each corresponding to
// one line in the usage message.  This vector of strings is built at runtime.
inline void usage0(bool prefix, std::ostringstream& os, const Error& error, const std::vector<std::string>& list)
{
  bool first = true;
  for(auto i: list)
  {
    if(first)
      first = false;
    else
      os << std::endl;
    usage_prefix(prefix, os);
    os << i;
    prefix = false;
  }
  throw usage_error(error, os.str());
}

// Template which ends the expansion of usage strings.  It then throws an
// exception which is intended to be caught by the main program which can then
// print the generated usage string on the terminal.
template<typename T>
void usage0(bool prefix, std::ostringstream& os, const Error& error, const T& last)
{
  usage_prefix(prefix, os);
  os << last;
  throw usage_error(error, os.str());
}

// When this function is called for the first argument in the parameter pack
// the prefix bool will be true and the "usage:" prefix string will be added.
// For all subsequent calls the prefix bool will be false.
template<typename T, typename... Args>
void usage0(bool prefix, std::ostringstream& os, const Error& error, const T& first, const Args&... args)
{
  usage_prefix(prefix, os);
  os << first << std::endl;
  usage0(false, os, error, args...);
}

} // namespace detail

// Can be called from any part of the program to signal a usage error.  Each
// argument in the parameter pack is printed on its own line.  May also be
// called from inside the option library.
template<typename... Args>
void usage(const Args&... args)
{
  std::ostringstream os;
  Error error;
  detail::usage0(true, os, error, args...);
}

template<typename... Args>
void usage(const Error& error, const Args&... args)
{
  std::ostringstream os;
  detail::usage0(true, os, error, args...);
}

} // namespace kuri::option
