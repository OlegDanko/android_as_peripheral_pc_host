// https://github.com/infinite-tape/x264/blob/master/example.c

#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <type_traits>
#include "Codec.hpp"
#include <bit_io/BitReader.hpp>
#include <bit_io/BitIOTypes.hpp>

#include <rtc/datachannel.hpp>
#include <rtc/rtc.hpp>

#include <thread>

#define repeat(_repeat_times) for(size_t _repeat_times_iterator = 0; _repeat_times_iterator < _repeat_times; _repeat_times_iterator++)

template<typename T>
auto& as_int_ref(T& val) {
    if constexpr (sizeof(T) == sizeof(uint8_t))
        return (*reinterpret_cast<uint8_t*>(&val));
    else if constexpr (sizeof(T) == sizeof(uint16_t))
        return (*reinterpret_cast<uint16_t*>(&val));
    else if constexpr (sizeof(T) == sizeof(uint32_t))
        return (*reinterpret_cast<uint32_t*>(&val));
    else if constexpr (sizeof(T) == sizeof(uint64_t))
        return (*reinterpret_cast<uint64_t*>(&val));
}

//template<typename T>
//typename std::enable_if<std::is_floating_point_v<T>, i_bin_read_stream&>::type
//operator>>(i_bin_read_stream& str, T& val) {
//    return str >> as_int_ref(val);
//}


template<typename T, typename str_t>
typename std::enable_if<std::is_integral_v<T>, T>::type
read_bytes(str_t& str) {
    T val = 0;
    size_t i = sizeof(T);
    do {
        val |= str.read_byte();
        val <<= 8*(--i > 0);
    } while(i > 0);
    return val;
}

template<typename T, size_t N, typename str_t>
std::array<T, N> read_bytes_array(str_t& str) {
    std::array<T, N> arr;
    for(auto& a : arr) a = read_bytes<T>(str);
    return arr;
}

template<typename str_t, typename T>
typename std::enable_if<std::is_integral_v<T>, str_t&>::type
write_bytes(str_t& str, T& val) {
    size_t i = sizeof(T);
    do {
        str.write_byte(val >> --i*8);
    } while(i > 0);
    return str;
}

class bin_read_stream {
    std::vector<uint8_t>& data;
    size_t position{0};
public:
    bin_read_stream(std::vector<uint8_t>& data) : data(data) {}

    uint8_t read_byte() {
        if(position == data.size()) return 0; // failed
        return data.at(position++);
    }
    template<typename T>
    T read() {

    }

};

class bin_write_stream {
    std::vector<uint8_t> data{};
public:
    void write_byte(uint8_t b) {
        data.push_back(b);
    }

    template<typename T>
    bin_write_stream& write(const T& t) {
        return write_bytes(*this, t);
    }

    std::vector<uint8_t> take_data() {
        std::vector<uint8_t> taken{};
        std::swap(data, taken);
        return taken;
    }
};


template<typename BYTE>
class arr_view_bin_stream {
    arr_view<BYTE> data;
    size_t position{0};
public:
    arr_view_bin_stream(arr_view<BYTE> data) : data(data) {}

    BYTE read_byte() {
        if(position == data.size()) return 0; // failed, throw
        return data.at(position++);
    }

    template<typename T>
    T read() {
        return read_bytes<T>(*this);
    }
    template<typename T>
    const T& peek() const {
        if(position + sizeof(T) > data.size()) throw std::out_of_range("Naughty peaking");
        return *reinterpret_cast<const T*>(&data.at(position));
    }
    size_t remaining() const { return data.size() - position; }
};

class FramesGenerator {
    using nal_callback_t = std::function<void(const NAL& nal)>;
    Codec codec;
    Picture picture{codec.make_picture()};
    nal_callback_t nal_callback;

    std::atomic_bool running{true};
    std::thread thread;
public:
    FramesGenerator(nal_callback_t on_nal) : nal_callback(on_nal) {
        thread = std::thread([this]{
            while(running.load()) {
                gen_frame(picture);
                nal_callback(codec.encode(picture));
                picture.i_pts()++;
            }
        });
    }
    ~FramesGenerator() {
        running = false;
        thread.join();
    }
};

enum EStartPrefixType {
    PREFIX_NONE,
    PREFIX_3_BYTE,
    PREFIX_4_BYTE
};

template<typename T, typename ... Args>
bool is_one_of(const T& t, const Args...args) {
    return (... | (t == args));
}


class NALProducer {
    std::shared_ptr<rtc::RtpPacketizationConfig> packetizer_config;
    rtc::H264RtpPacketizer packetizer;
    FramesGenerator gen;
    std::unordered_map<uint8_t, std::vector<uint8_t>> initial_nal_units;

    template<size_t N>
    using bytes_t = std::array<uint8_t, N>;

    EStartPrefixType check_new_nal(const arr_view_bin_stream<uint8_t>& view) {
        if(view.remaining() < 4) [[unlikely]]
            return PREFIX_NONE;

        auto peek = view.peek<uint32_t>();

        return peek == 0x01000000
            ? PREFIX_4_BYTE :
                   (peek << 8 == 0x01000000)
                   ? PREFIX_3_BYTE
                   : PREFIX_NONE;
    }

