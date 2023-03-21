#include <websocket/websocketpp/server/test/WSServerMock.hpp>
#include <websocket/websocketpp/server/WebSocketppServer.hpp>
#include <websocket/test/IRemoteCallbacksMock.hpp>
#include <boost/test/unit_test.hpp>

#include <fakeit.hpp>

using namespace fakeit;

#define DECL_SERVER_MOCK_STRUCT \
    struct ServerMock : public IWSServerMock { \
        using mock_t = Mock<IWSServerMock>; \
        ServerMock() : mock(std::make_shared<mock_t>()) {} \
    public: \
        std::shared_ptr<mock_t> mock; \
        void set_access_channels(uint32_t a) override { mock->get().set_access_channels(a); } \
        void clear_access_channels(uint32_t a) override { \
            mock->get().clear_access_channels(a); \
        } \
        void init_asio() override { mock->get().init_asio(); } \
        void set_open_handler(hdl_callback_t a) override { mock->get().set_open_handler(a); } \
        void set_fail_handler(hdl_callback_t a) override { mock->get().set_fail_handler(a); } \
        void set_message_handler(msg_callback_t a) override { \
            mock->get().set_message_handler(a); \
        } \
        void set_close_handler(hdl_callback_t a) override { mock->get().set_close_handler(a); } \
        void set_reuse_addr(bool a) override { mock->get().set_reuse_addr(a); } \
        void listen(int a) override { mock->get().listen(a); } \
        void start_accept() override { mock->get().start_accept(); } \
        void run() override { mock->get().run(); } \
        void stop_listening() override { mock->get().stop_listening(); } \
        void close(hdl_t a, int b, const std::string& c) override { mock->get().close(a, b, c); } \
        void stop() override { mock->get().stop(); } \
        void send(hdl_t a, const std::string &b, int c, std::error_code &d) override { \
            mock->get().send(a, b, c, d); \
        } \
    };
        // End

auto fake_methods(Mock<IWSServerMock>& mock) {
    Fake(Method(mock, set_access_channels));
    Fake(Method(mock, clear_access_channels));
    Fake(Method(mock, init_asio));
    Fake(Method(mock, set_open_handler));
    Fake(Method(mock, set_fail_handler));
    Fake(Method(mock, set_message_handler));
    Fake(Method(mock, set_close_handler));
    Fake(Method(mock, set_reuse_addr));
    Fake(Method(mock, listen));
    Fake(Method(mock, start_accept));
    Fake(Method(mock, run));
    Fake(Method(mock, stop_listening));
    Fake(Method(mock, stop));
}

BOOST_AUTO_TEST_SUITE(WSPPServerTests)

BOOST_AUTO_TEST_CASE(WSPPServerCreateAndDestroyCase) {
    DECL_SERVER_MOCK_STRUCT;
    ServerMock mock_server;

    auto& mock = *mock_server.mock;
    fake_methods(mock);

    std::atomic_bool run_started{false};
    When(Method(mock, run)).Do([&](){
        run_started = true;
        run_started.notify_one();
    });

    if(true) {
        WebSocketppServer<decltype(mock_server), WSConnectionHandleMock, OpCodeCvtIdentity, 0>
                wspp_server(mock_server, 1234, [&](auto){}, 1, 2);

        Verify(Method(mock, set_access_channels)).Exactly(1);
        Verify(Method(mock, set_access_channels).Using(1)).Exactly(1);
        Verify(Method(mock, clear_access_channels)).Exactly(1);
        Verify(Method(mock, clear_access_channels).Using(2)).Exactly(1);
        Verify(Method(mock, set_open_handler)).Exactly(1);
        Verify(Method(mock, set_fail_handler)).Exactly(1);
        Verify(Method(mock, set_message_handler)).Exactly(1);
        Verify(Method(mock, set_close_handler)).Exactly(1);
        Verify(Method(mock, set_reuse_addr)).Exactly(1);
        Verify(Method(mock, set_reuse_addr).Using(true)).Exactly(1);
        Verify(Method(mock, listen)).Exactly(1);
        Verify(Method(mock, listen).Using(1234)).Exactly(1);
        Verify(Method(mock, start_accept)).Exactly(1);

        run_started.wait(false);
    }
    Verify(Method(mock, stop_listening));
    Verify(Method(mock, stop));
}

