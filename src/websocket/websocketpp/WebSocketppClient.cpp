#include <websocket/websocketpp/WebSocketppClient.hpp>

WebsocketppClient::WebsocketppClient(const std::string &uri, message_callback_t msg_callback) {
    client.set_access_channels(websocketpp::log::alevel::none);
    client.clear_access_channels(websocketpp::log::alevel::none);

    client.init_asio();

    client.set_open_handler([this](auto hdl){
        handle = hdl;
        state.val = OPENED;
        state.notify_one();
    });
    client.set_fail_handler([this](auto hdl){
        state.val = FAILED;
        state.notify_one();
    });
    client.set_message_handler([msg_callback](auto hdl, auto msg){
        msg_callback(msg->get_payload());
    });
    client.set_close_handler([this](auto){
        state.val = CLOSED;
    });

    websocketpp::lib::error_code ec;
    auto con = client.get_connection(uri, ec);

    if (ec) {
        std::cout << "connection creation failed: "
                  << ec.message()
                  << std::endl;
        state.val = FAILED;
        return;
    }

    client.connect(con);

    t = std::thread([this]{
        client.run();
    });
    state.wait(CONNECTING);

    switch(state.val) {
    case OPENED:
        std::cout << "Opened connection successfully" << std::endl;
        break;
    case FAILED:
        std::cout << "Failed to open connection" << std::endl;
        break;
    case CLOSED:
        std::cout << "Connection closed already?" << std::endl;
        break;
    }
}

WebsocketppClient::~WebsocketppClient() {
    if(state.val == OPENED) {
        client.close(handle, 1000, "Closed and that's that");
    }
    if(t.joinable()) {
        t.join();
    }
}

bool WebsocketppClient::send(const std::string &msg) {
    if(state.val != OPENED)
        return false;
    client.send(handle, msg, websocketpp::frame::opcode::TEXT);
    return true;
}
