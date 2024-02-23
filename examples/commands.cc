// Copyright 2021, 2024 Krister Joas
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

#include <algorithm>
#include <string>
#include <iostream>
#include <iomanip>
#include <optional>
#include <option/Program.hh>
#include <option/Commands.hh>

using namespace kuri;

void first(int&, option::args_t::iterator a, option::args_t::iterator b)
{
  auto verbose = false;
  std::optional<std::string> print;
  auto result = option::Program()
    .optional("--verbose", [&]() { verbose = true; })
    .optional("--print", [&](const option::Option& o) {
      print = o.value;
    })
    .args()
    .parse(a, b);
  std::cout << "verbose: " << verbose << '\n';
  std::cout << "print: " << (print ? *print : std::string{"-"}) << '\n';
}

void second(int&, option::args_t::iterator, option::args_t::iterator)
{
  option::usage("second");
}

int main(int argc, char** argv)
{
  try
  {
    // Collect the arguments from `argv` into a vector<string>.
    //
    option::args_t args{argv + 1, argv + argc};
    //
    option::Commands<int> commands(option::basename(argv[0]));
    int i = 0;
    commands.command("first", first)
      .command("second", second)
      .parse(i, args.begin(), args.end());
  }
  catch(const option::usage_error& e)
  {
    // In case of errors `Program` throws the exception `usage_error`.
    //
    std::cerr << e.what() << '\n';
  }
}