BOOST_AUTO_TEST_CASE(WSPPServerMsgIn) {
    Mock<IRemoteCallbacksMock> remote_cb_mock;
    Fake(Method(remote_cb_mock, msg));

    DECL_SERVER_MOCK_STRUCT;
    ServerMock mock_server;

    auto& mock = *mock_server.mock;
    fake_methods(mock);

    std::function<void(WSConnectionHandleMock)> open_handler;
    When(Method(mock, set_open_handler)).Do([&](auto callback){ open_handler = callback; });
    std::function<void(WSConnectionHandleMock, IWSServerMock::message_ptr)> message_handler;
    When(Method(mock, set_message_handler)).Do([&](auto callback){ message_handler = callback; });

    Fake(Method(mock, send));
    Fake(Method(mock, close));

    if(true) {
        std::unique_ptr<IConnectionProvider> remote;
        WebSocketppServer<decltype(mock_server), WSConnectionHandleMock, OpCodeCvtIdentity, 0>
                wspp_server(mock_server, 1234, [&](auto r){ remote = std::move(r); }, 1, 2);

        open_handler(1);
        remote->init_message_callback([&](const std::string& msg){
            remote_cb_mock.get().msg(msg);
        });

        std::string in_captured;
        When(Method(remote_cb_mock, msg)).AlwaysDo([&](const auto& msg) { in_captured = msg; });

        message_handler(WSConnectionHandleMock(1),
                        std::make_shared<IWSServerMock::message>("in"));

        Verify(Method(remote_cb_mock, msg)).Exactly(1);
        BOOST_CHECK(0 == in_captured.compare("in"));
    }
}


BOOST_AUTO_TEST_CASE(WSPPServerMsgInWrongHandle) {
    Mock<IRemoteCallbacksMock> remote_cb_mock;
    Fake(Method(remote_cb_mock, msg));

    DECL_SERVER_MOCK_STRUCT;
    ServerMock mock_server;

    auto& mock = *mock_server.mock;
    fake_methods(mock);

    std::function<void(WSConnectionHandleMock)> open_handler;
    When(Method(mock, set_open_handler)).Do([&](auto callback){ open_handler = callback; });
    std::function<void(WSConnectionHandleMock, IWSServerMock::message_ptr)> message_handler;
    When(Method(mock, set_message_handler)).Do([&](auto callback){ message_handler = callback; });

    Fake(Method(mock, send));
    Fake(Method(mock, close));

    if(true) {
        std::unique_ptr<IConnectionProvider> remote;
        WebSocketppServer<decltype(mock_server), WSConnectionHandleMock, OpCodeCvtIdentity, 0>
                wspp_server(mock_server, 1234, [&](auto r){ remote = std::move(r); }, 1, 2);

        open_handler(1);
        remote->init_message_callback([&](const std::string& msg){
            remote_cb_mock.get().msg(msg);
        });

        Fake(Method(remote_cb_mock, msg));

        message_handler(WSConnectionHandleMock(2),
                        std::make_shared<IWSServerMock::message>("fail"));

        Verify(Method(remote_cb_mock, msg)).Exactly(0);
    }
}

BOOST_AUTO_TEST_CASE(WSPPServerMsgOut) {
    DECL_SERVER_MOCK_STRUCT;
    ServerMock mock_server;

    auto& mock = *mock_server.mock;
    fake_methods(mock);

    std::function<void(WSConnectionHandleMock)> open_handler;
    When(Method(mock, set_open_handler)).Do([&](auto callback){ open_handler = callback; });
    std::function<void(WSConnectionHandleMock, IWSServerMock::message_ptr)> message_handler;
    When(Method(mock, set_message_handler)).Do([&](auto callback){ message_handler = callback; });

    Fake(Method(mock, send));
    Fake(Method(mock, close));

    if(true) {
        std::unique_ptr<IConnectionProvider> remote;
        WebSocketppServer<decltype(mock_server), WSConnectionHandleMock, OpCodeCvtIdentity, 0>
                wspp_server(mock_server, 1234, [&](auto r){ remote = std::move(r); }, 1, 2);

        open_handler(1);

        std::string out_captured;
        When(Method(mock, send)).Do([&](auto, const auto& str, auto, auto){
            out_captured = str;
        });

        BOOST_CHECK(remote->send("out"));
        Verify(Method(mock, send).Matching([&](auto hdl, auto, auto op_code, auto){
            return hdl.id_holder.id == 1 && op_code == OP_CODE_TEXT;
        })).Exactly(1);
        BOOST_CHECK(0 == out_captured.compare("out"));
    }
}


