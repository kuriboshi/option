//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <sstream>
#include <stdexcept>

namespace wani::option
{
namespace detail
{
void usage_prefix(bool prefix, const std::string& program_name, std::ostringstream&);

template<typename T>
void usage0(bool prefix, const std::string& program_name, std::ostringstream& os, const T& last)
{
  usage_prefix(prefix, program_name, os);
  os << last;
  throw std::runtime_error(os.str());
}

template<typename T, typename... Args>
void usage0(bool prefix, const std::string& program_name, std::ostringstream& os,
  const T& first, const Args&... args)
{
  usage_prefix(prefix, program_name, os);
  os << first << std::endl;
  usage0(false, program_name, os, args...);
}

} // namespace detail

std::string program_name();
void usage();

template<typename... Args>
void usage(const Args&... args)
{
  std::ostringstream os;
  detail::usage0(true, program_name(), os, args...);
}

} // namespace wani::option
