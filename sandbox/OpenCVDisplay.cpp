#include "OpenCVDisplay.hpp"

void ImageDisplay::thread_fn() {
    cv::namedWindow(name, cv::WINDOW_KEEPRATIO);
    while(true) {
        cv::Mat img;
        {
            std::unique_lock lk(mtx);
            cond.wait(lk, [this]{ return image_set || !running; });
            if(!running) return;
            img = image;
            image_set = false;
        }
        cv::imshow(name, img);
        cv::waitKey(1);
        cv::resizeWindow(name, 1024, 1024);
    }
    cv::destroyWindow(name);
}

ImageDisplay::ImageDisplay(std::string name) : name(name) {}

void ImageDisplay::start() {
    running = true;
    if(t.joinable())
        return;

    t = std::thread([this]{ thread_fn(); });
}

void ImageDisplay::stop() {
    if(!t.joinable())
        return;

    std::lock_guard lk(mtx);
    running = false;
    cond.notify_one();
    t.join();
}

void ImageDisplay::set_image(const cv::Mat &img) {
    std::lock_guard lk(mtx);
    image = img;
    image_set = true;
    cond.notify_one();
}

cv::Mat picture_to_cv_mat(const Picture &pic, bool yuv2rgb) {
    cv::Mat img(pic.height*3/2, pic.width, CV_8UC1);

    size_t index = 0;

    for(auto av : {pic.get_luma(), pic.get_cr(), pic.get_cb()}) {
        for(auto val : av) {
            img.at<uint8_t>(index++) = val;
        }
    }
    if(!yuv2rgb)
        return img;

    cv::Mat img_rgb;
    cv::cvtColor(img, img_rgb, cv::COLOR_YUV2RGB_I420);

    return img_rgb;
}

cv::Mat frame_to_cv_mat(const AVFrame &frame, bool yuv2rgb) {
    cv::Mat img(frame.height*3/2, frame.width, CV_8UC1);

    auto index = 0ul;

    for (int y = 0; y < frame.height; y++) {
        for (int x = 0; x < frame.width; x++) {
            img.at<uint8_t>(index++) = frame.data[0][y * frame.linesize[0] + x];
        }
    }

    /* Cb and Cr */
    for (int y = 0; y < frame.height/2; y++) {
        for (int x = 0; x < frame.width/2; x++) {
            img.at<uint8_t>(index++) = frame.data[1][y * frame.linesize[1] + x];
        }
    }
    for (int y = 0; y < frame.height/2; y++) {
        for (int x = 0; x < frame.width/2; x++) {
            img.at<uint8_t>(index++) = frame.data[2][y * frame.linesize[2] + x];
        }
    }

    if(!yuv2rgb)
        return img;


    cv::Mat img_rgb;
    cv::cvtColor(img, img_rgb, cv::COLOR_YUV2RGB_I420);

    return img_rgb;
}
