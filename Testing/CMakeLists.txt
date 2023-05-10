cmake_minimum_required(VERSION 3.18)
project(Cipher)

find_package(OpenSSL REQUIRED)

file(GLOB SRC *.c *.h)

include_directories(${OPENSSL_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
