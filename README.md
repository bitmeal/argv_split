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



## todo

* write tests
* add CI testing