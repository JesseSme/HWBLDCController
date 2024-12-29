# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(HelloWorldSystemC)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include SystemC directories
include_directories(/usr/include)
link_directories(/usr/lib/x86_64-linux-gnu)

# Specify the source file
set(SOURCE spi_main.cpp)

# Create the executable
add_executable(hello_world ${SOURCE})

# Link SystemC library
target_link_libraries(hello_world systemc m)
