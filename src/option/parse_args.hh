//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <tuple>
#include <memory>
#include <vector>
#include <map>

namespace kuri::option
{
using args_t = std::vector<std::string>;
using args_range_t = std::pair<args_t::iterator, args_t::iterator>;

} // namespace kuri::option
