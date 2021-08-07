// Copyright 2021 Krister Joas
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <filesystem>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace kuri::option
{
///
/// @brief Split a string at the delimeter character.
///
/// @details
///   Split a string and return as a vector<string>.  If the final argument
///   include_empties is true then multiple consecutive delimiters results in
///   empty strings in the result vector.
///
/// @param s
///   String to be split.
/// @param delim
///   The delimiter used for splitting the string.
/// @param include_empties
///   False means multiple consecutive delimiters are folded into one.  True
///   will instead put empty strings in the return collection.  Default is
///   false.
///
/// @return A vector of strings split at the delimiter.  If `include_empties`
///   is true then the vector may contain empty strings.
///
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

///
/// @brief Parse a string which contains a description of a set of numbers.
///
/// @details
///   The description is a sequence of either numbers or ranges of numbers
///   separated by commas.  The ranges are two numbers separated by a hyphen.
///   The min and max arguments makes it possible to parse open ranges that
///   starts or ends with a hyphen.
///
/// @param s
///   A string containing a range of numbers.
/// @param min
///   The minimum number allowed.  Used to handle the open ended `-n` case.
/// @param max
///   The maximum number allowed.  Used to handle the open ended `n-` case.
///
/// @return A set of integers defined by the range.
///
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

///
/// @brief Utility function which returns the filename of the path.
///
/// @details
///   Extract the final component of a path passed in as parameter.  This is
///   meant to optionally be used when passed as the name of the program in the
///   Program class.  Then usage message will print only the name and not
///   include the path.
///
/// @param path
///   A string containing a path.
/// @return The basename, i.e. the filename of the path.
///
inline std::string basename(const std::string& path)
{
  std::filesystem::path p(path);
  return p.filename().string();
}

} // namespace kuri::option
