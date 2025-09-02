cmake_minimum_required(VERSION 3.15)
project(Pluginator VERSION 1.1.10)

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

# Find required pkgs
find_package(CURL REQUIRED)

# Create executable
add_executable(pluginator
    src/main.cpp
    src/pluginator.cpp
    src/logger.cpp
    src/config.cpp
    src/lang.cpp
    src/http_client.cpp
    src/json_helper.cpp
    src/plugin_manager.cpp
    src/purpur_manager.cpp
    src/progress_bar.cpp
    src/utils.cpp
)

# Link libs
target_link_libraries(pluginator CURL::libcurl)

# Additional includes
if(APPLE)
    target_link_libraries(pluginator "-framework CoreFoundation" "-framework Security")
endif()