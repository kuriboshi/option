add_library(option)
target_sources(option
  PRIVATE
  src/option/Option.hh
  src/option/Program.hh
  src/option/parse_args.hh
  src/option/string_functions.hh
  src/option/usage.cc
  src/option/usage.hh)
target_link_libraries(option PUBLIC util)
target_include_directories(option PUBLIC "src")

# This library is here to compile the empty .cc files.  They only
# exist to test that the .hh files include all needed headers and they
# don't contain any symbols.
add_library(_option OBJECT)
target_sources(_option
  PRIVATE
  src/option/Option.cc
  src/option/Program.cc
  src/option/parse_args.cc
  src/option/string_functions.cc)
target_link_libraries(_option PRIVATE util)
