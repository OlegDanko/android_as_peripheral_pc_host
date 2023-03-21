#include <boost/test/unit_test.hpp>

#include <websocket/Remote.hpp>
#include <websocket/test/IRemoteCallbacksMock.hpp>
#include <thread>
#include <atomic>
#include <fakeit.hpp>

using namespace fakeit;

BOOST_AUTO_TEST_SUITE(RemoteTests)

BOOST_AUTO_TEST_CASE(RemoteClosedNoCallback) {
    Remote r(nullptr);
    BOOST_CHECK_NO_THROW(r.set_closed());
}

BOOST_AUTO_TEST_CASE(RemoteClosedWithCallback) {
    fakeit::Mock<IRemoteCallbacksMock> cb_mock;
    Fake(Method(cb_mock, close));

    Remote r(std::move(nullptr));
    r.init_closed_callback([&cb_mock]{cb_mock.get().close();});
    // first time - callback called
    BOOST_CHECK_NO_THROW(r.set_closed());
    Verify(Method(cb_mock,close)).Exactly(1);
    // second time - no second invocation
    BOOST_CHECK_NO_THROW(r.set_closed());
    Verify(Method(cb_mock,close)).Exactly(1);
}

BOOST_AUTO_TEST_CASE(RemoteMsgNoCallback) {
    Remote r(std::move(nullptr));
    std::string str{"test"};
    BOOST_CHECK_NO_THROW(r.message_received(str));
    // set_close must be used before object can be destroyed
    r.set_closed();
}

BOOST_AUTO_TEST_CASE(RemoteMsgWithCallback) {
    fakeit::Mock<IRemoteCallbacksMock> cb_mock;
    Fake(Method(cb_mock, msg));

    Remote r(std::move(nullptr));
    std::string str{"test"};
    r.init_message_callback([&str, &cb_mock](const auto& msg){ cb_mock.get().msg(msg); });
    BOOST_CHECK_NO_THROW(r.message_received(str));
    Verify(Method(cb_mock,msg).Using(str)).Exactly(1);
    // set_close must be used before object can be destroyed
    r.set_closed();
}

BOOST_AUTO_TEST_CASE(RemoteSend) {
    fakeit::Mock<IEndpointWrapper> ep_w_mock;
    Fake(Dtor(ep_w_mock));
    std::unique_ptr<IEndpointWrapper> ep_w_u_ptr(&ep_w_mock.get());

    Remote r(std::move(ep_w_u_ptr));
    std::string str1{"test_1"};
    When(Method(ep_w_mock,send)).Return(true);
    BOOST_CHECK(r.send(str1));
    std::string str2{"test_2"};
    When(Method(ep_w_mock,send)).Return(false);
    BOOST_CHECK(!r.send(str2));
    Verify(Method(ep_w_mock,send).Using(str1)).Exactly(1);
    Verify(Method(ep_w_mock,send).Using(str2)).Exactly(1);
    // set_close must be used before object can be destroyed
    r.set_closed();
}

BOOST_AUTO_TEST_CASE(RemoteClose) {
    fakeit::Mock<IEndpointWrapper> ep_w_mock;
    Fake(Dtor(ep_w_mock));
    Fake(Method(ep_w_mock,close));
    std::unique_ptr<IEndpointWrapper> ep_w_u_ptr(&ep_w_mock.get());

    Remote r(std::move(ep_w_u_ptr));
    r.close();
    Verify(Method(ep_w_mock,close)).Exactly(1);
    // set_close must be used before object can be destroyed
    r.set_closed();
}

BOOST_AUTO_TEST_CASE(RemoteClosedByDestructor) {
    fakeit::Mock<IEndpointWrapper> ep_w_mock;
    Fake(Dtor(ep_w_mock));

    std::atomic_bool closed_called = false;
    When(Method(ep_w_mock,close)).Do([&closed_called]{
        closed_called = true;
        closed_called.notify_one();
    });

    std::unique_ptr<IEndpointWrapper> ep_w_u_ptr(&ep_w_mock.get());

    Remote* r_ptr;
    std::thread t([&ep_w_u_ptr, &r_ptr]{
        Remote r(std::move(ep_w_u_ptr));
        r_ptr = &r;
    });

    closed_called.wait(false);
    r_ptr->set_closed();
    t.join();
}

BOOST_AUTO_TEST_SUITE_END()
