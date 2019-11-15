//
// Created by mingj on 2019/10/22.
//

#include <iostream>

#define USE_FFMPEG

#include <SDL2Helper.h>
#include <EasyCamera.h>
#include <EasyVideoEncoder.h>
#include "ndn_rtpp.h"
#include <pthread.h>
#include <EasyMicroPhone.h>

using namespace std;


void *captureAudio(void *args) {
    string prefix = (char *) args;
    prefix += "/audio";
    ndn_rtpp audioProducer(prefix);
    EasyMicroPhone easyMicroPhone;
    easyMicroPhone.openMicrophone()
            ->prepare();
    SDL_Event e;
    easyMicroPhone.begin([=, &audioProducer, &e](AVFrame *frame) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return true;
            }
        }
        audioProducer.sendobj(reinterpret_cast<const char *>(frame->data[0]), frame->pkt_size);
        // 在这边发送音频
        return false;
    });
    SDL_Quit();
}

int main(int argc, char **argv) {

    if (argc != 2) {
        cerr << "usage: ./ndn_transport_source <prefix>" << endl;
        return 1;
    }
    string prefix = argv[1];
    prefix += "/video";
    ndn_rtpp myproducer(prefix);

//    // 创建捕获音频的线程
//    pthread_t id;
//    pthread_create(&id, nullptr, captureAudio, (void *) argv[1]);

    cout << "what?" << endl;
    EasyCamera easyCamera;
    easyCamera.openCamera()
            ->prepare();
    AVCodecContext *pCodecContext = easyCamera.getCodecCtx();
    int screenW = pCodecContext->width;
    int screenH = pCodecContext->height;
//    SDL2Helper sdl2Helper(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
//    sdl2Helper.createWindow("My Camera Capture test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//                            screenW, screenH)
//            ->createRenderer();
//
//    SDL_Texture *texture = sdl2Helper.createTexture(SDL_PIXELFORMAT_YV12,
//                                                    SDL_TEXTUREACCESS_STREAMING, screenW, screenH);
//    SDL_Rect rect;
//    rect.x = 0;
//    rect.y = 0;
//    rect.w = screenW;
//    rect.h = screenH;

    EasyVideoEncoder easyEncoder(AV_CODEC_ID_H264);

    CodecContextParam param{};
    param.bit_rate = 400000;
    param.width = pCodecContext->width;
    param.height = pCodecContext->height;
    param.time_base.num = 1;
    param.time_base.den = 25;
    param.gop_size = 10;
    param.max_b_frames = 1;
    param.pix_fmt = AV_PIX_FMT_YUV420P;

    easyEncoder.initCodecParam(param)
            ->prepareEncode();

    SDL_Event e;
    easyCamera.begin([=, &e, &easyEncoder, &myproducer](AVFrame *pFrameYUV) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return true;
            }
        }
//        sdl2Helper.updateYUVTexture(texture, &rect, pFrameYUV);
//        sdl2Helper.renderClear()
//                ->renderCopy(texture, nullptr, &rect)
//                ->renderPresent();
        easyEncoder.encode(pFrameYUV, [=, &myproducer](AVPacket *pkt) {
            // 在这里发送数据
            myproducer.sendobj((const char *) (pkt->data), pkt->size);
        });
        return false;
    });

//    sdl2Helper.quit();
}

