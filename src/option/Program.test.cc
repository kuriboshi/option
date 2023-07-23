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

#define CATCH_CONFIG_RUNNER
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "Program.hh"

using namespace kuri::option;
using namespace std::literals;

TEST_CASE("One optional boolean option")
{
  bool test = false;
  std::vector<std::string> args = {"--test"};
  Program program("test");
  REQUIRE(!test);
  program.optional("--test", [&]() { test = true; });
  auto result = program.parse(args.begin(), args.end());
  SECTION("Verify options")
  {
    REQUIRE(test);
    REQUIRE(result == args.end());
  }
  SECTION("Verify help string")
  {
    auto help = program.help();
    REQUIRE(help.size() == 1);
    REQUIRE(help[0] == "test [--test]");
  }
}

TEST_CASE("One required boolean option")
{
  bool test = false;
  SECTION("Provide the require option")
  {
    std::vector<std::string> args = {"--test"};
    Program program("test");
    REQUIRE(!test);
    program.required("--test", [&]() { test = true; });
    auto result = program.parse(args.begin(), args.end());
    SECTION("Verify options")
    {
      REQUIRE(test);
      REQUIRE(result == args.end());
    }
    SECTION("Verify help string")
    {
      auto help = program.help();
      REQUIRE(help.size() == 1);
      REQUIRE(help[0] == "test --test");
    }
  }
}

TEST_CASE("Leave required option out")
{
  Program program("test");
  bool test = false;
  REQUIRE(!test);
  program.required("--test", [&]() { test = true; });
  std::vector<std::string> args = {};
  REQUIRE_THROWS_WITH(
    program.parse(args.begin(), args.end()), "missing required argument: --test\nusage: test --test"s);
}

TEST_CASE("Single option taking a value")
{
  Program program("test");
  std::string value;
  REQUIRE(value.empty());
  std::vector<std::string> args = {"--value", "value"};
  auto result = program.optional("--value", [&](const Option& o) { value = o.value; }).parse(args.begin(), args.end());
  REQUIRE(value == "value");
  REQUIRE(result == args.end());
}

TEST_CASE("Bad boolean option")
{
  Program program("test");
  bool test = false;
  REQUIRE(!test);
  std::vector<std::string> args = {"--bad"};
  program.optional("--test", [&]() { test = true; });
  REQUIRE_THROWS(program.parse(args.begin(), args.end()));
}

TEST_CASE("Test --option=value syntax")
{
  Program program("test");
  std::string value;
  REQUIRE(value.empty());
  program.required("--option", [&](const Option& o) { value = o.value; });
  std::vector<std::string> args = {"--option=value"};
  auto result = program.parse(args.begin(), args.end());
  CHECK(value == "value");
}

TEST_CASE("Test list of arguments")
{
  Program program("test");
  std::string value;
  REQUIRE(value.empty());
  std::vector<std::string> args = {"1", "2"};
  auto result = program.args(1).required("--help", [&]() { program.usage(); }).parse(args.begin(), args.end());
  REQUIRE(std::distance(result, args.end()) == 2);
  CHECK(*result++ == "1"s);
  CHECK(*result == "2"s);
}

TEST_CASE("Extra arguments")
{
  Program program("test");
  program.args(0, 0);
  std::vector<std::string> args = {"1", "2"};
  REQUIRE_THROWS(program.parse(args.begin(), args.end()));
}

TEST_CASE("No min, only max arguments")
{
  Program program("test");
  program.args(0, 1);
  {
    std::vector<std::string> args = {"1", "2"};
    CHECK_THROWS(program.parse(args.begin(), args.end()));
  }
  {
    std::vector<std::string> args = {};
    CHECK_NOTHROW(program.parse(args.begin(), args.end()));
  }
  {
    std::vector<std::string> args = {"1"};
    CHECK_NOTHROW(program.parse(args.begin(), args.end()));
  }
}

TEST_CASE("Help string for arguments")
{
  Program program("test");
  program.args();               // No arguments
  program.args(1);              // At least one argument
  program.args(1, 1);           // Only one argument
  program.args(2, 3);           // Two or three arguments
  program.args(0, 2);           // Zero, one, or two arguments
  program.args(0);              // Any number of arguments
  CHECK_THROWS(program.args({}, 2)); // Invalid
  CHECK_THROWS(program.args(3, 2));  // Invalid
  auto help = program.help();
  CHECK(help[0] == "test");
  CHECK(help[1] == "test <arg> [<arg>...]");
  CHECK(help[2] == "test <arg>");
  CHECK(help[3] == "test <arg> <arg> [<arg>]");
  CHECK(help[4] == "test [<arg> [<arg>]]");
  CHECK(help[5] == "test [<arg>...]");
}

int main(int argc, char* argv[])
{
  int result = Catch::Session().run(argc, argv);
  return result;
}
