//
// Created by mingj on 2019/10/31.
//

#ifndef NDN_FFMPEG_RTC_EASYMICROPHONE_H
#define NDN_FFMPEG_RTC_EASYMICROPHONE_H

#include <functional>
#include "FFmpegFailedException.h"
#include "FFmpegUtil.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
};

class EasyMicroPhone {
    typedef std::function<bool(AVFrame *pFrame)> MicrophoneCallbackFunc;
private:
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVFrame *pAudioFrame = nullptr;
    AVFrame *pAudioFrameOut = nullptr;
    AVPacket *packet = nullptr;
    int audioIndex = -1;
    struct SwrContext *audioConvertCtx = nullptr;
    static void throwException(const std::string &msg);

public:
    EasyMicroPhone();
    ~EasyMicroPhone();
    EasyMicroPhone* openMicrophone(const std::string &av_input_short_name = "alsa", const std::string &url = "default");
    EasyMicroPhone* prepare();
    EasyMicroPhone* begin(const MicrophoneCallbackFunc& callback);

};


#endif //NDN_FFMPEG_RTC_EASYMICROPHONE_H
