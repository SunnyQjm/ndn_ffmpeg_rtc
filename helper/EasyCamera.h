//
// Created by mingj on 2019/10/19.
//

#ifndef NDN_FFMPEG_RTC_EASYCAMERA_H
#define NDN_FFMPEG_RTC_EASYCAMERA_H

#include <functional>
#include "FFmpegFailedException.h"
#include "FFmpegUtil.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
};

class EasyCamera {
    typedef std::function<bool(AVFrame *pFrameYUV)> CameraCaptureCallbackFunc;
private:
    AVFrame *pFrame{}, *pFrameYUV{}, *pFrameRgb{};
    AVPacket *packet{};
    SwsContext *yuvImageConvert{};
    SwsContext *rgbImageConvert{};
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx{};
    AVCodec *pCodec{};
    int videoIndex = -1;

    static void throwException(const std::string &msg);

public:
    EasyCamera();

    ~EasyCamera();

    EasyCamera *
    openCamera(const std::string &av_input_short_name = "video4linux2", const std::string &url = "/dev/video0");

    EasyCamera *prepare();

    EasyCamera *begin(const CameraCaptureCallbackFunc &callback);
    EasyCamera *begin(const CameraCaptureCallbackFunc &yuvCallback, const CameraCaptureCallbackFunc &rgbCallback);

    // Get
    AVCodecContext *getCodecCtx() { return pCodecCtx; }
};


#endif //NDN_FFMPEG_RTC_EASYCAMERA_H
