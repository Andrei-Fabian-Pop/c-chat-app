cmake_minimum_required(VERSION 3.22)
project(c-chat-app)

set(CMAKE_CXX_STANDARD 17)

add_executable(client src/Client/Client_UI.cpp src/Shared/Message.hpp src/Client/Client.h)
add_executable(server src/Server/Server.cpp src/Shared/Message.hpp src/Server/Session.h src/Server/Room.h src/Server/Participant.h src/Shared/Observer.h)
