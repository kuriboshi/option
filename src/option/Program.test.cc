#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "Program.hh"

using namespace kuri::option;
using namespace std::literals;

TEST_CASE("One optional boolean option")
{
  bool test = false;
  std::vector<std::string> args = {"--test"};
  Program program({args.begin(), args.end()}, "test");
  REQUIRE(!test);
  program.optional("--test", [&]() { test = true; });
  auto result = program.parse();
  SECTION("Verify options")
  {
    REQUIRE(test);
    REQUIRE(result.first == result.second);
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
    Program program({args.begin(), args.end()}, "test");
    REQUIRE(!test);
    program.required("--test", [&]() { test = true; });
    auto result = program.parse();
    SECTION("Verify options")
    {
      REQUIRE(test);
      REQUIRE(result.first == result.second);
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
  bool test = false;
  std::vector<std::string> args = {};
  Program program({args.begin(), args.end()}, "test");
  REQUIRE(!test);
  program.required("--test", [&]() { test = true; });
  {
    REQUIRE_THROWS_WITH(
      program.parse(),
      "missing required argument: --test\nusage: test --test"s);
  }
}

TEST_CASE("Single option taking a value")
{
  std::string value;
  std::vector<std::string> args = {"--value", "value"};
  Program program({args.begin(), args.end()}, "test");
  REQUIRE(value.empty());
  auto result = program.optional("--value", [&](const Option& o) { value = o.value; }).parse();
  REQUIRE(value == "value");
  REQUIRE(result.first == result.second);
}

TEST_CASE("Bad boolean option")
{
  bool test = false;
  std::vector<std::string> args = {"--bad"};
  Program program({args.begin(), args.end()}, "test");
  REQUIRE(!test);
  program.optional("--test", [&]() { test = true; });
  REQUIRE_THROWS(program.parse());
}

TEST_CASE("Test --option=value syntax")
{
  std::string value;
  std::vector<std::string> args = {"--option=value"};
  Program program({args.begin(), args.end()}, "test");
  REQUIRE(value.empty());
  program.required("--option", [&](const Option& o) { value = o.value; });
  auto result = program.parse();
  CHECK(value == "value");
}

TEST_CASE("Test list of arguments")
{
  std::string value;
  std::vector<std::string> args = {"1", "2"};
  Program program({args.begin(), args.end()}, "test");
  auto result = program
    .args(1)
    .required("--help", [&]() { program.usage(); })
    .parse();
  REQUIRE(std::distance(result.first, result.second) == 2);
  CHECK(*result.first++ == "1"s);
  CHECK(*result.first == "2"s);
}

int main( int argc, char* argv[] ) {
  int result = Catch::Session().run( argc, argv );
  return result;
}
