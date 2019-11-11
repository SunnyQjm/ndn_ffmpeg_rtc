//
// Created by mingj on 19-10-12.
//

#include "SDL2Helper.h"

SDL2Helper *SDL2Helper::logSDLError(std::ostream &os, const std::string &msg) {
    os << msg << "\nerror: " << SDL_GetError() << std::endl;
    return this;
}

SDL2Helper::SDL2Helper(Uint32 flags) {
    if (SDL_Init(flags) != 0) {
        logSDLError(std::cerr, "SDL_Init");
    } else {
        // initial SDL2 library success
        isSDL2Init = true;
    };

    if ((flags & SDL_INIT_AUDIO) != 0) {             // 开启了音频支持就初始化音频相关的操作
        // 默认的callback
//        this->audioSpec.callback = [](void *userData, Uint8 *stream, int len) {
//            SDL_memset(stream, 0, len);
//            AudioFrame audioFrame;
//        };
        this->audioSpec.callback = nullptr;
        this->audioQueueMutex = SDL_CreateMutex();
        this->audioQueueCond = SDL_CreateCond();
    }
}


SDL2Helper *SDL2Helper::createWindow(const std::string &title,
                                     int x, int y, int w, int h, Uint32 flags) {
    window = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    if (window == nullptr) {
        this->throwSDLFailedException("CreateWindow");
    }
    return this;
}


SDL2Helper *SDL2Helper::createWindowFrom(const void *data) {
    window = SDL_CreateWindowFrom(data);
    if(window == nullptr) {
        this->throwSDLFailedException("CreateWindow");
    }
    return this;
}

SDL2Helper *SDL2Helper::createRenderer(int index, Uint32 flags) {
    if (this->window == nullptr) {
        throw SDLFailedException("Please invoke createWindow to create a SDL_Window object success first!");
    }
    renderer = SDL_CreateRenderer(this->window, index, flags);
    if (renderer == nullptr) {
        this->throwSDLFailedException("CreateRender");
    }
    return this;
}

SDL2Helper *SDL2Helper::quit() {
    SDLCleanUp::cleanup(renderer, window, audioQueueMutex, audioQueueCond);
#ifdef SDL2_HELPER_USE_SDL2_IMAGE
    if (isSDL2ImageInit) {
        IMG_Quit();
        isSDL2ImageInit = false;
    }
#endif
#ifdef SDL2_HELPER_USE_SDL2_TTF
    if (isSDL2TtfInit) {
        TTF_Quit();
        isSDL2TtfInit = false;
    }
#endif
    if (isSDL2Init) {
        SDL_Quit();
        isSDL2Init = false;
    }
    return this;
}

SDL_Window *SDL2Helper::getWindow() {
    return this->window;
}

SDL_Renderer *SDL2Helper::getRenderer() {
    return this->renderer;
}

SDL_Surface *SDL2Helper::loadBMP(const std::string &filePath) {
    SDL_Surface *bmp = SDL_LoadBMP(filePath.c_str());
    if (bmp == nullptr) {
        this->throwSDLFailedException("SDL_LoadBMP -> " + filePath);
    }
    return bmp;
}

SDL_Texture *SDL2Helper::createTextureFromSurface(SDL_Surface *surface) {
    SDL_Texture *tex = SDL_CreateTextureFromSurface(this->renderer, surface);
    if (tex == nullptr) {
        this->throwSDLFailedException("SDL_CreateTextFromSurface");
    }
    return tex;
}

SDL2Helper *SDL2Helper::renderClear() {
    SDL_RenderClear(this->renderer);
    return this;
}

SDL2Helper *SDL2Helper::renderCopy(SDL_Texture *texture, const SDL_Rect *srcRect, const SDL_Rect *dstRect) {
    SDL_RenderCopy(this->renderer, texture, srcRect, dstRect);
    return this;
}

SDL2Helper *SDL2Helper::renderPresent() {
    SDL_RenderPresent(this->renderer);
    return this;
}

