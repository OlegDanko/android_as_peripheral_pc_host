#pragma once
#include <string>

class IEndpointWrapper {
public:
    virtual bool send(const std::string&) = 0;
    virtual void close() = 0;
    virtual ~IEndpointWrapper() = default;
};
