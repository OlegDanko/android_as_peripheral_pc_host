#include <iostream>

#include <rtc/datachannel.hpp>
#include <rtc/rtc.hpp>

#include "OpenCVDisplay.hpp"
#include <opencv2/imgproc.hpp>

int main_display(int argc, char** argv) {
    Codec cdc(24, 24);
    auto pic = cdc.make_picture();

    pic.i_pts() = 0;

    ImageDisplay display;
    display.start();

    std::queue<cv::Mat> images;

    Decoder decoder([&](const auto& frame){
        std::cout << images.size() << std::endl;
        display.set_image(images.front());
        images.pop();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        display.set_image(frame_to_cv_mat(frame));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    });

    auto iterations = 10000000;
    do {
        gen_frame(pic);
        images.push(picture_to_cv_mat(pic));
        auto nal = cdc.encode(pic);
        pic.i_pts()++;
        decoder.feed(nal.payload());
    } while (iterations--);

    display.stop();

    return 0;
}

int main() {
    std::cout << "sender" << std::endl;

    std::shared_ptr<rtc::WebSocket> client;

    rtc::WebSocketServer::Configuration config;
    config.port = 33333;
    rtc::WebSocketServer server(config);

    std::promise<void> promiseConn, promiseClose;
    auto futureConn = promiseConn.get_future();
    auto futureClose = promiseClose.get_future();

    server.onClient([&](std::shared_ptr<rtc::WebSocket> c) {
        client = c;
        std::cout << client->remoteAddress().value_or("no address") << std::endl;
        client->onOpen([&]{ promiseConn.set_value(); });
        client->onClosed([&]{ promiseClose.set_value(); });
        client->onError([&](const std::string&) {
            promiseClose.set_value();
            promiseConn.set_value();
        });

        client->onMessage([&](rtc::message_variant msg){
            if(std::holds_alternative<std::string>(msg)) {
                std::cout << std::get<std::string>(msg) << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                client->send({std::string{"ping"}});
            }
        });
    });

    futureConn.get();

    Codec cdc(24, 24);
    auto pic = cdc.make_picture();
    pic.i_pts() = 0;

    while(client->isOpen()) {
        gen_frame(pic);
        auto nal = cdc.encode(pic);
        pic.i_pts()++;

        std::vector<std::byte> packet;
        for(auto b : nal.payload())
            packet.push_back(std::byte(b));

        client->send({packet});
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    futureClose.get();


    return 0;
}
