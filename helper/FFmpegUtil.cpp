//
// Created by mingj on 2019/10/19.
//

#include "FFmpegUtil.h"
#include <iostream>

using namespace std;

int EasyFFmpeg::FFmpegUtil::findFirstStreamIndexByType(AVFormatContext *pFormatCtx, enum AVMediaType avMediaType) {
    if (!pFormatCtx)
        return -1;
    cout << "nb_streams:" << pFormatCtx->nb_streams << endl;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == avMediaType) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

AVFrame *EasyFFmpeg::FFmpegUtil::allocAVFrameAndDataBufferWithType(enum AVPixelFormat pix_fmt, int width, int height,
                                                                   int align) {
    AVFrame *pFrame = av_frame_alloc();
    u_char *outBuffer = nullptr;
    outBuffer = (u_char *) av_malloc(av_image_get_buffer_size(pix_fmt, width, height, align));
    av_image_fill_arrays(pFrame->data, pFrame->linesize, outBuffer, pix_fmt, width, height, align);
    return pFrame;
}


AVFrame *EasyFFmpeg::FFmpegUtil::allocAVFrameAndDataBufferWithTyp(enum AVSampleFormat sampleFormat, int nbChannels,
                                                                  int nbSamples, int align) {
    AVFrame *pFrame = av_frame_alloc();
    uint8_t *outBuffer = nullptr;
    outBuffer = (uint8_t *) av_malloc(av_samples_get_buffer_size(nullptr, nbChannels, nbSamples, sampleFormat, align));
    av_samples_fill_arrays(pFrame->data, pFrame->linesize, outBuffer, nbChannels, nbSamples, sampleFormat, align);
    return pFrame;
}


SwsContext *
EasyFFmpeg::FFmpegUtil::SWS_GetContext(AVCodecContext *pCodecContext, enum AVPixelFormat dstFormat, int flags,
                                       SwsFilter *srcFilter, SwsFilter *dstFilter, const double *param) {
    SwsContext *swsContext = nullptr;
    swsContext = sws_getContext(pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt,
                                pCodecContext->width,
                                pCodecContext->height, dstFormat, flags, srcFilter, dstFilter, param);
    return swsContext;
}

void EasyFFmpeg::FFmpegUtil::decode(AVCodecContext *pCodecCtx, AVPacket *packet, AVFrame *pFrame,
                                    const EasyFFmpeg::FFmpegUtil::DecodeCallbackFunc &callbackFunc) {
    if (packet == nullptr || pFrame == nullptr)
        return;
    int ret = avcodec_send_packet(pCodecCtx, packet);
    if (ret != 0)
        return;
    while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
        callbackFunc(pFrame);
    }
    av_packet_unref(packet);
}

void EasyFFmpeg::FFmpegUtil::encode(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *packet,
                                    const EasyFFmpeg::FFmpegUtil::EncodeCallbackFunc &callbackFunc) {
    if (packet == nullptr || pFrame == nullptr)
        return;
    int ret = avcodec_send_frame(pCodecCtx, pFrame);
    if (ret != 0)
        return;
    while (avcodec_receive_packet(pCodecCtx, packet) >= 0) {
        callbackFunc(packet);
    }
    av_packet_unref(packet);
}

