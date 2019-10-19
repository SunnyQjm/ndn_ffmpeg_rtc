//
// Created by mingj on 2019/10/16.
//

#ifndef NDN_FFMPEG_RTC_FFMPEGHELPER_H
#define NDN_FFMPEG_RTC_FFMPEGHELPER_H

#include <iostream>
#include <functional>
#include <FFmpegFailedException.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

#define USE_FFMPEG

/**
 * 理解ffmpeg中的pts，dts，time_base => https://blog.csdn.net/weixin_30532987/article/details/97188953
 */
class FFmpegHelper {
    typedef std::function<void(AVFrame *pFrame)> DecodeCallbackFunc;

private:
    AVFormatContext* pFormatContext = nullptr;
public:
    FFmpegHelper();
    ~FFmpegHelper();
    static void init();
    FFmpegHelper* initFormatContext();
    FFmpegHelper* openCamera(const std::string& av_input_short_name = "video4linux2", const std::string& url = "/dev/video0");
    int findFirstStreamIndexByType(enum AVMediaType avMediaType);
    AVCodecContext* openCodec(enum AVMediaType avMediaType);
    static AVFrame * allocAVFrameAndDataBufferWithType(enum AVPixelFormat pix_fmt, int width, int height, int align = 1);
    static AVPacket * allocAVPacket();
    static SwsContext *SWS_GetContext(AVCodecContext *pCodecContext, enum AVPixelFormat dstFormat, int flags = SWS_BICUBIC,
            SwsFilter* srcFilter = nullptr, SwsFilter* dstFilter = nullptr, const double *param = nullptr);

    FFmpegHelper* decode(AVCodecContext* avCodecContext, AVPacket* packet, AVFrame* pFrame,
                         const DecodeCallbackFunc& callbackFunc);

    AVFormatContext* getFormatContext();
};


#endif //NDN_FFMPEG_RTC_FFMPEGHELPER_H
