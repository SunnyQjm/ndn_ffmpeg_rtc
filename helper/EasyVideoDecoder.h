//
// Created by mingj on 2019/10/18.
//

#ifndef NDN_FFMPEG_RTC_EASYVIDEODECODER_H
#define NDN_FFMPEG_RTC_EASYVIDEODECODER_H

#include <functional>
#include "FFmpegFailedException.h"

extern "C"
{
#include <libavcodec/avcodec.h>
};

/**
 * 基于FFmpeg实现的视频解码器
 *
 */
class EasyVideoDecoder {
    typedef std::function<void(AVFrame *frame)> DecodeCallbackFunc;
private:
    AVCodec *pCodec{};
    AVCodecContext *pCodecCtx{};
    AVCodecParserContext *pCodecParserCtx{};
    AVFrame *pFrame{};
    enum AVCodecID codecId;
    AVPacket pkt{};

    static void throwException(const std::string &msg);

public:
    /**
     * 初始化解码器
     * @param codecId 待解码的码流的编码类型
     */
    explicit EasyVideoDecoder(enum AVCodecID codecId);

    /**
     * 解码前的准备，包括初始化解码器，分配一些必要的数据结构的空间等等
     * => 在调用decode方法进行解码之前需要调用本接口，且本接口只需要调用一次，便可多次调用decode方法
     */
    EasyVideoDecoder *prepareDecode();

    AVPacket *parse(const uint8_t *buf, size_t size);

    /**
     * 对一个压缩后的packet进行解码成YUV格式的AVFrame
     * @param packet         待解码的AVPacket（在解码结束后会对传入的AVPacket指针调用av_packet_unref）
     * @param callback       回调函数，在解码完毕后会调用，可以在这个回调里面处理解码的结果
     * @return
     */
    EasyVideoDecoder *decode(AVPacket *packet, const DecodeCallbackFunc &callback);

    ~EasyVideoDecoder();
};


#endif //NDN_FFMPEG_RTC_EASYVIDEODECODER_H
