cmake_minimum_required(VERSION 3.30)

set(PROJECT_NAME ctclp)
project(${PROJECT_NAME} LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  ctre
  GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
  GIT_TAG main
)
FetchContent_MakeAvailable(ctre)

add_subdirectory(src)
