#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

class Message {
public:
    enum {
        header_length_ = 4, // bytes
        max_body_length_ = 512 // bytes
    };

    Message() : body_length_(0), data_() {}

    [[maybe_unused]] [[nodiscard]] const char *data() const {
        return data_;
    }

    char *data() {
        return data_;
    }

    [[nodiscard]] std::size_t length() const {
        return header_length_ + body_length_;
    }

    [[maybe_unused]] [[nodiscard]] const char *body() const {
        return data_ + header_length_;
    }

    char *body() {
        return data_ + header_length_;
    }

    [[nodiscard]] std::size_t body_length() const {
        return body_length_;
    }

    void body_length(std::size_t new_length) {
        body_length_ = new_length;
        if (body_length_ > max_body_length_)
            body_length_ = max_body_length_;
    }

    bool decode_header() {
        char header[header_length_ + 1] = "";
        std::strncat(header, data_, header_length_);
        body_length_ = std::atoi(header);
        if (body_length_ > max_body_length_) {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode_header() {
        char header[header_length_ + 1] = "";
        std::sprintf(header, "%4d", (int)(body_length_));
        std::memcpy(data_, header, header_length_);
    }

private:
    char data_[header_length_ + max_body_length_]{};
    std::size_t body_length_;
};

#endif // CHAT_MESSAGE_HPP