//
// Copyright (c) 2021 Krister Joas
//

#include "usage.hh"

namespace wani::option::detail
{
void usage_prefix(bool prefix, const std::string& program_name, std::ostringstream& os)
{
  if(prefix)
    os << "usage: ";
  else
    os << "       ";
  os << program_name << ' ';
}

} // namespace wani::option::detail
