cmake_minimum_required(VERSION 3.18)
project(SCFTPClient)

file(GLOB SRC *.c *.h)

add_executable(${PROJECT_NAME} ${SRC})
