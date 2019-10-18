//
// Created by mingj on 2019/10/16.
//

#include "FFmpegHelper.h"

void FFmpegHelper::init() {
    av_register_all();
    avformat_network_init();
    avdevice_register_all();
}

FFmpegHelper::FFmpegHelper() {

}

FFmpegHelper::~FFmpegHelper() {
    // 自动释放资源
    if (pFormatContext) {
        avformat_free_context(pFormatContext);
    }
}

FFmpegHelper *FFmpegHelper::initFormatContext() {
    pFormatContext = avformat_alloc_context();
    return this;
}

FFmpegHelper *FFmpegHelper::openCamera(const std::string &av_input_short_name, const std::string &url) {
    // Linux 打开摄像头设备的输入流
    AVInputFormat *avInputFormat = av_find_input_format(av_input_short_name.c_str());
    if (avformat_open_input(&pFormatContext, url.c_str(), avInputFormat, nullptr) != 0) {
        throw FFmpegFailedException("Couldn't open input stream.");
    }

    if (avformat_find_stream_info(pFormatContext, nullptr) < 0) {
        throw FFmpegFailedException("Couldn't find stream information.");
    }

    return this;
}

int FFmpegHelper::findFirstStreamIndexByType(enum AVMediaType avMediaType) {
    for (unsigned int i = 0; i < this->pFormatContext->nb_streams; i++) {
        if (pFormatContext->streams[i]->codecpar->codec_type == avMediaType) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

AVFormatContext *FFmpegHelper::getFormatContext() {
    return pFormatContext;
}

AVCodecContext *FFmpegHelper::openCodec(enum AVMediaType avMediaType) {
    int index = this->findFirstStreamIndexByType(avMediaType);
    AVCodecContext *pCodecCtx = this->pFormatContext->streams[index]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == nullptr) {
        throw FFmpegFailedException("Codec not found.");
    }
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        throw FFmpegFailedException("Could not open codec.");
    }
    return pCodecCtx;
}

AVFrame *FFmpegHelper::allocAVFrameAndDataBufferWithType(enum AVPixelFormat pix_fmt, int width, int height, int align) {
    AVFrame *pFrame = av_frame_alloc();
    u_char *outBuffer = nullptr;
    outBuffer = (u_char *) av_malloc(av_image_get_buffer_size(pix_fmt, width, height, align));
    av_image_fill_arrays(pFrame->data, pFrame->linesize, outBuffer, pix_fmt, width, height, align);
    return pFrame;
}

AVPacket *FFmpegHelper::allocAVPacket() {
    return (AVPacket *) av_malloc(sizeof(AVPacket));
}

SwsContext *FFmpegHelper::SWS_GetContext(AVCodecContext *pCodecContext, enum AVPixelFormat dstFormat, int flags,
                                         SwsFilter *srcFilter, SwsFilter *dstFilter, const double *param) {
    SwsContext *swsContext = nullptr;
    swsContext = sws_getContext(pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt,
                                pCodecContext->width,
                                pCodecContext->height, dstFormat, flags, srcFilter, dstFilter, param);
    return swsContext;
}

FFmpegHelper *FFmpegHelper::decode(AVCodecContext *avCodecContext, AVPacket *packet, AVFrame *pFrame,
                                   const DecodeCallbackFunc& callbackFunc) {
    int ret = avcodec_send_packet(avCodecContext, packet);
    if (ret != 0)
        return this;
    while (avcodec_receive_frame(avCodecContext, pFrame) >= 0) {
        callbackFunc(pFrame);
    }
    av_packet_unref(packet);
    return this;
}



