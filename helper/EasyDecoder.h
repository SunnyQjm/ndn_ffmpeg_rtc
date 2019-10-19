//
// Created by mingj on 2019/10/18.
//

#ifndef NDN_FFMPEG_RTC_EASYDECODER_H
#define NDN_FFMPEG_RTC_EASYDECODER_H

#include <functional>
#include "FFmpegFailedException.h"

extern "C"
{
#include <libavcodec/avcodec.h>
};

class EasyDecoder {
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
    explicit EasyDecoder(enum AVCodecID codecId);

    EasyDecoder *prepareDecode();

    EasyDecoder *decode(AVPacket *packet, const DecodeCallbackFunc &callback);

    ~EasyDecoder();
};


#endif //NDN_FFMPEG_RTC_EASYDECODER_H
