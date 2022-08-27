# Chat Application

### C++ Chat Console application using C++17 and boost::asio asynchronous library(https://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio.html). The project generates 2 executables (./server and ./client). The server app retains the message history(default: last 100 messages) so that they can be broadcast to every new joined user. Ctrl+C is used to exit the apps.

## Server architecture

- The server app user the Observer design pattern to update all participants.
- The first received message is the name of the new user (stored in a map)
- The Participant class represents the observer in the model and the Room class represents the subject
- The Session class is responsible with connecting the Server with the client (Participant)

## Usage:

- Client app
  - `./client` - starts the executable, then it prompts for the ip, port number and name
  - `./client <ip address> <port number>` -  start the executable with the ip and port as arguments, the programme will prompt user to enter name
  - `./client <ip address> <port number> <name>` - all variables are specified as arguments
- Server app
  - `./server <port number>`