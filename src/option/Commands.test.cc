#include <catch2/catch.hpp>

#include "Commands.hh"

using namespace kuri::option;
using namespace std::literals;

void test0(int& context, const args_range_t& rest) { context = 0; }
void test1(int& context, const args_range_t& rest) { context = 1; }
void test2(int& context, const args_range_t& rest) { context = 2; }
void test3(int& context, const args_range_t& rest) { context = 3; }

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
    commands.parse(context, {args.begin(), args.end()});
    CHECK(context == 0);
  }
  SECTION("Lookup test1")
  {
    std::vector<std::string> args = {"test1"};
    commands.parse(context, {args.begin(), args.end()});
    CHECK(context == 1);
  }
  SECTION("Lookup test2")
  {
    std::vector<std::string> args = {"test2"};
    commands.parse(context, {args.begin(), args.end()});
    CHECK(context == 2);
  }
  SECTION("Lookup test3")
  {
    std::vector<std::string> args = {"test3"};
    commands.parse(context, {args.begin(), args.end()});
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
    commands.parse(context, {args.begin(), args.end()}),
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
    commands.parse(context, {args.begin(), args.end()}),
    "usage: test0\n"
    "       test1\n"
    "       test2\n"
    "       test3");

}
