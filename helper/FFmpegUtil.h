//
// Created by mingj on 2019/10/19.
//

#ifndef NDN_FFMPEG_RTC_FFMPEGUTIL_H
#define NDN_FFMPEG_RTC_FFMPEGUTIL_H

#include <functional>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}
namespace EasyFFmpeg {
    class FFmpegUtil {
        typedef std::function<void(AVFrame *pFrame)> DecodeCallbackFunc;
        typedef std::function<void(AVPacket *packet)> EncodeCallbackFunc;
    public:
        static int findFirstStreamIndexByType(AVFormatContext *pFormatCtx, enum AVMediaType avMediaType);

        static AVFrame *
        allocAVFrameAndDataBufferWithType(enum AVPixelFormat pix_fmt, int width, int height, int align = 1);

        static SwsContext *
        SWS_GetContext(AVCodecContext *pCodecContext, enum AVPixelFormat dstFormat, int flags = SWS_BICUBIC,
                       SwsFilter *srcFilter = nullptr, SwsFilter *dstFilter = nullptr, const double *param = nullptr);

        static bool
        decode(AVCodecContext *pCodecCtx, AVPacket *packet, AVFrame *pFrame, const DecodeCallbackFunc &callbackFunc);
        static void
        encode(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *packet, const EncodeCallbackFunc &callbackFunc);
    };
}


#endif //NDN_FFMPEG_RTC_FFMPEGUTIL_H
