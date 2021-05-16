//
// Copyright (c) 2021 Krister Joas
//

#include "usage.hh"

namespace wani::detail
{
void usage_prefix(bool usage, std::ostringstream& os)
{
  if(usage)
    os << "usage: wani ";
  else
    os << "       wani ";
}

} // namespace wani::detail

namespace wani
{
void usage()
{
  // clang-format off
  usage(
    "<command> [<arguments>]",
    "assignment",
    "assignments",
    "lessons",
    "levels",
    "reader",
    "stages",
    "subject",
    "summary");
  // clang-format on
}

} // namespace wani
