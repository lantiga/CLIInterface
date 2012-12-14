cmake_minimum_required(VERSION 2.8)

project(CLIInterface)

add_subdirectory(Utilities/tinyxml2)
add_subdirectory(Utilities/tclap)

include_directories(Code)
include_directories(Utilities/tinyxml2)
include_directories(Utilities/tclap/include)

add_executable(cli_sample Testing/cli_sample.cpp)

target_link_libraries(cli_sample tinyxml2)

configure_file(Testing/cli_sample.xml ${CLIInterface_BINARY_DIR}/cli_sample.xml COPYONLY)
