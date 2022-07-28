cmake_minimum_required(VERSION 3.22)
project(test_chat)

set(CMAKE_CXX_STANDARD 17)

add_executable(client Client/chat_client.cpp Shared/Message.hpp)
add_executable(server Server/Server.cpp Shared/Message.hpp Server/Session.h Server/Room.h Server/Participant.h Shared/Observer.h)
