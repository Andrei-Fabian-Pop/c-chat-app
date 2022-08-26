#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "../Shared/Message.hpp"
#include "Session.h"
#include "Room.h"

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_service &io_service, const tcp::endpoint &endpoint) :
            io_service_(io_service),
            acceptor_(io_service, endpoint),
            socket_(io_service) {
        std::cout << get_ip() << " " << endpoint.port() << '\n';

        doAccept();
    }

private:
    auto get_ip() -> std::string {
        // returns the server ip, according to google
        boost::asio::ip::udp::resolver resolver(this->io_service_);
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), "google.com", "");
        boost::asio::ip::udp::resolver::iterator endpoints = resolver.resolve(query);
        boost::asio::ip::udp::endpoint ep = *endpoints;
        boost::asio::ip::udp::socket socket(this->io_service_);
        socket.connect(ep);
        boost::asio::ip::address addr = socket.local_endpoint().address();

        return addr.to_string();
    }

    void doAccept() {
        acceptor_.async_accept(
                socket_,
                [this](boost::system::error_code ec) {
                    if (!ec) {
                        std::make_shared<Session>(std::move(socket_), room_)->start();
                    }

                    doAccept();
                }
        );
    }

    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    Room room_;
};

//----------------------------------------------------------------------

auto main(int argc, char **argv) -> int {
    try {
        if (argc < 2) {
            std::cerr << "Usage: Server <port> [<port> ...]\n";
            return 1;
        }

        boost::asio::io_service io_service;

        std::list<Server> servers;
        for (int i = 1; i < argc; ++i) {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            servers.emplace_back(io_service, endpoint);
        }

        io_service.run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}