//
// Copyright (c) 2021 Krister Joas
//

#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <set>

namespace wani::option
{
inline std::vector<std::string> split_string(const std::string& s, char delim, bool include_empties = false)
{
  std::vector<std::string> result;
  std::string item;
  for(auto& i: s)
  {
    if(i == delim)
    {
      if(include_empties || !item.empty())
      {
        result.push_back(item);
        item.clear();
      }
    }
    else
      item.push_back(i);
  }
  if(include_empties || !item.empty())
    result.push_back(item);
  return result;
}

inline std::set<int> numeric_range(const std::string& s, int min, int max)
{
  std::set<int> result;
  auto comma_ranges = split_string(s, ',');
  for(auto i: comma_ranges)
  {
    auto dash_ranges = split_string(i, '-', true);
    if(dash_ranges.size() > 2)
      throw std::runtime_error("bad range: " + s);
    auto first = dash_ranges[0].empty() ? min : std::stoi(dash_ranges[0]);
    if(dash_ranges.size() == 2)
    {
      auto last = dash_ranges[1].empty() ? max : std::stoi(dash_ranges[1]);
      for(; first <= last; ++first) result.insert(first);
    }
    else
      result.insert(first);
  }
  return result;
}

} // namespace wani::option
