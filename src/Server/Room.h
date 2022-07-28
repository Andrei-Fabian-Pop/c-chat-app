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
public:
    void join(Observer *participant) {
        this->addObserver(participant);
        for (auto msg: recent_msgs_)
            participant->update(msg);
    }

    void leave(Observer *participant) {
        this->removeObserver(participant);
    }

    void deliver(const Message &msg) {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();

        this->notify(msg);
    }

private:
    std::set<chat_participant_ptr> participants_;
    chat_message_queue recent_msgs_;
    enum {
        max_recent_msgs = 100
    };
};

#endif //TEST_CHAT_ROOM_H
