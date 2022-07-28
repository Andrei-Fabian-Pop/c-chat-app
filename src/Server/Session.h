//
// Created by andrew on 28/07/22.
//

#ifndef TEST_CHAT_SESSION_H
#define TEST_CHAT_SESSION_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "../Shared/Message.hpp"
#include "Participant.h"
#include "Room.h"

using boost::asio::ip::tcp;

class Session : public Participant, public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, Room &room) : socket_(std::move(socket)), room_(room) {}

    void start() {
        room_.join(&(*this->shared_from_this()));
        do_read_header();
    }

    void update(const Message &msg) override {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    }

private:
    void do_read_header() {
        boost::asio::async_read(
                socket_,
                boost::asio::buffer(read_msg_.data(), Message::header_length_),
                [this, self = this->shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec && read_msg_.decode_header()) {
                        do_read_body();
                    } else {
                        room_.leave(&(*this->shared_from_this()));
                    }
                }
        );
    }

    void do_read_body() {
        boost::asio::async_read(
                socket_,
                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                [this, self = this->shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec) {
                        room_.deliver(read_msg_);
                        do_read_header();
                    } else {
                        room_.leave(&(*this->shared_from_this()));
                    }
                }
        );
    }

    void do_write() {
        boost::asio::async_write(
                socket_,
                boost::asio::buffer(write_msgs_.front().data(),
                                    write_msgs_.front().length()),
                [this, self = this->shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec) {
                        write_msgs_.pop_front();
                        if (!write_msgs_.empty()) {
                            do_write();
                        }
                    } else {
                        // TODO: add error message to output
                        room_.leave(&(*this->shared_from_this()));
                    }
                }
        );
    }

    tcp::socket socket_;
    Room &room_;
    Message read_msg_;
    chat_message_queue write_msgs_;
};

#endif //TEST_CHAT_SESSION_H
