#pragma once

#include <sstream>

namespace wani
{
namespace detail
{
void usage_prefix(bool usage, std::ostringstream&);

template<typename T>
void usage0(bool usage, std::ostringstream& os, const T& last)
{
  usage_prefix(usage, os);
  os << last;
  throw std::runtime_error(os.str());
}

template<typename T, typename... Args>
void usage0(bool usage, std::ostringstream& os, const T& first, const Args&... args)
{
  usage_prefix(usage, os);
  os << first << std::endl;
  usage0(false, os, args...);
}

} // namespace detail

template<typename... Args>
void usage(const Args&... args)
{
  std::ostringstream os;
  wani::detail::usage0(true, os, args...);
}

void usage();

} // namespace wani
