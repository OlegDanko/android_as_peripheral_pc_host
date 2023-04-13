#include "Codec.hpp"

#include <stdexcept>

int64_t& Picture::i_pts() { return pic.i_pts; }

int64_t Picture::i_pts() const { return pic.i_pts; }

arr_view<uint8_t> Picture::get_luma() const { return {pic.img.plane[0], width*height}; }

arr_view<uint8_t>Picture::get_cr() const { return {pic.img.plane[1], width*height/4}; }

arr_view<uint8_t>Picture::get_cb() const { return {pic.img.plane[2], width*height/4}; }

Picture::~Picture() {
    if(height && width)
        x264_picture_clean(&pic);
}

void gen_frame(Picture &pic) {
    int off = pic.i_pts() * 10;

    int w = pic.width, h = pic.height;

    auto luma = pic.get_luma();
    auto luma_it = luma.begin();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            *luma_it = x + y + off * 3;
            ++luma_it;
        }
    }

    w /= 2;
    h /= 2;
    auto cr = pic.get_cr();
    auto cb = pic.get_cb();
    auto cr_it = cr.begin();
    auto cb_it = cb.begin();
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            *cr_it = 128 + y + off * 2;
            *cb_it = 64 + x + off * 5;
            ++cr_it;
            ++cb_it;
        }
    }
}

NAL Codec::encode_(x264_picture_t *pic) {
    NAL nal;
    int i_nal; // TODO: research
    x264_picture_t pic_out; // TODO: research

    nal.frame_size = x264_encoder_encode( h, &nal.nal, &i_nal, pic, &pic_out );
    if(nal.frame_size < 0)
        throw std::runtime_error("x264_encoder_encode failed");

    return nal;
}

Codec::Codec(int width, int height) {
    if( x264_param_default_preset( &param, "ultrafast", "zerolatency" ) < 0 )
        throw std::runtime_error("x264_param_default_preset failed");

    param.i_csp = X264_CSP_I420;
    param.i_width  = width;
    param.i_height = height;
    param.b_vfr_input = 0;
    param.b_repeat_headers = 1;
    param.b_annexb = 1;

    if(x264_param_apply_profile( &param, "high" ) < 0)
        throw std::runtime_error("x264_param_default_preset failed");


    h = x264_encoder_open( &param );
    if(!h)
        throw std::runtime_error("x264_encoder_open failed");
}

Codec::~Codec() {
    if(h) x264_encoder_close(h);
}

Picture Codec::make_picture() {
    Picture pic;
    pic.pic.i_type = X264_TYPE_IDR;
    pic.i_pts() = 0;

    if(x264_picture_alloc( &pic.pic, param.i_csp, param.i_width, param.i_height ) < 0 ) {
        throw std::runtime_error("x264_encoder_open failed");
    }
    pic.width = param.i_width;
    pic.height = param.i_height;

    return pic;
}

NAL Codec::encode(Picture &pic) {
    return encode_(&pic.pic);
}

NAL Codec::encode() {
    return encode_();
}

int Codec::delayed_frames_count() {
    return x264_encoder_delayed_frames(h);
}

Decoder::Decoder(std::function<void (const AVFrame &)> frame_callback)
    : frame_callback(frame_callback) {
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    codec_context = avcodec_alloc_context3(codec);
    parser = av_parser_init(codec->id);
    frame = av_frame_alloc();
    pkt = av_packet_alloc();

    avcodec_open2(codec_context, codec, NULL);
}

Decoder::~Decoder() {
    av_parser_close(parser);
    avcodec_free_context(&codec_context);
    av_frame_free(&frame);
    av_packet_free(&pkt);
}

void Decoder::feed(arr_view<uint8_t> buf) {
    while(buf.data()) {
        auto ret = av_parser_parse2(parser,
                                    codec_context,
                                    &pkt->data,
                                    &pkt->size,
                                    buf.data(),
                                    buf.size(),
                                    AV_NOPTS_VALUE,
                                    AV_NOPTS_VALUE,
                                    0);

        if (ret < 0) {
            fprintf(stderr, "Error while parsing\n");
            exit(1);
        }
        buf = buf.sub_view(ret);

        if (pkt->size) {
            auto ret = avcodec_send_packet(codec_context, pkt);
            if (ret < 0) {
                fprintf(stderr, "Error sending a packet for decoding\n");
                exit(1);
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(codec_context, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    fprintf(stderr, "Error during decoding\n");
                    exit(1);
                }
                if(frame_callback) frame_callback(*frame);
            }

        } else if (buf.size() == 0) {
            break;
        }
    }
}
