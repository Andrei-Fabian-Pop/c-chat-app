//
// Created by andrew on 28/07/22.
//

#ifndef TEST_CHAT_CLIENT_H
#define TEST_CHAT_CLIENT_H

#include <deque>
#include <boost/asio.hpp>
#include <utility>
#include <array>
#include "../Shared/Message.hpp"

using boost::asio::ip::tcp;
using chat_message_queue = std::deque<Message>;

class Client {
public:
    Client(boost::asio::io_service &io_service, tcp::resolver::iterator endpoint_iterator, std::string name)
            : io_service_(io_service),
              socket_(io_service),
              name_(std::move(name)) {
        do_connect(std::move(endpoint_iterator));
    }

    void write(const Message &msg) {
        this->io_service_.post(
                [this, msg]() {
                    bool write_in_progress = !write_msgs_.empty();
                    write_msgs_.push_back(msg);
                    if (!write_in_progress) {
                        do_write();
                    }
                }
        );
    }

    void close() {
        io_service_.post([this]() { socket_.close(); });
    }

private:
    void do_connect(tcp::resolver::iterator endpoint_iterator) {
        boost::asio::async_connect(
                socket_,
                std::move(endpoint_iterator),
                [this](boost::system::error_code ec, const tcp::resolver::iterator &) {
                    if (!ec) {
                        do_read_header();
                    }
                }
        );
    }

    void do_read_header() {
        boost::asio::async_read(
                socket_,
                boost::asio::buffer(read_msg_.data(), Message::header_length_),
                [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec && read_msg_.decode_header()) {
                        do_read_body();
                    } else {
                        socket_.close();
                    }
                }
        );
    }

    void do_read_body() {
        boost::asio::async_read(
                socket_,
                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec) {
                        std::cout.write(read_msg_.body(), (long) read_msg_.body_length());
                        std::cout << "\n";
                        do_read_header();
                    } else {
                        socket_.close();
                    }
                }
        );
    }

    void do_write() {
        boost::asio::async_write(
                socket_,
                boost::asio::buffer(write_msgs_.front().data(),write_msgs_.front().length()),
                [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec) {
                        write_msgs_.pop_front();
                        if (!write_msgs_.empty()) {
                            do_write();
                        }
                    } else {
                        socket_.close();
                    }
                }
        );
    }

private:
    boost::asio::io_service &io_service_;
    tcp::socket socket_;
    Message read_msg_;
    chat_message_queue write_msgs_;
    std::string name_;
};


//using boost::asio::ip::tcp;
//using chat_message_queue = std::deque<Message>;
//
//class Client {
//public:
//    Client(boost::asio::io_service &io_service,
//           tcp::resolver::iterator endpoint_iterator,
//           std::string name)
//            : io_service_(io_service),
//              socket_(io_service),
//              name_(std::move(name)) {
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
//                        boost::asio::async_write(
//                                this->socket_,
//                                boost::asio::buffer(this->name_.data(), this->name_.size()),
//                                [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
//
//                                    do_read_header();
//                                }
//                        );
//                    }
//                }
//        );
//    }
//
//    void do_read_header() {
//        boost::asio::async_read(
//                socket_,
//                boost::asio::buffer(read_msg_.data(), Message::max_name_length_),
//                [this](boost::system::error_code ec, std::size_t bytes_transferred) {
//                    if (!ec && read_msg_.decode_header()) {
//                        do_read_name();
//                    } else {
//                        socket_.close();
//                    }
//                }
//        );
//    }
//
//    void do_read_name() {
//        boost::asio::async_read(
//                socket_,
//                boost::asio::buffer(read_msg_.name(), read_msg_.max_name_length_),
//                [this](boost::system::error_code ec, std::size_t bytes_transferred) {
//                    if (!ec) {
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
//                    if (bytes_transferred == 0) {
//                        do_read_header();
//                    } else if (!ec) {
//                        std::string name = read_msg_.name();
//                        std::string body = read_msg_.body();
////                        std::cout.write(read_msg_.body(), (long) read_msg_.body_length());
//                        std::cout << "[" << name << "] " << body << '\n';
////                        std::cout << "\n";
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
//                boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
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
//    std::string name_;
//};

#endif //TEST_CHAT_CLIENT_H
