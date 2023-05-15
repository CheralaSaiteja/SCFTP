cmake_minimum_required(VERSION 3.8)
project(UserMan)

file(GLOB SRC *.cpp *.c *.h)

add_executable(${PROJECT_NAME} ${SRC})

link_directories(${CMAKE_SOURCE_DIR}/lib)

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_link_libraries(${PROJECT_NAME} mysqlclient)
