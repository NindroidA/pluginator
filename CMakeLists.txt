cmake_minimum_required(VERSION 3.15)
project(Pluginator VERSION 1.0.4)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Version
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp"
)

# Include directories
include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# Create executable
add_executable(pluginator
    src/main.cpp
    src/pluginator.cpp
    src/logger.cpp
)