//
// Created by mingj on 2019/10/19.
//

#ifndef NDN_FFMPEG_RTC_EASYCAMERA_H
#define NDN_FFMPEG_RTC_EASYCAMERA_H

#include <functional>
#include "FFmpegFailedException.h"
#include <FFmpegUtil.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
};

class EasyCamera {
    typedef std::function<bool(AVFrame *pFrameYUV)> CameraCaptureCallbackFunc;
private:
    AVFrame *pFrame{}, *pFrameYUV{};
    AVPacket *packet{};
    SwsContext *imageConvertCtx{};
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx{};
    AVCodec *pCodec{};

    static void throwException(const std::string &msg);

public:
    EasyCamera();

    ~EasyCamera();

    EasyCamera *
    openCamera(const std::string &av_input_short_name = "video4linux2", const std::string &url = "/dev/video0");

    EasyCamera *prepare();

    EasyCamera *begin(const CameraCaptureCallbackFunc &callback);

    // Get
    AVCodecContext *getCodecCtx() { return pCodecCtx; }
};


#endif //NDN_FFMPEG_RTC_EASYCAMERA_H
