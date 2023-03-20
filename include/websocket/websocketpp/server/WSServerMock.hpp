#pragma once
#include <string>
#include <memory>
#include <functional>


struct WSConnectionHandleMock {
    struct {
        size_t id;
        size_t get() { return id; }
    } id_holder;
    auto lock() { return id_holder; }
    WSConnectionHandleMock(size_t id) : id_holder{id} {}
};

struct WSServerMock {
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

    void set_access_channels(const uint32_t lvl) { unimplemented(); }
    void clear_access_channels(const uint32_t lvl) { unimplemented(); }
    virtual void init_asio() { unimplemented(); }

    virtual void set_open_handler(hdl_callback_t) { unimplemented(); }
    virtual void set_fail_handler(hdl_callback_t) { unimplemented(); }
    virtual void set_message_handler(msg_callback_t) { unimplemented(); }
    virtual void set_close_handler(hdl_callback_t) { unimplemented(); }

    virtual void set_reuse_addr(bool) { unimplemented(); }
    virtual void listen(int) { unimplemented(); }
    virtual void start_accept() { unimplemented(); }
    virtual void run() { unimplemented(); }

    virtual void stop_listening() { unimplemented(); }
    virtual void close(hdl_t, int, std::string) { unimplemented(); }
    virtual void stop() { unimplemented(); }

    virtual void send(hdl_t,
                      const std::string&,
                      int,
                      std::error_code&) { unimplemented(); }
};
