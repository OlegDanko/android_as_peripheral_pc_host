#pragma once

#include "Codec.hpp"

#include <opencv2/opencv.hpp>
#include <thread>
#include <condition_variable>

extern "C" {
#include <libavcodec/avcodec.h>
}

class ImageDisplay {
    std::string name;
    cv::Mat image;
    std::mutex mtx;
    bool image_set{false};
    bool running{false};
    std::condition_variable cond;

    std::thread t;

    void thread_fn();
public:
    ImageDisplay(std::string name = "image");

    void start();

    void stop();

    void set_image(const cv::Mat& img);
};

cv::Mat picture_to_cv_mat(const Picture& pic, bool yuv2rgb = true);

cv::Mat frame_to_cv_mat(const AVFrame& frame, bool yuv2rgb = true);




