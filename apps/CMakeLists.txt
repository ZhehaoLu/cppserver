# apps/CMakeLists.txt

add_executable(client_01 app_01/client.cpp)
add_executable(server_01 app_01/server.cpp)

add_executable(client_02 app_02/client.cpp app_02/util.cpp)
add_executable(server_02 app_02/server.cpp app_02/util.cpp)

add_executable(client_03 app_03/client.cpp app_03/util.cpp)
add_executable(server_03 app_03/server.cpp app_03/util.cpp)
