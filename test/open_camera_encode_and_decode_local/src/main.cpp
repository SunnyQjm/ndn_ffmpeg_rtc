//
// Created by mingj on 2019/10/19.
//
#include <iostream>
#define USE_FFMPEG
#include <SDL2Helper.h>
#include <EasyCamera.h>
#include <EasyEncoder.h>
#include <EasyDecoder.h>

using namespace std;


int main() {
    EasyCamera easyCamera;
    easyCamera.openCamera()
            ->prepare();
    AVCodecContext *pCodecContext = easyCamera.getCodecCtx();
    int screenW = pCodecContext->width;
    int screenH = pCodecContext->height;
    SDL2Helper sdl2Helper(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    sdl2Helper.createWindow("My Camera Capture test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            screenW, screenH)
            ->createRenderer();

    SDL_Texture *texture = sdl2Helper.createTexture(SDL_PIXELFORMAT_YV12,
                                                    SDL_TEXTUREACCESS_STREAMING, pCodecContext->width,
                                                    pCodecContext->height);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = screenW;
    rect.h = screenH;

    EasyEncoder easyEncoder(AV_CODEC_ID_H264);

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
    EasyDecoder easyDecoder(AV_CODEC_ID_H264);
    easyDecoder.prepareDecode();
    SDL_Event e;
    easyCamera.begin([=, &sdl2Helper, &e, &easyEncoder, &easyDecoder](AVFrame *pFrameYUV) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return true;
            }
        }
        easyEncoder.encode(pFrameYUV,
                           [=, &easyDecoder, &sdl2Helper](
                                   AVPacket *pkt) {
                               easyDecoder.decode(pkt, [=, &sdl2Helper](
                                       AVFrame *frame1) {
                                   sdl2Helper.updateYUVTexture(texture, &rect, frame1);
                                   sdl2Helper.renderClear()
                                           ->renderCopy(texture, nullptr, &rect)
                                           ->renderPresent();
                               });
                           });
        return false;
    });
    sdl2Helper.quit();
}