BOOST_AUTO_TEST_CASE(WSPPServerMsgOutFail) {
    DECL_SERVER_MOCK_STRUCT;
    ServerMock mock_server;

    auto& mock = *mock_server.mock;
    fake_methods(mock);

    std::function<void(WSConnectionHandleMock)> open_handler;
    When(Method(mock, set_open_handler)).Do([&](auto callback){ open_handler = callback; });
    std::function<void(WSConnectionHandleMock, IWSServerMock::message_ptr)> message_handler;
    When(Method(mock, set_message_handler)).Do([&](auto callback){ message_handler = callback; });

    Fake(Method(mock, send));
    Fake(Method(mock, close));

    if(true) {
        std::unique_ptr<IConnectionProvider> remote;
        WebSocketppServer<decltype(mock_server), WSConnectionHandleMock, OpCodeCvtIdentity, 0>
                wspp_server(mock_server, 1234, [&](auto r){ remote = std::move(r); }, 1, 2);

        open_handler(1);

        std::string out_captured;
        When(Method(mock, send)).Do([&](auto, auto, auto, auto& ec){
            ec = std::make_error_code(std::errc::timed_out);
        });

        BOOST_CHECK(!remote->send("fail"));
    }
}

BOOST_AUTO_TEST_CASE(WSPPServerClose) {
    Mock<IRemoteCallbacksMock> remote_cb_mock;
    Fake(Method(remote_cb_mock, close));

    DECL_SERVER_MOCK_STRUCT;
    ServerMock mock_server;

    auto& mock = *mock_server.mock;
    fake_methods(mock);

    std::function<void(WSConnectionHandleMock)> open_handler;
    When(Method(mock, set_open_handler)).Do([&](auto callback){ open_handler = callback; });
    std::function<void(WSConnectionHandleMock)> close_handler;
    When(Method(mock, set_close_handler)).Do([&](auto callback){ close_handler = callback; });

    Fake(Method(mock, send));
    Fake(Method(mock, close));


    std::array<std::atomic_bool, 2> closed{false, false};
    When(Method(mock, close)).AlwaysDo([&](auto hdl, auto, auto){
        if(hdl.lock().get() >= 2) return;
        auto& c = closed.at(hdl.lock().get());
        c = true;
        c.notify_one();
    });


    std::vector<std::unique_ptr<IConnectionProvider>> remotes;
    if(true) {
        WebSocketppServer<decltype(mock_server), WSConnectionHandleMock, OpCodeCvtIdentity, 0>
                wspp_server(mock_server, 1234, [&](auto r){
            remotes.push_back(std::move(r));
        }, 1, 2);

        for(int i = 0; i < 6; i++) {
            open_handler(i);
        }
        BOOST_CHECK(remotes.size() == 6);

        for(int i = 0; i < 6; i++) {
            remotes.at(i)->init_closed_callback([&]{
                remote_cb_mock.get().close();
            });
        }

        // =========== Closing through destructor ================
        std::thread t([r0 = std::move(remotes.at(0)), r1 = std::move(remotes.at(1))] mutable {
            r0 = nullptr;
            r1 = nullptr;
        });
        closed.at(0).wait(false);
        close_handler(0);
        closed.at(1).wait(false);
        close_handler(1);

        t.join();

        Verify(Method(remote_cb_mock, close)).Exactly(2);


        // =========== Closing through handler ================
        close_handler(2);
        close_handler(3);

        Verify(Method(remote_cb_mock, close)).Exactly(4);

        // =========== Closing through server destructor ================
    }
    Verify(Method(remote_cb_mock, close)).Exactly(6);
}


BOOST_AUTO_TEST_SUITE_END()
