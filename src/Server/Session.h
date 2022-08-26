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
    Session(tcp::socket socket, Room &room) : socket_(std::move(socket)), room_(room) {
        this->read_nickname_ = false;
    }

    void start() {
        room_.join(&(*this->shared_from_this()));
        doReadHeader();
    }

    void update(const Message &msg) override {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            doWrite();
        }
    }

private:
    void doReadHeader() {
        boost::asio::async_read(
                socket_,
                boost::asio::buffer(read_msg_.data(), Message::header_length_),
                [this, self = this->shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec && read_msg_.decode_header()) {
                        doReadBody();
                    } else {
                        std::cout << "Leaving room\n";
                        room_.leave(&(*self));
                    }
                }
        );
    }

    void doReadBody() {
        boost::asio::async_read(
                socket_,
                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                [this, self = this->shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec) {
                        if (bytes_transferred != 0) {
                            if (!this->read_nickname_) {
                                this->nickname_ = this->read_msg_.body();
                                this->room_.set_nickname(self, this->nickname_);
                                this->read_nickname_ = true;
                                std::cout << this->nickname_ << " connected\n";
                            } else {
                                room_.deliver(read_msg_, self);
                            }
                        }
                        doReadHeader();
                    } else {
                        std::cout << "Leaving room\n";
                        room_.leave(&(*self));
                    }
                }
        );
    }

    void doWrite() {
        boost::asio::async_write(
                socket_,
                boost::asio::buffer(write_msgs_.front().data(),
                                    write_msgs_.front().length()),
                [this, self = this->shared_from_this()](boost::system::error_code ec, std::size_t bytes_transferred) {
                    if (!ec) {
                        write_msgs_.pop_front();
                        if (!write_msgs_.empty()) {
                            doWrite();
                        }
                    } else {
                        std::cout << "Leaving room\n";
                        room_.leave(&(*self));
                    }
                }
        );
    }

    tcp::socket socket_;
    Room &room_;
    Message read_msg_;
    std::string nickname_;
    chatMessageQueue write_msgs_;
    bool read_nickname_;
};

#endif //TEST_CHAT_SESSION_H
