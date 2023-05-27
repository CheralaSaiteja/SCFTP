cmake_minimum_required(VERSION 3.8)
project(SCFTP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

set(SERVER_INC Server/include/)
set(CLIENT_INC Client/include/)
set(CONFIGTOOL_INC ConfigTool/include/)
set(USERMAN_INC UserMan/include/)

add_subdirectory(deps/cJSON)

# server src
file(GLOB SERVER_SRC 
    Server/src/*.c 
    Server/src/*.cpp )

# client src
file(GLOB CLIENT_SRC 
    Client/src/*.c 
    Client/src/*.cpp)

# ConfigTool src
file(GLOB CONFIGTOOL_SRC 
    ConfigTool/src/*.c 
    ConfigTool/src/*.cpp) 

# userman src
file(GLOB USERMAN_SRC 
    UserMan/src/*.c 
    UserMan/src/*.cpp) 

# executables
add_executable(Server ${SERVER_SRC})
add_executable(Client ${CLIENT_SRC})
add_executable(ConfigTool ${CONFIGTOOL_SRC})
add_executable(UserMan ${USERMAN_SRC})

# globl directory link
link_directories(${CMAKE_SOURCE_DIR}/lib)

# include directories
target_include_directories(Server PUBLIC ${SERVER_INC})
target_include_directories(Client PUBLIC ${CLIENT_INC})
target_include_directories(ConfigTool PUBLIC ${CONFIGTOOL_INC} deps/cJSON/)
target_include_directories(UserMan PUBLIC ${USERMAN_INC})

# link libraries
# target_link_libraries(Server PUBLIC ${SERVER_INC})
# target_link_libraries(Client PUBLIC ${CLIENT_INC})
target_link_libraries(ConfigTool PUBLIC cjson) 
target_link_libraries(UserMan PUBLIC mysqlclient)