    template<typename FN_ON_BYTE, typename FN_ON_DONE>
    void parse_nal(arr_view_bin_stream<uint8_t>& packet_view,
                   FN_ON_BYTE on_byte,
                   FN_ON_DONE on_done) {
        // omg saving so much CPU time skipping the last 3 bytes /s
        while(packet_view.remaining() > 3) {
            switch(check_new_nal(packet_view)) {
            case PREFIX_4_BYTE:
                packet_view.read_byte();
            case PREFIX_3_BYTE:
                repeat(3) { packet_view.read_byte(); }
                if constexpr (!std::is_same_v<FN_ON_DONE, std::nullptr_t>)
                    on_done();
                return;
            default:;
            }
            if constexpr (!std::is_same_v<FN_ON_BYTE, std::nullptr_t>)
                on_byte(packet_view.read<uint8_t>());
            else
                packet_view.read<uint8_t>();
        }
        if constexpr (!std::is_same_v<FN_ON_BYTE, std::nullptr_t>) {
            repeat(3) { on_byte(packet_view.read<uint8_t>()); }
        } else {
            repeat(3) { packet_view.read<uint8_t>(); }
        }

        if constexpr (!std::is_same_v<FN_ON_DONE, std::nullptr_t>)
            on_done();
    }

    void save_nal(arr_view_bin_stream<uint8_t>& packet_view, uint8_t type) {
        std::vector<uint8_t> nal;
        parse_nal(packet_view, [&nal](uint8_t b){ nal.push_back(b); }, nullptr);

        initial_nal_units[type] = nal; // TODO: mutex
    }
    void skip_nal(arr_view_bin_stream<uint8_t>& packet_view) {
        parse_nal(packet_view, nullptr, nullptr);
    }

    void save_initial_nalus(const NAL& nal) {
        std::cout << "Searching for initial nalus in a new packet" << std::endl;
//        auto packet = nal.payload();
//        auto packet_view = arr_view_bin_stream(packet);

//        while(packet_view.remaining())
//            std::cout << std::hex << (int)packet_view.read_byte() << " ";
//        std::cout << std::endl << "end" << std::endl;
//        return;
//        skip_nal(packet_view); // should just skip the starting sequence
//        while(packet_view.remaining() > 0) {
//            rtc::NalUnitHeader header{._first = packet_view.read<uint8_t>()};

//            std::cout << "unit type " << (int)header.unitType() << std::endl;

//            if(is_one_of(header.unitType(), 7, 8, 5)) {
//                save_nal(packet_view, header.unitType());
//            } else {
//                skip_nal(packet_view);
//            }
//        }

    }
    void serve_nal(const NAL& nal) {
        auto packet = nal.payload();

        auto bytes = std::make_shared<std::vector<std::byte>>();

        for(auto b : packet)
            bytes->push_back((std::byte)b);

        auto packetized = packetizer.packetize(std::move(bytes), false);

        std::cout << "New packet:\n";

        std::vector<uint8_t> packet_ized;
        for(auto b : *packetized) {
            packet_ized.push_back((uint8_t)b);
            std::cout << std::hex << (int)b << " ";
        }
        std::cout << std::endl;

        bit_io::BitReader reader(packet_ized);

        std::cout << "Header: " << std::hex
                  << " V = " << (int)bit_io::type_t<uint8_t, 2>(reader).val
                  << " P = " << (int)bit_io::type_t<uint8_t, 1>(reader).val
                  << " X = " << (int)bit_io::type_t<uint8_t, 1>(reader).val
                  << " CC = " << (int)bit_io::type_t<uint8_t, 4>(reader).val
                  << " M = " << (int)bit_io::type_t<uint8_t, 1>(reader).val
                  << " PT = " << (int)bit_io::type_t<uint8_t, 7>(reader).val
                  << " SEQ = " << ntohs(bit_io::type_t<uint16_t>(reader).val)
                  << " TM = " << ntohs(bit_io::type_t<uint32_t>(reader).val)
                  << " SSRC = " << ntohs(bit_io::type_t<uint32_t>(reader).val)
                  << std::endl;


//        save_initial_nalus(nal);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
public:
    NALProducer()
        : packetizer_config(std::make_shared<rtc::RtpPacketizationConfig>(
            1, "video-stream", 102, rtc::H264RtpPacketizer::defaultClockRate
            ))
        , packetizer(rtc::H264RtpPacketizer::Separator::Length, packetizer_config)
        , gen([this](const auto& n){ serve_nal(n); })
    { }
};

int main(int argc, char **argv) {
    rtc::PeerConnection peer;

    std::string name = "video-stream";
    std::string msid = "stream1";
    uint8_t payloadType = 102;
    uint32_t ssrc = 1;

    rtc::Description::Video vid(name);
    vid.addH264Codec(payloadType);
    vid.addSSRC(ssrc, name, msid, name);
    auto track = peer.addTrack(vid);

    auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(
        ssrc, name, payloadType, rtc::H264RtpPacketizer::defaultClockRate);

    auto packetizer = std::make_shared<rtc::H264RtpPacketizer >(
        rtc::H264RtpPacketizer::Separator::Length, rtpConfig);

    auto h264Handler = std::make_shared<rtc::H264PacketizationHandler>(packetizer);
    auto srReporter = std::make_shared<rtc::RtcpSrReporter>(rtpConfig);
    h264Handler->addToChain(srReporter);
    auto nackResponder = std::make_shared<rtc::RtcpNackResponder>();
    h264Handler->addToChain(nackResponder);
    // set handler
    track->setMediaHandler(h264Handler);
    track->onOpen([](){ std::cout << "opened" << std::endl; });

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main_write_to_file(int argc, char **argv) {
    std::atomic_size_t frames = 0;    

    FramesGenerator gen([&](const auto& nal){
        auto frame = frames.fetch_add(1);
        frames.notify_one();

        auto payload = nal.payload();
        auto file_name = (std::stringstream() << "sample-" << frame << ".h264").str();
        FILE *outfile = fopen(file_name.c_str(), "wb");
        if(!payload.size()) return;
        if(!fwrite( payload.data(), payload.size(), 1, outfile ))
            throw std::runtime_error("fwrite failed");
        fclose(outfile);
    });

    while(frames.load() < 320)
        frames.wait(0);

    return 0;
}
