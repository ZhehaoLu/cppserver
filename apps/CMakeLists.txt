# apps/CMakeLists.txt

add_executable(client_01 app_01/client.cpp)
add_executable(server_01 app_01/server.cpp)

add_executable(client_02 app_02/client.cpp app_02/util.cpp)
add_executable(server_02 app_02/server.cpp app_02/util.cpp)

add_executable(client_03 app_03/client.cpp app_03/util.cpp)
add_executable(server_03 app_03/server.cpp app_03/util.cpp)

add_executable(client_04 app_04/client.cpp app_04/util.cpp app_04/Epoll.cpp app_04/InetAddress.cpp app_04/Socket.cpp)
add_executable(server_04 app_04/server.cpp app_04/util.cpp app_04/Epoll.cpp app_04/InetAddress.cpp app_04/Socket.cpp)

add_executable(client_05 app_05/client.cpp app_05/util.cpp app_05/Epoll.cpp app_05/InetAddress.cpp app_05/Socket.cpp app_05/Channel.cpp)
add_executable(server_05 app_05/server.cpp app_05/util.cpp app_05/Epoll.cpp app_05/InetAddress.cpp app_05/Socket.cpp app_05/Channel.cpp)

add_library(app06_lib
        app_06/src/Channel.cpp
        app_06/src/Epoll.cpp
        app_06/src/EventLoop.cpp
        app_06/src/InetAddress.cpp
        app_06/src/Server.cpp
        app_06/src/Socket.cpp
        app_06/src/util.cpp
)

# So headers like #include "Epoll.h" work
target_include_directories(app06_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(client_06 app_06/client.cpp)
target_link_libraries(client_06 PRIVATE app06_lib)

add_executable(server_06 app_06/server.cpp)
target_link_libraries(server_06 PRIVATE app06_lib)
