//
// Created by andrew on 28/07/22.
//

#include <iostream>
#include <thread>

#include <boost/asio.hpp>
#include <utility>

#include "Client.h"

class UI {
private:
    std::string name_;
    std::string ip_;
    std::string port_;
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator_;

public:
    explicit UI() : resolver_(this->io_service_) {
        std::cout << "Welcome!!\n";
        std::cout << "Enter your name:";
        std::cin >> this->name_;

        std::cout << "Enter the ip address of the server:";
        std::cin >> this->ip_;

        std::cout << "Enter the port of the server:";
        std::cin >> this->port_;

        this->endpoint_iterator_ = boost::asio::ip::tcp::resolver::iterator(this->resolver_.resolve(this->ip_, this->port_));
    }

    explicit UI(std::string ip, std::string port) : resolver_(this->io_service_), ip_(std::move(ip)), port_(std::move(port)) {
        std::cout << "Welcome!!\n";
        std::cout << "Enter your name:";
        std::cin >> this->name_;

        this->endpoint_iterator_ = boost::asio::ip::tcp::resolver::iterator(this->resolver_.resolve(this->ip_, this->port_));
    }

    void run() {
        Client c(this->io_service_, this->endpoint_iterator_, this->name_);

        std::thread t([&] { this->io_service_.run(); });

        char line[Message::max_body_length_ + 1];
        while (std::cin.getline(line, Message::max_body_length_ + 1)) {
            Message msg;
            msg.body_length(std::strlen(line));
            std::memcpy(msg.body(), line, msg.body_length());
            msg.encode_header();
            c.write(msg);
        }

//        std::string line {};
//        char l[Message::max_body_length_];
//        while (std::cin.getline(l, Message::max_body_length_ + 1)) {
//            Message msg;
//            line = l;
//            msg.body_length(std::strlen(line.data()));
//            msg.encode_header();
//            c.write(msg);
//        }

        c.close();
        t.join();
    }
};

auto main(int argc, char** argv) -> int {
    try {
        if (argc != 1) {
            UI ui(argv[1], argv[2]);
            ui.run();
        } else {
            UI ui;
            ui.run();
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
//#include <deque>
//#include <iostream>
//#include <thread>
//#include <boost/asio.hpp>
//#include <utility>
//#include "../Shared/Message.hpp"
//
//using boost::asio::ip::tcp;
//using chat_message_queue = std::deque<Message>;
//
//class chat_client {
//public:
//    chat_client(boost::asio::io_service &io_service,
//                tcp::resolver::iterator endpoint_iterator)
//            : io_service_(io_service),
//              socket_(io_service) {
//        do_connect(std::move(endpoint_iterator));
//    }
//
//    void write(const Message &msg) {
//        this->io_service_.post(
//                [this, msg]() {
//                    bool write_in_progress = !write_msgs_.empty();
//                    write_msgs_.push_back(msg);
//                    if (!write_in_progress) {
//                        do_write();
//                    }
//                }
//        );
//    }
//
//    void close() {
//        io_service_.post([this]() { socket_.close(); });
//    }
//
//private:
//    void do_connect(tcp::resolver::iterator endpoint_iterator) {
//        boost::asio::async_connect(
//                socket_,
//                std::move(endpoint_iterator),
//                [this](boost::system::error_code ec, const tcp::resolver::iterator &) {
//                    if (!ec) {
//                        do_read_header();
//                    }
//                }
//        );
//    }
//
//    void do_read_header() {
//        boost::asio::async_read(
//                socket_,
//                boost::asio::buffer(read_msg_.data(), Message::header_length_),
//                [this](boost::system::error_code ec, std::size_t bytes_transferred) {
//                    if (!ec && read_msg_.decode_header()) {
//                        do_read_body();
//                    } else {
//                        socket_.close();
//                    }
//                }
//        );
//    }
//
//    void do_read_body() {
//        boost::asio::async_read(
//                socket_,
//                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
//                [this](boost::system::error_code ec, std::size_t bytes_transferred) {
//                    if (!ec) {
//                        std::cout.write(read_msg_.body(), (long) read_msg_.body_length());
//                        std::cout << "\n";
//                        do_read_header();
//                    } else {
//                        socket_.close();
//                    }
//                }
//        );
//    }
//
//    void do_write() {
//        boost::asio::async_write(
//                socket_,
//                boost::asio::buffer(write_msgs_.front().data(),write_msgs_.front().length()),
//                [this](boost::system::error_code ec, std::size_t bytes_transferred) {
//                    if (!ec) {
//                        write_msgs_.pop_front();
//                        if (!write_msgs_.empty()) {
//                            do_write();
//                        }
//                    } else {
//                        socket_.close();
//                    }
//                }
//        );
//    }
//
//private:
//    boost::asio::io_service &io_service_;
//    tcp::socket socket_;
//    Message read_msg_;
//    chat_message_queue write_msgs_;
//};
//
//auto main(int argc, char **argv) -> int {
//    try {
//        if (argc != 3) {
//            std::cerr << "Usage: chat_client <host> <port>\n";
//            return 1;
//        }
//
//        boost::asio::io_service io_service;
//
//        tcp::resolver resolver(io_service);
//        auto endpoint_iterator = resolver.resolve({argv[1], argv[2]});
//        chat_client c(io_service, endpoint_iterator);
//
//        std::thread t([&io_service]() { io_service.run(); });
//
//        char line[Message::max_body_length_ + 1];
//        while (std::cin.getline(line, Message::max_body_length_ + 1)) {
//            Message msg;
//            msg.body_length(std::strlen(line));
//            std::memcpy(msg.body(), line, msg.body_length());
//            msg.encode_header();
//            c.write(msg);
//        }
//
//        c.close();
//        t.join();
//    }
//    catch (std::exception &e) {
//        std::cerr << "Exception: " << e.what() << "\n";
//    }
//
//    return 0;
//}