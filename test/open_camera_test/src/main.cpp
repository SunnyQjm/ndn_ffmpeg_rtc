//
// Created by mingj on 2019/10/16.
//

#include <iostream>
#include <FFmpegHelper.h>
#include <SDL2Helper.h>

using namespace std;
// Refresh Event
#define SFM_REFRESH_EVENT (SDL_USEREVENT + 1)
#define SFM_BREAK_EVENT (SDL_USEREVENT + 2)

int thread_exit = 0;

int sfpRefreshThread(void *opaque) {
    thread_exit = 0;
    while (!thread_exit) {
        SDL_Event event;
        event.type = SFM_REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    thread_exit = 0;
    // Break
    SDL_Event event;
    event.type = SFM_BREAK_EVENT;
    SDL_PushEvent(&event);
    return 0;
}

int main() {
    FFmpegHelper::init();
    FFmpegHelper fFmpegHelper;
    AVCodecContext *pCodecContext = fFmpegHelper.initFormatContext()
            ->openCamera()
            ->openCodec(AVMEDIA_TYPE_VIDEO);
    AVFrame *pFrame, *pFrameYUV;
    pFrame = av_frame_alloc();
    pFrameYUV = FFmpegHelper::allocAVFrameAndDataBufferWithType(AV_PIX_FMT_YUV420P, pCodecContext->width,
                                                                pCodecContext->height);
    int screenW = pCodecContext->width;
    int screenH = pCodecContext->height;
    SDL2Helper sdl2Helper(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    sdl2Helper.createWindow("My Camera Capture test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            screenW, screenH)
            ->createRenderer();

    auto *packet = FFmpegHelper::allocAVPacket();
    SwsContext *imageConvertCtx = FFmpegHelper::SWS_GetContext(pCodecContext, AV_PIX_FMT_YUV420P);
    SDL_Texture *texture = sdl2Helper.createTexture(SDL_PIXELFORMAT_YV12,
                                                    SDL_TEXTUREACCESS_STREAMING, pCodecContext->width,
                                                    pCodecContext->height);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = screenW;
    rect.h = screenH;

    SDL_CreateThread(sfpRefreshThread, nullptr, nullptr);
    SDL_Event event;

    AVFormatContext *pFormatContext = fFmpegHelper.getFormatContext();
    for (;;) {
        // Wait
        SDL_WaitEvent(&event);
        if (event.type == SFM_REFRESH_EVENT) {
            if (av_read_frame(pFormatContext, packet) >= 0) {
                fFmpegHelper.decode(pCodecContext, packet, pFrame,
                                    [=, &sdl2Helper](AVFrame *frame) {
                                        /**
                                         * sws_scale
                                         * https://blog.csdn.net/u010029439/article/details/82859206
                                         */
                                        sws_scale(imageConvertCtx,
                                                  (const unsigned char *const *) frame->data,
                                                  frame->linesize,
                                                  0,
                                                  pCodecContext->height,
                                                  pFrameYUV->data,
                                                  pFrameYUV->linesize);
                                        sdl2Helper.updateYUVTexture(texture, &rect, pFrameYUV);
                                        sdl2Helper.renderClear()
                                                ->renderCopy(texture, nullptr, &rect)
                                                ->renderPresent()
                                                ->delay(40);
                                    });
            } else {
                thread_exit = 1;
            }
        } else if (event.type == SDL_QUIT) {
            thread_exit = 1;
        } else if (event.type == SFM_BREAK_EVENT) {
            break;
        }
    }

    av_free(pFrameYUV);
    sws_freeContext(imageConvertCtx);
    sdl2Helper.quit();
    avcodec_close(pCodecContext);
}