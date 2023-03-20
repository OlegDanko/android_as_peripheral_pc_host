#pragma once
#include "../IConnectionProvider.hpp"
//#include "../utils/AtomicWrapper.hpp"
#include "IEndpointWrapper.hpp"

#include <system_error>
#include <memory>
#include <atomic>
#include <iostream>

class Remote : public IConnectionProvider {
    std::function<void(const std::string&)> msg_callback;
    std::function<void()> closed_callback;
public:
    std::unique_ptr<IEndpointWrapper> server;

    std::atomic_bool closed{0};
    Remote(std::unique_ptr<IEndpointWrapper> server);

    bool send(const std::string& msg) override;

    void init_message_callback(std::function<void(const std::string&)> cb) override;

    void init_closed_callback(std::function<void()> cb) override;

    void message_received(const std::string& msg);

    void set_closed();

    void close();

    ~Remote();
};
