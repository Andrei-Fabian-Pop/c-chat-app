//
// Created by andrew on 28/07/22.
//

#ifndef TEST_CHAT_OBSERVER_H
#define TEST_CHAT_OBSERVER_H

#include <vector>
#include <set>
#include <algorithm>
#include "Message.hpp"

class Observer {
public:
    virtual void update(const Message &msg) = 0;
    virtual ~Observer() = default;
};

class Subject {
protected:
    std::vector<Observer *> observer_;
public:
    void addObserver(Observer *o) { this->observer_.push_back(o); }

    void removeObserver(Observer *o) {
        this->observer_.erase(std::find(this->observer_.begin(), this->observer_.end(), o));
    }

    void notify(const Message &msg) {
        for (auto obs: observer_) {
            obs->update(msg);
        }
    }
};


#endif //TEST_CHAT_OBSERVER_H
