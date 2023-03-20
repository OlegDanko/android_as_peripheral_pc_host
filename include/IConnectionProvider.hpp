#pragma once

#include <string>
#include <functional>

class IConnectionProvider {
public:
    virtual bool send(const std::string&) = 0;
    virtual void init_message_callback(std::function<void(const std::string&)>) = 0;
    virtual void init_closed_callback(std::function<void()>) = 0;
    virtual ~IConnectionProvider() = default;
};
