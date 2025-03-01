add_library(ctclp_props INTERFACE)
target_compile_features(ctclp_props INTERFACE cxx_std_23)
target_include_directories(ctclp_props INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_include_directories(ctclp_props INTERFACE ${ctre_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ctclp_props)
