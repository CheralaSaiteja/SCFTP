cmake_minimum_required(VERSION 3.18)
project(ConfigTool)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.c)

add_subdirectory(deps/cJSON/)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC deps/cJSON/)
target_link_libraries(${PROJECT_NAME} PUBLIC cjson)
