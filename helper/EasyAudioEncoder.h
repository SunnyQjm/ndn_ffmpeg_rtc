//
// Created by mingj on 2019/10/31.
//

#ifndef NDN_FFMPEG_RTC_EASYAUDIOENCODER_H
#define NDN_FFMPEG_RTC_EASYAUDIOENCODER_H

#include <functional>
extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
};

/**
 * 音频PCM裸流压缩编码器
 */
class EasyAudioEncoder {
public:
    typedef std::function<void(AVPacket *pkt)> EncodeCallbackFunc;
    struct CodecContextParam {
        enum AVSampleFormat sampleFormat;
        int sampleRate;
        uint64_t channelLayout;
        int bitRate;
    };

private:
    AVPacket pkt{};
    AVCodecContext *pCodecContext = nullptr;
    AVCodec *pCodec = nullptr;
    enum AVCodecID codecId;

    static void throwException(const std::string &msg);

public:
    explicit EasyAudioEncoder(enum AVCodecID codecId);

    /**
     * 设置一些编码参数
     * @param param
     * @return
     */
    EasyAudioEncoder* initCodecParam(const CodecContextParam &param);

    /**
     * 完成一些编码前的准备工作
     * @return
     */
    EasyAudioEncoder* prepareEncode();

    /**
     * 编码
     * @param pFrame
     * @param callback
     * @return
     */
    EasyAudioEncoder *encode(AVFrame* pFrame, const EncodeCallbackFunc &callback);

    ~EasyAudioEncoder();
};


#endif //NDN_FFMPEG_RTC_EASYAUDIOENCODER_H
