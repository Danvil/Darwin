cmake_minimum_required(VERSION 2.6)

set(EIGEN3_INCLUDE_DIR "/usr/local/include/eigen3" CACHE STRING "Eigen3 include directory")

add_definitions(-std=c++0x)

project(Darwin)
add_subdirectory(Candy)
add_subdirectory(Tangerin)
