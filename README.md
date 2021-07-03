# kuri::option -- argument parsing

A simple library handling command line options.  The purpose of this
header only library is to handle the easiest and most common cases of
command line arguments.

* Boolean and options with a string value
* Options taking values accepts `--option value` or `--option=value`
* Only one string per option
* Processing of options through callbacks
* Grouping of options
* Sub commands
* Helper function to parse number ranges (e.g. 1-3,5,7-)
* Min and max number of arguments after the options
* Conventional use of double hyphen (`--`) to signal end of options
* Builds the help and usage string automatically

Let's start with a basic example to illustrate some of the features: A
program which takes two options, one boolean and one with a string
value, followed by an arbitrary number of arguments.

The example is also available [examples/basic.cc](examples/basic.cc).

```cpp
#include <string>
#include <iostream>
#include <iomanip>
#include <optional>
#include <option/Program.hh>

using namespace kuri;

int main(int argc, char** argv)
{
  try
  {
    // Collect the arguments from `argv` into a vector<string>.
    //
    option::args_t args{argv + 1, argv + argc};
    //
    // Define the variables which we will set when the options are processed.
    //
    auto verbose = false;
    std::optional<std::string> print;
    //
    // Create the `Program` object and add the options.  Finally we call the
    // `parse` member function to do the parsing.  The return value is a pair
    // of iterators in the original vector of arguments.
    //
    auto result = option::Program({args.begin(), args.end()}, "main")
      .optional("--verbose", [&]() { verbose = true; })
      .optional("--print", [&](const option::Option& o) {
        print = o.value;
      })
      .args()
      .parse();
    //
    // Now print the result of
    //
    std::cout << "verbose = " << std::boolalpha << verbose << '\n';
    if (print)
      std::cout << "print = " << *print << '\n';
    for(auto i = result.first; i != result.second; ++i)
      std::cout << "arg = " << *i << '\n';
  }
  catch(const option::usage_error& e)
  {
    // In case of errors `Program` throws the exception `usage_error`.
    //
    std::cerr << e.what() << '\n';
  }
}
```
