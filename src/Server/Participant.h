//
// Created by andrew on 28/07/22.
//

#ifndef TEST_CHAT_PARTICIPANT_H
#define TEST_CHAT_PARTICIPANT_H

#include "../Shared/Message.hpp"
#include "../Shared/Observer.h"
#include <iostream>

class Participant : public Observer {
public:
    ~Participant() override = default;

    void update(const Message &msg) override {
        std::cout << "[Participant Update]\n";
    }
};

#endif //TEST_CHAT_PARTICIPANT_H
