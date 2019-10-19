//
// Created by mingj on 2019/10/16.
//

#define USE_FFMPEG

#include <SDL2Helper.h>
#include <EasyCamera.h>

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

    SDL_Event e;
    easyCamera.begin([=, &sdl2Helper, &e](AVFrame *pFrameYUV) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                cout << "fuck" << endl;
                return true;
            }
        }
        sdl2Helper.updateYUVTexture(texture, &rect, pFrameYUV);
        sdl2Helper.renderClear()
                ->renderCopy(texture, nullptr, &rect)
                ->renderPresent();
        return false;
    });

}