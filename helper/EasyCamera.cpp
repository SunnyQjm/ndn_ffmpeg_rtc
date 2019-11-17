//
// Created by mingj on 2019/10/19.
//

#include "EasyCamera.h"
#include <iostream>

void EasyCamera::throwException(const std::string &msg) {
    std::string logMsg = "EasyCamera -> " + msg;
    throw FFmpegFailedException(logMsg);
}

EasyCamera::EasyCamera() {
    av_register_all();
    avformat_network_init();
    avdevice_register_all();

    pFormatCtx = avformat_alloc_context();
}

EasyCamera::~EasyCamera() {
    av_free(pFrameYUV->data[0]);
    av_free(pFrameYUV);
    av_free(pFrameRgb->data[0]);
    av_free(pFrameRgb);
    sws_freeContext(yuvImageConvert);
    avcodec_close(pCodecCtx);
    avformat_free_context(pFormatCtx);
}

EasyCamera *EasyCamera::openCamera(const std::string &av_input_short_name, const std::string &url) {
    // Linux 打开摄像头设备的输入流
    AVInputFormat *avInputFormat = av_find_input_format(av_input_short_name.c_str());
    av_register_input_format(avInputFormat);
    if (avformat_open_input(&pFormatCtx, url.c_str(), avInputFormat, nullptr) != 0) {
        throw FFmpegFailedException("Couldn't open input stream.");
    }
    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        throw FFmpegFailedException("Couldn't find stream information.");
    }
    return this;
}

EasyCamera *EasyCamera::prepare() {
    // 找到解码器并打开
    videoIndex = EasyFFmpeg::FFmpegUtil::findFirstStreamIndexByType(this->pFormatCtx, AVMEDIA_TYPE_VIDEO);
    pCodecCtx = this->pFormatCtx->streams[videoIndex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == nullptr) {
        throw FFmpegFailedException("Codec not found.");
    }
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        throw FFmpegFailedException("Could not open codec.");
    }

    // 给pFrame和pFrameYUV分配空间
    pFrame = av_frame_alloc();
    pFrameYUV = EasyFFmpeg::FFmpegUtil::allocAVFrameAndDataBufferWithType(AV_PIX_FMT_YUV420P, pCodecCtx->width,
                                                                          pCodecCtx->height);
    pFrameRgb = EasyFFmpeg::FFmpegUtil::allocAVFrameAndDataBufferWithType(AV_PIX_FMT_RGB32, pCodecCtx->width,
                                                                          pCodecCtx->height);

    pFrameYUV->format = AV_PIX_FMT_YUV420P;
    pFrameYUV->width = pCodecCtx->width;
    pFrameYUV->height = pCodecCtx->height;

    pFrameRgb->format = AV_PIX_FMT_RGB32;
    pFrameRgb->width = pCodecCtx->width;
    pFrameRgb->height = pCodecCtx->height;

    packet = av_packet_alloc();

    yuvImageConvert = EasyFFmpeg::FFmpegUtil::SWS_GetContext(pCodecCtx, AV_PIX_FMT_YUV420P);
    rgbImageConvert = EasyFFmpeg::FFmpegUtil::SWS_GetContext(pCodecCtx, AV_PIX_FMT_RGB32);

    return this;
}

EasyCamera *EasyCamera::begin(const EasyCamera::CameraCaptureCallbackFunc &callback) {
    bool exit = false;
    while (!exit) {
        if (av_read_frame(pFormatCtx, packet) >= 0) {
            if (packet->stream_index == videoIndex) {
                EasyFFmpeg::FFmpegUtil::decode(pCodecCtx, packet, pFrame, [=, &exit](AVFrame *frame) {
                    /**
                     * sws_scale
                     * https://blog.csdn.net/u010029439/article/details/82859206
                     */
                    sws_scale(yuvImageConvert,
                              (const unsigned char *const *) frame->data,
                              frame->linesize,
                              0,
                              pCodecCtx->height,
                              pFrameYUV->data,
                              pFrameYUV->linesize);
                    exit = callback(pFrameYUV);
                });
            }
        } else {
            exit = true;
        }
    }
    return this;
}

EasyCamera *EasyCamera::begin(const EasyCamera::CameraCaptureCallbackFunc &yuvCallback,
                              const EasyCamera::CameraCaptureCallbackFunc &rgbCallback) {
    bool exit = false;
    while (!exit) {
        if (av_read_frame(pFormatCtx, packet) >= 0) {
            if (packet->stream_index == videoIndex) {
                EasyFFmpeg::FFmpegUtil::decode(pCodecCtx, packet, pFrame, [=, &exit](AVFrame *frame) {
                    /**
                     * sws_scale
                     * https://blog.csdn.net/u010029439/article/details/82859206
                     */
                    sws_scale(rgbImageConvert,
                              (const unsigned char *const *) frame->data,
                              frame->linesize,
                              0,
                              pCodecCtx->height,
                              pFrameRgb->data,
                              pFrameRgb->linesize
                    );
                    rgbCallback(pFrameRgb);
                    sws_scale(yuvImageConvert,
                              (const unsigned char *const *) frame->data,
                              frame->linesize,
                              0,
                              pCodecCtx->height,
                              pFrameYUV->data,
                              pFrameYUV->linesize);
                    exit = yuvCallback(pFrameYUV);
                });
            }
        } else {
            exit = true;
        }
    }
    return this;
    return nullptr;
}
