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

add_library(ctclp INTERFACE)
target_compile_features(ctclp INTERFACE cxx_std_23)

target_include_directories(ctclp INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${ctre_SOURCE_DIR}/include
)

option(BUILD_EXAMPLES "Build example programs" ON)

if(BUILD_EXAMPLES)
    add_executable(example examples/main.cpp)
    target_link_libraries(example PRIVATE ctclp)
endif()
