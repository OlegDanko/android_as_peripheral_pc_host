#pragma once

struct IRemoteCallbacksMock {
    virtual void msg(const std::string&) = 0;
    virtual void close() = 0;
    virtual ~IRemoteCallbacksMock() = default;
};
