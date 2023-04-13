#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <functional>
#include <x264.h>

extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
}

template<typename T>
class arr_view
{
    T* _data;
    size_t _size;
public:
    arr_view(T* data, size_t size) : _data(data), _size(size) {}
    T* data() { return _data; }
    T* data() const { return _data; }
    size_t size() const { return _size; }

    arr_view sub_view(size_t offset) {
        return sub_view(offset, _size);
    }

    arr_view sub_view(size_t offset, size_t size) {
        if(offset >= _size)
            return {nullptr, 0};

        auto rem = _size - offset;
        return {&at(offset), std::min(rem, size)};
    }

    T& at(size_t n) const {
        if(n < _size)
            return _data[n];
        throw std::out_of_range("arr_view out of bounds");
    }

    class it {
        arr_view& arr;
        size_t position;
    public:
        it(arr_view& a, size_t p) : arr(a), position(p) {}
        T& operator*() const { return arr.at(position); }
        auto& operator++() { ++position; return *this; }
        bool operator==(const it& that) const { return position == that.position; }
        bool operator!=(const it& that) const { return !(*this == that); }
    };

    it begin() { return {*this, 0}; }
    it end() { return {*this, _size}; }
};

class NAL {
    x264_nal_t *nal;
    size_t frame_size;
public:
    NAL() = default;
    auto payload() {
        return arr_view(nal ? nal->p_payload : nullptr, frame_size);
    }
    const auto payload() const {
        return arr_view(nal ? nal->p_payload : nullptr, frame_size);
    }
    friend class Codec;
};

class Picture {
    x264_picture_t pic;
    Picture() = default;
public:
    size_t height = 0, width = 0;

    int64_t& i_pts();
    int64_t i_pts() const;

    arr_view<uint8_t> get_luma() const;
    arr_view<uint8_t> get_cr() const;
    arr_view<uint8_t> get_cb() const;

    ~Picture();
    friend class Codec;
};

void gen_frame(Picture& pic);

class Codec {
    x264_param_t param;
    x264_t *h{nullptr};

    NAL encode_(x264_picture_t* pic = NULL);

public:
    Codec(int width = 352, int height = 288);
    ~Codec();

    Picture make_picture();

    NAL encode(Picture& pic);

    NAL encode();

    int delayed_frames_count();
};

class Decoder {
    const AVCodec *codec;
    AVCodecContext *codec_context;
    AVCodecParserContext *parser;
    AVFrame *frame;
    AVPacket *pkt;

    std::function<void(const AVFrame&)> frame_callback;
public:
    Decoder(std::function<void(const AVFrame&)> frame_callback);
    ~Decoder();

    void feed(arr_view<uint8_t> buf);
};
