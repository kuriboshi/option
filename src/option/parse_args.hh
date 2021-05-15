#pragma once

#include <tuple>
#include <memory>
#include <vector>
#include <map>

#include "usage.hh"

namespace wani
{
using args_t = std::vector<std::string>;
using args_range_t = std::pair<args_t::iterator, args_t::iterator>;

template<typename Base, typename C>
void parse_command(Base& base, const C& commands, const args_range_t& args)
{
  if(args.first == args.second)
    usage();
  auto c = commands.find(*args.first);
  if(c == commands.end())
    usage();
  c->second(base, {args.first + 1, args.second});
}

} // namespace wani
