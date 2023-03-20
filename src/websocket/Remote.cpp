#include <websocket/Remote.hpp>

Remote::Remote(std::unique_ptr<IEndpointWrapper> server)
    : server(std::move(server)) {}

bool Remote::send(const std::string &msg) {
    return server->send(msg);
}

void Remote::init_message_callback(std::function<void (const std::string &)> cb) {
    msg_callback = cb;
}

void Remote::init_closed_callback(std::function<void ()> cb) {
    closed_callback = cb;
}

void Remote::message_received(const std::string &msg) {
    if(msg_callback) msg_callback(msg);
}

void Remote::set_closed() {
    if(closed.exchange(true)) return;
    if(closed_callback) closed_callback();
    closed.notify_one();
}

void Remote::close() {
    server->close();
}

Remote::~Remote() {
    if(!closed) {
        close();
    }
    closed.wait(false);
}
