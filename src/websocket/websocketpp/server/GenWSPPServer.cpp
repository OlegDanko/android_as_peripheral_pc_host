#include <websocket/websocketpp/server/GenWSPPServer.hpp>
#include <websocket/websocketpp/server/WebSocketppServer.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/logger/levels.hpp>

typedef websocketpp::server<websocketpp::config::asio> server_t;
typedef websocketpp::connection_hdl conn_hdl_t;


struct OpCodeCvt {
    static websocketpp::frame::opcode::value to_impl(EOpCode) {
        return websocketpp::frame::opcode::TEXT;
    }
};

std::unique_ptr<Class> gen_websocketpp_server(int port, remote_created_callback callback) {
    return std::make_unique<WebSocketppServer<server_t, conn_hdl_t, OpCodeCvt>>(
        port,
        callback,
        0,//websocketpp::log::alevel::all,
        0//websocketpp::log::alevel::frame_payload
    );
}
