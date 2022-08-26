//
// Created by andrew on 28/07/22.
//

#ifndef C_CHAT_APP_CLIENT_H
#define C_CHAT_APP_CLIENT_H

#include <deque>
#include <boost/asio.hpp>
#include <utility>
#include <array>
#include "../Shared/Message.hpp"

using boost::asio::ip::tcp;
using chatMessageQueue = std::deque<Message>;

class Client {
public:
    Client(boost::asio::io_service &ioService, tcp::resolver::iterator endpointIterator, std::string name)
            : io_service_(ioService),
              socket_(ioService),
              name_(std::move(name)) {
        doConnect(std::move(endpointIterator));
    }

    void write(const Message &msg) {
        this->io_service_.post(
                [this, msg]() {
                    bool writeInProgress = !write_msgs_.empty();
                    write_msgs_.push_back(msg);
                    if (!writeInProgress) {
                        doWrite();
                    }
                }
        );
    }

    void close() {
        io_service_.post([this]() { socket_.close(); });
    }


private:
    void doConnect(tcp::resolver::iterator endpointIterator) {
        boost::asio::async_connect(
                socket_,
                std::move(endpointIterator),
                [this](boost::system::error_code ec, const tcp::resolver::iterator &) {
                    if (!ec) {
                        Message name;
                        name.body_length(this->name_.length());
                        strcpy(name.body(), this->name_.c_str());
                        name.encode_header();
                        this->write(name);
                    }
                }
        );
        doReadHeader();
    }

    void doReadHeader() {
        boost::asio::async_read(
                socket_,
                boost::asio::buffer(read_msg_.data(), Message::header_length_),
                [this](boost::system::error_code ec, std::size_t bytesTransferred) {
                    if (!ec && read_msg_.decode_header()) {
                        doReadBody();
                    } else {
                        std::cout << "Leaving room\n";
                        socket_.close();
                    }
                }
        );
    }

    void doReadBody() {
        boost::asio::async_read(
                socket_,
                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                [this](boost::system::error_code ec, std::size_t bytesTransferred) {
                    if (!ec) {
                        std::cout.write(read_msg_.body(), (long) read_msg_.body_length());
                        std::cout << "\n";
                        doReadHeader();
                    } else {
                        std::cout << "Leaving room\n";
                        socket_.close();
                    }
                }
        );
    }

    void doWrite() {
        boost::asio::async_write(
                socket_,
                boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
                [this](boost::system::error_code ec, std::size_t bytesTransferred) {
                    if (!ec) {
                        write_msgs_.pop_front();
                        if (!write_msgs_.empty()) {
                            doWrite();
                        }
                    } else {
                        std::cout << "Leaving room\n";
                        socket_.close();
                    }
                }
        );
    }

private:
    boost::asio::io_service &io_service_;
    tcp::socket socket_;
    Message read_msg_;
    chatMessageQueue write_msgs_;
    std::string name_;
};

#endif //C_CHAT_APP_CLIENT_H