SDL2Helper *SDL2Helper::delay(Uint32 ms) {
    SDL_Delay(ms);
    return this;
}

SDL_Texture *SDL2Helper::loadTextureFromBMP(const std::string &filePth) {
    SDL_Texture *texture = nullptr;
    SDL_Surface *image = this->loadBMP(filePth);
    texture = this->createTextureFromSurface(image);
    return texture;
}

SDL2Helper *SDL2Helper::renderTexture(SDL_Texture *texture, int x, int y, int w, int h) {
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    this->renderCopy(texture, nullptr, &dst);
    return this;
}

SDL2Helper *SDL2Helper::renderTexture(SDL_Texture *texture, int x, int y) {
    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    return renderTexture(texture, x, y, w, h);
}

void SDL2Helper::throwSDLFailedException(const std::string &what) {
    std::string what_ = what + " -> " + SDL_GetError();
    throw SDLFailedException(what_);
}

SDL_Texture *SDL2Helper::createTexture(Uint32 format, int access, int w, int h) {
    SDL_Texture *texture = nullptr;
    texture = SDL_CreateTexture(this->renderer, format, access, w, h);
    return texture;
}

SDL2Helper *SDL2Helper::resize(int width, int height) {
    SDL_SetWindowSize(window, width, height);
    return this;
}

SDL2Helper *SDL2Helper::setAudioSpecFreq(int freq) {
    this->audioSpec.freq = freq;
    return this;
}

SDL2Helper *SDL2Helper::setAudioSpecFormat(SDL_AudioFormat format) {
    this->audioSpec.format = format;
    return this;
}

SDL2Helper *SDL2Helper::setAudioSpecChannels(Uint8 channels) {
    this->audioSpec.channels = channels;
    return this;
}

SDL2Helper *SDL2Helper::setAudioSpecSilence(Uint8 silence) {
    this->audioSpec.silence = silence;
    return this;
}

SDL2Helper *SDL2Helper::setAudioSpecSamples(Uint16 samples) {
    this->audioSpec.samples = samples;
    return this;
}

SDL2Helper *SDL2Helper::setAudioSpecPadding(Uint16 padding) {
    this->audioSpec.padding = padding;
    return this;
}

SDL2Helper *SDL2Helper::setAudioSpecSize(Uint32 size) {
    this->audioSpec.size = size;
    return this;
}

SDL2Helper *SDL2Helper::setAudioSpecCallback(SDL_AudioCallback callback) {
    this->audioSpec.callback = callback;
    return this;
}


SDL2Helper *SDL2Helper::putAudioData(Uint8 *data, int size) {
    if (size > 0) {
        AudioFrame audioFrame{};
        audioFrame.data = static_cast<Uint8 *>(malloc(size));
        audioFrame.size = size;
        memcpy(audioFrame.data, data, size);
        SDL_LockMutex(audioQueueMutex);
        audioQueue.push(audioFrame);
        SDL_CondSignal(audioQueueCond);
        SDL_UnlockMutex(audioQueueMutex);
    }
    return this;
}

SDL2Helper *SDL2Helper::getAudioFrame(AudioFrame *frame) {
    SDL_LockMutex(audioQueueMutex);
    if (audioQueue.empty()) {
        SDL_CondWait(audioQueueCond, audioQueueMutex);
    }
    auto audioFrame = audioQueue.front();
    frame->data = audioFrame.data;
    frame->size = audioFrame.size;
    SDL_UnlockMutex(audioQueueMutex);
    return this;
}


SDL2Helper *SDL2Helper::openAudio() {
    if (SDL_OpenAudio(&audioSpec, nullptr) < 0) {
        throwSDLFailedException("Open audio failed");
    }
    return this;
}

SDL2Helper *SDL2Helper::feedPCM(const void *data, Uint32 len) {
    // 使用SDL_OpenAudio方式打开，devId为1
    SDL_QueueAudio(1, data, len);
    return this;
}

SDL2Helper *SDL2Helper::pauseAudio(int onPause) {
    SDL_PauseAudio(onPause);
    return this;
}


















