cmake_minimum_required(VERSION 3.20.x)

project(DumpHelldiversGeneratedData VERSION 0.0.1 LANGUAGES CXX)

include(cmake/glaze.cmake)

#add_executable(${PROJECT_NAME} src/main.cpp)

add_library(${PROJECT_NAME} MODULE src/main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)

target_link_libraries(${PROJECT_NAME} PRIVATE glaze::glaze)