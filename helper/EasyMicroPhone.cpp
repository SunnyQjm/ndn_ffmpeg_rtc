//
// Created by mingj on 2019/10/31.
//

#include "EasyMicroPhone.h"
#include <iostream>

#define MAX_AUDIO_FRAME_SIZE 192000

using namespace std;

void EasyMicroPhone::throwException(const std::string &msg) {
    std::string logMsg = "EasyMicrophone -> " + msg;
    throw FFmpegFailedException(logMsg);
}

EasyMicroPhone::EasyMicroPhone() {
    av_register_all();
    avformat_network_init();
    avdevice_register_all();

    pFormatCtx = avformat_alloc_context();
}

EasyMicroPhone::~EasyMicroPhone() {
    av_free(&pAudioFrameOut->data[0]);
    av_free(pAudioFrameOut);
    av_free(pAudioFrame);
    swr_free(&audioConvertCtx);
    avcodec_close(pCodecCtx);
    avformat_free_context(pFormatCtx);
}

EasyMicroPhone *EasyMicroPhone::openMicrophone(const std::string &av_input_short_name, const std::string &url) {
    // Linux 打开麦克风设备的输入流
    AVInputFormat *avInputFormat = av_find_input_format(av_input_short_name.c_str());
    av_register_input_format(avInputFormat);
    if (avformat_open_input(&pFormatCtx, url.c_str(), avInputFormat, nullptr) != 0) {
        throwException("Couldn't open input stream.");
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        throwException("Couldn't find stream information.");
    }
    return this;
}

EasyMicroPhone *EasyMicroPhone::prepare() {
    audioIndex = EasyFFmpeg::FFmpegUtil::findFirstStreamIndexByType(this->pFormatCtx, AVMEDIA_TYPE_AUDIO);
    pCodecCtx = this->pFormatCtx->streams[audioIndex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == nullptr) {
        throwException("Codec not found.");
    }
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        throwException("Could not open codec.");
    }


    uint64_t out_channel_layout = AV_CH_LAYOUT_MONO;
    int out_nb_samples = 1024;
    int out_sample_rate = 48000;
    auto out_sample_format = AV_SAMPLE_FMT_FLT;
    int out_nb_channels = av_get_channel_layout_nb_channels(out_channel_layout);

    pAudioFrame = av_frame_alloc();
    pAudioFrameOut = EasyFFmpeg::FFmpegUtil::allocAVFrameAndDataBufferWithTyp(out_sample_format, out_nb_channels, out_nb_samples);
    pAudioFrameOut->format = out_sample_format;
    pAudioFrameOut->nb_samples = out_nb_samples;
    packet = av_packet_alloc();

    int in_channel_layout = av_get_default_channel_layout(pCodecCtx->channels);
    printf("audio sample_fmt=%d size=%d channel=%d in_channel_layout=%d sample_rate=%d\n", pCodecCtx->sample_fmt,
           pCodecCtx->frame_size,
           pCodecCtx->channels, in_channel_layout, pCodecCtx->sample_rate);

    audioConvertCtx = swr_alloc();
    if (audioConvertCtx == nullptr) {
        throwException("Could not allocate SwrContext\n");
    }

    if (swr_alloc_set_opts(audioConvertCtx, out_channel_layout, out_sample_format,
                           out_sample_rate, in_channel_layout,
                           pCodecCtx->sample_fmt, pCodecCtx->sample_rate,
                           0, nullptr) == nullptr) {
        throwException("Could not swr_alloc_set_opts\n");
    }
    if (swr_init(audioConvertCtx) < 0) {
        throwException("Failed to initialize the reSampling content\n");
    }
    return this;
}

EasyMicroPhone *EasyMicroPhone::begin(const EasyMicroPhone::MicrophoneCallbackFunc &callback) {
    bool exit = false;
    while (!exit) {
        if (av_read_frame(pFormatCtx, packet) >= 0) {
            if (packet->stream_index == audioIndex) {
                EasyFFmpeg::FFmpegUtil::decode(pCodecCtx, packet, pAudioFrame, [=, &exit](AVFrame *frame) {
//                    swr_convert(audioConvertCtx, pAudioFrameOut->data, MAX_AUDIO_FRAME_SIZE, (const uint8_t **) frame->data,
//                                frame->nb_samples);
//                    pAudioFrameOut->pkt_size = frame->pkt_size;
                    exit = callback(frame);
                });
            }
        }
    }
    return this;
}
