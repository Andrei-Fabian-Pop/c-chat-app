//
// Created by andrew on 28/07/22.
//

#ifndef TEST_CHAT_ROOM_H
#define TEST_CHAT_ROOM_H

#include <deque>
#include <utility>
#include <set>
#include <boost/asio.hpp>
#include "../Shared/Message.hpp"
#include "../Shared/Observer.h"
#include "Participant.h"

using chat_participant_ptr = std::shared_ptr<Participant>;
using chat_message_queue = std::deque<Message>;


class Room : public Subject {
public: // TODO: maybe make a default constructor
    void join(Observer *participant) {
        this->addObserver(participant);
        for (auto msg: recent_msgs_)
            participant->update(msg);
    }

    void leave(Observer *participant) {
        this->removeObserver(participant);
    }

    void deliver(const Message &unformatted_msg, const std::shared_ptr<Participant>& participant) {
        Message message;
        strcpy(message.body(), this->name_map_[participant].c_str());
        strcat(message.body(), ": ");
        strcat(message.body(), unformatted_msg.body());
        message.body_length(unformatted_msg.body_length() + this->name_map_[participant].length());
        message.encode_header();

        recent_msgs_.push_back(message);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();

        this->notify(message);
    }

    void set_nickname(const std::shared_ptr<Participant>& participant, const std::string& name) {
        this->name_map_[participant] = name;
    }

private:
    std::set<chat_participant_ptr> participants_;
    chat_message_queue recent_msgs_;
    std::unordered_map<std::shared_ptr<Participant>, std::string> name_map_;
    enum {
        max_recent_msgs = 100
    };
};

#endif //TEST_CHAT_ROOM_H
