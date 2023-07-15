cmake_minimum_required(VERSION 3.12)
project(checkmint)

# Create a target for the header-only library
add_library(checkmint INTERFACE)


target_include_directories(checkmint INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(checkmint INTERFACE cxx_std_20)

# Install the header file
# Install the header file into a dedicated folder
set(INCLUDE_INSTALL_DIR "include/checkmint")
install(FILES checkmint.hpp DESTINATION ${INCLUDE_INSTALL_DIR})

