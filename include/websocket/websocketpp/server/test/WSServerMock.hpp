#pragma once
#include <string>
#include <memory>
#include <exception>
#include <functional>

#include "../WSPPServerDefs.hpp"

struct WSConnectionHandleMock {
    struct {
        size_t id;
        size_t get() { return id; }
    } id_holder;
    auto lock() { return id_holder; }
    WSConnectionHandleMock(size_t id = 0) : id_holder{id} {}
};

struct OpCodeCvtIdentity {
    static auto to_impl(EOpCode code) { return code; }
};

struct IWSServerMock {
private:
    static void unimplemented() {
        throw std::logic_error("Function is not implemented in the mock, "
                               "should be overriden in a child class");
    }
public:

    struct message {
        std::string msg;
        const std::string& get_payload() { return msg; }
    };
    using message_ptr = std::shared_ptr<message>;

    using hdl_t = WSConnectionHandleMock;
    using hdl_callback_t = std::function<void(hdl_t)>;
    using msg_callback_t = std::function<void(hdl_t, message_ptr)>;

    virtual void set_access_channels(uint32_t lvl) = 0;
    virtual void clear_access_channels(uint32_t lvl) = 0;
    virtual void init_asio() = 0;

    virtual void set_open_handler(hdl_callback_t) = 0;
    virtual void set_fail_handler(hdl_callback_t) = 0;
    virtual void set_message_handler(msg_callback_t) = 0;
    virtual void set_close_handler(hdl_callback_t) = 0;

    virtual void set_reuse_addr(bool) = 0;
    virtual void listen(int) = 0;
    virtual void start_accept() = 0;
    virtual void run() = 0;

    virtual void stop_listening() = 0;
    virtual void close(hdl_t, int, const std::string&) = 0;
    virtual void stop() = 0;

    virtual void send(hdl_t,
                      const std::string&,
                      int,
                      std::error_code&) = 0;
    virtual ~IWSServerMock() = default;
};
