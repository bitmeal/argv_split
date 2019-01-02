[![Build Status](https://travis-ci.com/bitmeal/argv_split.svg?branch=master)](https://travis-ci.com/bitmeal/argv_split)  
[ G++-5/6/7 ]

# argv split
Simple parser to split a command line string into its components, like you'll find them in `argv`. Splits on whitespace and keeps quoted parts together. Supports single and double quotes; quotes can be nested (one level, eg: `"foo 'bar'"`) and will not be treated as quotes if escaped by a backslash (`\"` or `\'`).  
Tests are in `tests` directory and use CMake-integrated testing; call `make all` and `make test`.
  
## use
Header only. Include and use as shown. Parameter to the constructor can be omited, but your arguments array will be missing the program name; you can allway specify it explicitly in your arguments string.
```C++
argv_parser parser("programm_name");
parser.parse("-p \"my flag param\" --switch='switch value'");
const char** my_argv = parser.argv();
```
  
## tests
Uses CMake-integrated testing: Tests are configured in `tests/CMakeLists.txt`, using the provided function `add_cmdline_test( <test_name> "<quoted command line string>" )`. This will generate a test-units C++ code, including the given string of arguments. On execution, the arguments given on the command line and the arguments extracted from the given string are compared - test passes if they match.