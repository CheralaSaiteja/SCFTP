cmake_minimum_required(VERSION 3.8)
project(UserMan)

file(GLOB SRC *.cpp *.c *.h)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_link_libraries(${PROJECT_NAME} mysqlcppconn mariadbclient)