#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

#include "WSPPServerDefs.hpp"
#include "../../Remote.hpp"

template<typename server_t,
         typename conn_hdl_t,
         typename OP_CODE_CVT,
         int CLOSE_TIMEOUT_MS = 5000>
class WebSocketppServer : public Class {
public:
    using remote_created_callback = std::function<void(std::unique_ptr<IConnectionProvider>)>;

    std::unordered_map<decltype(conn_hdl_t().lock().get()), Remote*> remotes_map;
    std::mutex mtx;
    std::condition_variable cv_closed;
    server_t server;
    std::thread t;

    std::unique_ptr<IEndpointWrapper> make_server_wrapper(conn_hdl_t hdl) {
        struct ServerWrapper : public IEndpointWrapper {
            server_t& server;
            conn_hdl_t hdl;
            ServerWrapper(server_t& s, conn_hdl_t h) : server(s), hdl(h) {}
            bool send(const std::string& msg) override {
                std::error_code ec;
                server.send(hdl, msg, OP_CODE_CVT::to_impl(OP_CODE_TEXT), ec);
                return ec ? false : true;
            }
            void close() override {
                server.close(hdl, 1000, "Closed, sorry");
            }
        };
        return std::make_unique<ServerWrapper>(server, hdl);
    }

    WebSocketppServer(server_t s,
                      int port,
                      remote_created_callback on_remote_created,
                      int acces_channels = 0,
                      int clear_acces_channels = 0) : server(std::move(s)) {
        server.set_access_channels(acces_channels);
        server.clear_access_channels(clear_acces_channels);

        server.init_asio();

        server.set_open_handler([this, on_remote_created](auto hdl) {
            std::cout << "Opened connection" << std::endl;
            std::lock_guard lk(mtx);
            auto remote = std::make_unique<Remote>(make_server_wrapper(hdl));
            auto remote_ptr = remote.get();
            if(on_remote_created)
                on_remote_created(std::move(remote));
            remotes_map[hdl.lock().get()] = remote_ptr;
        });
        server.set_fail_handler([](auto hdl) {
            std::cout << "Failed to open connection" << std::endl;
        });
        server.set_message_handler([this](auto hdl, const auto& msg) {
            std::lock_guard lk(mtx);
            if(auto it = remotes_map.find(hdl.lock().get()); remotes_map.end() != it) {
                it->second->message_received(msg->get_payload());
            }
        });
        server.set_close_handler([this](auto hdl) {
            std::cout << "Connection closed" << std::endl;
            std::lock_guard lk(mtx);
            if(auto it = remotes_map.find(hdl.lock().get()); remotes_map.end() != it) {
                it->second->set_closed();
                remotes_map.erase(it);
                cv_closed.notify_one();
                std::cout << "Notifying cv_closed with map size "
                          << remotes_map.size() << std::endl;
            }
        });

        server.set_reuse_addr(true);
        server.listen(port);
        server.start_accept();

        t = std::thread([this]{
            server.run();
        });
    }
    ~WebSocketppServer() {
        server.stop_listening();
        std::unique_lock lk(mtx);
        for(auto it : remotes_map) {
            it.second->close();
        }
        auto all_closed = cv_closed.wait_for(lk, std::chrono::milliseconds(CLOSE_TIMEOUT_MS),
                                             [this](){ return remotes_map.empty(); });

        server.stop();
        t.join();

        if(!all_closed) {
            for(auto it : remotes_map) {
                it.second->set_closed();
            }
        }
    }
};
