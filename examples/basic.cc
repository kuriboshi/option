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
