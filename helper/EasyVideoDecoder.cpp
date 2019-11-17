//
// Created by mingj on 2019/10/18.
//

#include "EasyVideoDecoder.h"
#include <iostream>

using namespace std;

void EasyVideoDecoder::throwException(const std::string &msg) {
    std::string logMsg = "EasyVideoDecoder -> " + msg;
    throw FFmpegFailedException(logMsg);
}

EasyVideoDecoder::EasyVideoDecoder(enum AVCodecID codecId) : codecId(codecId) {
}

EasyVideoDecoder *EasyVideoDecoder::prepareDecode() {
    // 找到对应格式的解码器
    pCodec = avcodec_find_decoder(codecId);

    if (!pCodec) {
        throwException("Could not find decoder");
    }
    avcodec_register(pCodec);


    // 创建编解码上下文
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx) {
        throwException("Could not allocate video codec context");
    }

    // 初始化视频流解析上下文
    pCodecParserCtx = av_parser_init(codecId);
    if (!pCodecParserCtx) {
        throwException("Could not allocate video parser context");
    }

    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        throwException("Could not open codec");
    }

    pFrame = av_frame_alloc();
    av_init_packet(&pkt);
    return this;
}

EasyVideoDecoder *EasyVideoDecoder::decode(AVPacket *packet, const EasyVideoDecoder::DecodeCallbackFunc &callback) {
    int ret = avcodec_send_packet(pCodecCtx, packet);
    if (ret != 0)
        return this;
    while (avcodec_receive_frame(pCodecCtx, pFrame) >= 0) {
        callback(pFrame);
    }
    av_packet_unref(packet);
    return this;
}

EasyVideoDecoder::~EasyVideoDecoder() {
    av_parser_close(pCodecParserCtx);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    av_free(pCodecCtx);
}

AVPacket *EasyVideoDecoder::parse(const uint8_t *buf, size_t size) {
    while (size > 0 || pkt.size == 0) {
        int len = av_parser_parse2(pCodecParserCtx, pCodecCtx, &pkt.data, &pkt.size, buf, size, AV_NOPTS_VALUE,
                                   AV_NOPTS_VALUE, AV_NOPTS_VALUE);
        buf += len;
        cout << size << " -> " << len <<  " -> " << pkt.size << endl;
        size -= len;
    }
    if (pkt.size == 0)
        return nullptr;
    return &pkt;
}

/**
 while (size > 0) {
        int len = av_parser_parse2(pCodecParserCtx, pCodecCtx, &pkt.data, &pkt.size, buf, size, AV_NOPTS_VALUE,
                                   AV_NOPTS_VALUE, AV_NOPTS_VALUE);
        buf += len;
        size -= len;
        if (pkt.size != 0)
            break;
    }
    if (pkt.size == 0)
        return nullptr;
    return &pkt;
 */
