//
// Created by mingj on 2019/10/31.
//

#include "EasyAudioEncoder.h"
#include "FFmpegFailedException.h"
#include <iostream>

using namespace std;

void EasyAudioEncoder::throwException(const std::string &msg) {
    std::string logMsg = "EasyAudioEncoder -> " + msg;
    throw FFmpegFailedException(logMsg);
}

EasyAudioEncoder::EasyAudioEncoder(enum AVCodecID codecId) : codecId(codecId) {
    pCodec = avcodec_find_encoder(codecId);
    avcodec_register(pCodec);
    if (!pCodec) {
        throwException("Could not find encoder");
    }
    pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext) {
        throwException("Could not allocate video codec context");
    }
}

EasyAudioEncoder *EasyAudioEncoder::initCodecParam(const EasyAudioEncoder::CodecContextParam &param) {
    pCodecContext->codec_id = codecId;
    pCodecContext->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecContext->sample_fmt = param.sampleFormat;
    pCodecContext->sample_rate = param.sampleRate;
    pCodecContext->channel_layout = param.channelLayout;
    pCodecContext->channels = av_get_channel_layout_nb_channels(pCodecContext->channel_layout);
    pCodecContext->bit_rate = param.bitRate;
    return this;
}

EasyAudioEncoder *EasyAudioEncoder::prepareEncode() {
    // 打开编码器
    if (avcodec_open2(pCodecContext, pCodec, nullptr) < 0) {
        throwException("Could not open codec");
    }
    return this;
}

EasyAudioEncoder *EasyAudioEncoder::encode(AVFrame *pFrame, const EasyAudioEncoder::EncodeCallbackFunc &callback) {
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;
    int ret = avcodec_send_frame(pCodecContext, pFrame);
    cout << "send: " << ret << endl;
    if (ret != 0) {
        return this;
    }
    while ((ret = avcodec_receive_packet(pCodecContext, &pkt)) >= 0) {
        cout << "callback1: " << pkt.size << endl;
        callback(&pkt);
        cout << "callback2: " << pkt.size << endl;
    }
    cout << "receive: " << ret << endl;
    av_packet_unref(&pkt);
    return this;
}

EasyAudioEncoder::~EasyAudioEncoder() {
    avcodec_close(pCodecContext);
    av_free(pCodecContext);
}
