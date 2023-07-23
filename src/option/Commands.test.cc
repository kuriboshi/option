// Copyright 2021, 2023 Krister Joas
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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "Commands.hh"

using namespace kuri::option;
using namespace std::literals;

void test0(int& context, args_t::iterator, args_t::iterator) { context = 0; }
void test1(int& context, args_t::iterator, args_t::iterator) { context = 1; }
void test2(int& context, args_t::iterator, args_t::iterator) { context = 2; }
void test3(int& context, args_t::iterator, args_t::iterator) { context = 3; }

TEST_CASE("Check correct callback is called")
{
  Commands<int> commands("test");
  commands.command("test0", test0);
  commands.command("test1", test1);
  commands.command("test2", test2);
  commands.command("test3", test3);
  int context;
  SECTION("Lookup test0")
  {
    std::vector<std::string> args = {"test0"};
    commands.parse(context, args.begin(), args.end());
    CHECK(context == 0);
  }
  SECTION("Lookup test1")
  {
    std::vector<std::string> args = {"test1"};
    commands.parse(context, args.begin(), args.end());
    CHECK(context == 1);
  }
  SECTION("Lookup test2")
  {
    std::vector<std::string> args = {"test2"};
    commands.parse(context, args.begin(), args.end());
    CHECK(context == 2);
  }
  SECTION("Lookup test3")
  {
    std::vector<std::string> args = {"test3"};
    commands.parse(context, args.begin(), args.end());
    CHECK(context == 3);
  }
}

TEST_CASE("Lookup command should fail")
{
  Commands<int> commands("test");
  commands.command("test0", test0);
  commands.command("test1", test1);
  commands.command("test2", test2);
  commands.command("test3", test3);
  std::vector<std::string> args = {"test_x"};
  int context;
  REQUIRE_THROWS_WITH(
    commands.parse(context, args.begin(), args.end()),
    "usage: test test0\n"
    "       test test1\n"
    "       test test2\n"
    "       test test3");

}

TEST_CASE("Lookup command should fail with no program name")
{
  Commands<int> commands;
  commands.command("test0", test0);
  commands.command("test1", test1);
  commands.command("test2", test2);
  commands.command("test3", test3);
  std::vector<std::string> args = {"test_x"};
  int context;
  REQUIRE_THROWS_WITH(
    commands.parse(context, args.begin(), args.end()),
    "usage: test0\n"
    "       test1\n"
    "       test2\n"
    "       test3");

}
