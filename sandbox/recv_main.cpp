#include <iostream>

#include <rtc/datachannel.hpp>
#include <rtc/rtc.hpp>

#include "OpenCVDisplay.hpp"

int main() {
    ImageDisplay display;
    display.start();

    Decoder decoder([&](const auto& frame){
        display.set_image(frame_to_cv_mat(frame));
    });

	std::cout << "receiver" << std::endl;

    rtc::WebSocket socket;
    socket.open("ws://localhost:33333/");

    std::promise<void> promiseConn, promiseClose;
    auto futureConn = promiseConn.get_future();
    auto futureClose = promiseClose.get_future();

    socket.onOpen([&]{ promiseConn.set_value(); });
    socket.onClosed([&]{ promiseClose.set_value(); });
    socket.onError([&](const std::string&) {
        promiseClose.set_value();
        promiseConn.set_value();
    });

    socket.onMessage([&socket, &decoder](rtc::message_variant msg){
        if(std::holds_alternative<std::string>(msg)) {
            std::cout << std::get<std::string>(msg) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            socket.send({std::string{"pong"}});
        } else {
            auto packet = std::get<std::vector<std::byte>>(msg);
            decoder.feed(arr_view{(uint8_t*)&packet.at(0), packet.size()});
        }
    });

    futureConn.get();
    futureClose.get();

    display.stop();

	return 0;
}
