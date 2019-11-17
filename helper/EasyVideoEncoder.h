//
// Created by mingj on 2019/10/18.
//

#ifndef NDN_FFMPEG_RTC_EASYVIDEOENCODER_H
#define NDN_FFMPEG_RTC_EASYVIDEOENCODER_H

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

class EasyVideoEncoder {
public:
    typedef std::function<void(AVPacket *pkt)> EncodeCallbackFunc;

private:
    AVPacket pkt{};
    AVCodecContext *pCodecContext = nullptr;
    AVCodec *pCodec = nullptr;
    enum AVCodecID codecId;

    static void throwException(const std::string &msg);

public:
    explicit EasyVideoEncoder(enum AVCodecID codecId);

    /**
     * 设置一些编码参数
     * @param param
     * @return
     */
    EasyVideoEncoder *initCodecParam(const CodecContextParam &param);

    /**
     * 完成一些编码前的准备工作
     * @return
     */
    EasyVideoEncoder *prepareEncode();

    /**
     * 编码
     * @param pFrame
     * @param callback
     * @return
     */
    EasyVideoEncoder *encode(AVFrame *pFrame, const EncodeCallbackFunc &callback);

    ~EasyVideoEncoder();
};


#endif //NDN_FFMPEG_RTC_EASYVIDEOENCODER_H
