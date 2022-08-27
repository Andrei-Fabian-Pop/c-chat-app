//
// Created by andrew on 28/07/22.
//

#include <iostream>
#include <thread>
#include <csignal>

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

    std::unique_ptr<Client> c;

    static void exitHandler([[maybe_unused]] int signal) {
        std::cout << "\nSIGINT called\n";
    }

    static void addSigInt() {
        struct sigaction sigIntHandler{};

        sigIntHandler.sa_handler = exitHandler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;

        sigaction(SIGINT, &sigIntHandler, nullptr);
    }

public:
    explicit UI() : resolver_(this->io_service_) {
        addSigInt();

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
        addSigInt();

        std::cout << "Welcome!!\n";
        std::cout << "Enter your name:";
        std::cin >> this->name_;

        this->endpoint_iterator_ = boost::asio::ip::tcp::resolver::iterator(this->resolver_.resolve(this->ip_, this->port_));
    }

    explicit UI(std::string ip, std::string port, std::string name) : resolver_(this->io_service_), ip_(std::move(ip)), port_(std::move(port)), name_(std::move(name)) {
        addSigInt();

        this->endpoint_iterator_ = boost::asio::ip::tcp::resolver::iterator(this->resolver_.resolve(this->ip_, this->port_));
    }

    ~UI() {
        c->close();
    }

    void run() {
        c = std::make_unique<Client>(this->io_service_, this->endpoint_iterator_, this->name_);

        std::thread t([&] { this->io_service_.run(); });

        char line[Message::max_body_length_ + 1];
        while (std::cin.getline(line, Message::max_body_length_ + 1)) {
            Message msg;
            msg.body_length(std::strlen(line));
            std::memcpy(msg.body(), line, msg.body_length());
            msg.encode_header();
            c->write(msg);
        }

        c->close();
        t.join();
    }
};

auto main(int argc, char** argv) -> int {
    try {
        std::unique_ptr<UI> ui;
        if (argc == 3) {
            ui = std::make_unique<UI>(argv[1], argv[2]);
        } else if (argc == 4) {
            ui = std::make_unique<UI>(argv[1], argv[2], argv[3]);
        } else {
            ui = std::make_unique<UI>();
        }
        ui->run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
