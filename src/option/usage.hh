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
///
/// @brief Represents multiple errors with an error string for each error.
///
class Error
{
public:
  ///
  /// @brief Default constructor with no errors.
  ///
  Error() {}
  ///
  /// @brief Constructor taking a vector of error strings.
  ///
  Error(std::vector<std::string> error) : error(error) {}
  /// @brief The collection of error strings.
  std::vector<std::string> error;
};

///
/// @brief Signals a argument parsing error.
///
class usage_error: public std::exception
{
public:
  ///
  /// @brief The exception constructor.
  ///
  /// @param error The error object with zero of more error messages.
  /// @param usage The usage string which is printed after any error messages.
  ///
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

  ///
  /// @brief Returns the error string built by the constructor.
  ///
  /// @return The error string.
  ///
  virtual const char* what() const noexcept override
  {
    return _what.c_str();
  }
  ///
  /// @brief Return the `Error` object.
  ///
  /// @return The `Error` object.
  ///
  const Error& error() const { return _error; }
  ///
  /// @brief Return the usage string.
  ///
  /// @return The usage string.
  ///
  const std::string& usage() const { return _usage; }

private:
  /// @brief The `Error` object.
  Error _error;
  /// @brief The usage string.
  std::string _usage;
  /// @brief The string used for the `what` function.
  std::string _what;
};

namespace detail
{
///
/// @brief When the usage message is printed only the first line will contain
///   the string "usage:'.  This helper function takes care of that.
///
/// @param prefix True if the "usage:" prefix should be printed.
/// @param os The usage messages is collected in this `ostringstream`.
///
inline void usage_prefix(bool prefix, std::ostringstream& os)
{
  if(prefix)
    os << "usage: ";
  else
    os << "       ";
}

///
/// @brief A special overload when we have a vector of strings, each
///   corresponding to one line in the usage message.  This vector of strings
///   is built at runtime.
///
/// @param prefix True if the "usage:" prefix should be printed.
/// @param os The usage messages is collected in this `ostringstream`.
/// @param error The `Error` object with its collection of error messages.
/// @param list A list of usage messages when we have multiple `Command`
///   objects to process.
///
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

///
/// @brief Template which ends the expansion of usage strings.  It then throws
///   an exception which is intended to be caught by the main program which can
///   then print the generated usage string on the terminal.
///
/// @tparam T The type of the last item.
/// @param prefix True if the "usage:" prefix should be printed.
/// @param error The `Error` object with its collection of error messages.
/// @param last The last item to add to the usage message.
///
template<typename T>
void usage0(bool prefix, std::ostringstream& os, const Error& error, const T& last)
{
  usage_prefix(prefix, os);
  os << last;
  throw usage_error(error, os.str());
}

///
/// @brief When this function is called for the first argument in the parameter
///   pack the prefix bool will be true and the "usage:" prefix string will be
///   added.  For all subsequent calls the prefix bool will be false.
///
/// @tparam T The type of the first item.
/// @tparam Args The rest of the parameter pack.
/// @param prefix True if the "usage:" prefix should be printed.
/// @param os The usage messages is collected in this `ostringstream`.
/// @param error The `Error` object with its collection of error messages.
/// @param first The first item to add to the usage message.
/// @param args The rest of the parameter pack.
///
template<typename T, typename... Args>
void usage0(bool prefix, std::ostringstream& os, const Error& error, const T& first, const Args&... args)
{
  usage_prefix(prefix, os);
  os << first << std::endl;
  usage0(false, os, error, args...);
}

} // namespace detail

///
/// @brief Can be called from any part of the program to signal a usage error.
///   Each argument in the parameter pack is printed on its own line.  May also
///   be called from inside the option library.  This overload doesn't take an
///   `Error` object and instead passes an empty such object the to the
///   `usage0` function.
///
/// @tparam Args The types of the parameter pack.
/// @param args The parameter pack.
///
template<typename... Args>
void usage(const Args&... args)
{
  std::ostringstream os;
  Error error;
  detail::usage0(true, os, error, args...);
}

///
/// @brief Can be called from any part of the program to signal a usage error.
///   Each argument in the parameter pack is printed on its own line.  May also
///   be called from inside the option library.
///
/// @tparam Args The types of the parameter pack.
/// @param error The `Error` object with its error messages.
/// @param args The parameter pack.
///
template<typename... Args>
void usage(const Error& error, const Args&... args)
{
  std::ostringstream os;
  detail::usage0(true, os, error, args...);
}

} // namespace kuri::option
