//
// Created by mingj on 2019/10/18.
//

#ifndef NDN_FFMPEG_RTC_EASYENCODER_H
#define NDN_FFMPEG_RTC_EASYENCODER_H

#include <functional>
#include "FFmpegFailedException.h"

extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
};
struct CodecContextParam {
    int64_t bit_rate;
    int width;
    int height;
    AVRational time_base;
    int gop_size;
    int max_b_frames;
    enum AVPixelFormat pix_fmt;
};

class EasyEncoder {
    typedef std::function<void(AVPacket *pkt)> EncodeCallbackFunc;

private:
    AVPacket pkt{};
    AVCodecContext *pCodecContext = nullptr;
    AVCodec *pCodec = nullptr;

    static void throwException(const std::string& msg);

public:
    EasyEncoder(enum AVCodecID codecId);
    /**
     * 设置一些编码参数
     * @param param
     * @return
     */
    EasyEncoder* initCodecParam(const CodecContextParam& param);
    /**
     * 完成一些编码前的准备工作
     * @return
     */
    EasyEncoder* prepareEncode();

    /**
     * 编码
     * @param pFrame
     * @param callback
     * @return
     */
    EasyEncoder* encode(AVFrame *pFrame, const EncodeCallbackFunc& callback);
    ~EasyEncoder();
};


#endif //NDN_FFMPEG_RTC_EASYENCODER_H
