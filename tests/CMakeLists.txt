cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)

# name project and provide version number
project(argv_split_tests)
get_filename_component(ARGV_SPLIT_H_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)


########################
# SETUP TESTING
########################
enable_testing()

function(add_cmdline_test TEST_INSTANCE TEST_CMDLINE_STR)
	configure_file(argv_split_test.cpp.in ${TEST_INSTANCE}.cpp ESCAPE_QUOTES)
	add_executable(${TEST_INSTANCE} ${TEST_INSTANCE}.cpp)
	target_include_directories(${TEST_INSTANCE} PRIVATE ${ARGV_SPLIT_H_DIR})
	separate_arguments( TEST_CMDLINE UNIX_COMMAND ${TEST_CMDLINE_STR})
	add_test(${TEST_INSTANCE}_test ${TEST_INSTANCE} ${TEST_CMDLINE})
endfunction(add_cmdline_test)


########################
# ADD TESTS
########################
add_cmdline_test(flags "-a -b -cd")
add_cmdline_test(flags_param "-a foo -b bar -cd")
add_cmdline_test(flags_param_quoted "-a \"foo fuu\" -b \"bar\" -cd")
add_cmdline_test(flags_param_snglquoted "-a 'foo fuu' -b 'bar' -cd")
add_cmdline_test(nested_quotes "\"'single nested'\" positional")
add_cmdline_test(switches_eq "--foo=bar")
add_cmdline_test(switches_eq_quoted "--foo=\"bar bar\"")
add_cmdline_test(switches_eq_snglquoted "--foo='bar bar'")


