#pragma once
#include <memory>
#include <functional>

class IWebSocketClient {
public:
    virtual bool send(const std::string&) = 0;
    virtual ~IWebSocketClient() = default;

};

using message_callback_t = std::function<void(const std::string&)>;

std::unique_ptr<IWebSocketClient> gen_websocket_client(const std::string& uri,
                                                       message_callback_t callback);
