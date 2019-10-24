//
// Created by mingj on 19-10-12.
//

#ifndef SDL_TOURIALS_SDL2HELPER_H
#define SDL_TOURIALS_SDL2HELPER_H
//#define SDL2_HELPER_USE_SDL2_IMAGE
//#define SDL2_HELPER_USE_SDL2_TTF

#include <iostream>
#include <SDL2/SDL.h>
#include <utility>

// Judge if use SDL2_image extension library
#ifdef SDL2_HELPER_USE_SDL2_IMAGE

#include <SDL2/SDL_image.h>

#endif

// Judge if use SDL2_ttf extension library
#ifdef SDL2_HELPER_USE_SDL2_TTF
#include <SDL2/SDL_ttf.h>
#endif

#ifdef USE_FFMPEG
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}
#endif

namespace SDLCleanUp {
    template<typename T, typename... Args>
    void cleanup(T *t, Args &&... args) {
        cleanup(t);
        cleanup(std::forward<Args>(args)...);
    }


    template<>
    inline void cleanup<SDL_Window>(SDL_Window *win) {
        if (!win)
            return;
        SDL_DestroyWindow(win);
    }


    template<>
    inline void cleanup<SDL_Renderer>(SDL_Renderer *ren) {
        if (!ren) {
            return;
        }
        SDL_DestroyRenderer(ren);
    }

    template<>
    inline void cleanup<SDL_Texture>(SDL_Texture *tex) {
        if (!tex) {
            return;
        }
        SDL_DestroyTexture(tex);
    }

    template<>
    inline void cleanup<SDL_Surface>(SDL_Surface *surf) {
        if (!surf) {
            return;
        }
        SDL_FreeSurface(surf);
    }
}

class SDL2Helper {
    class SDLFailedException : public std::runtime_error {
    public:
        SDLFailedException(const std::string &what) : std::runtime_error(what) {

        }
    };

private:
    bool isSDL2Init = false;
    bool isSDL2ImageInit = false;
    bool isSDL2TtfInit = false;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

public:

    /**
     * Create SDL2Helper Object and init SDL2 library
     * @param flags is the param of SDL2_Init
     */
    SDL2Helper(Uint32 flags);

    /**
     * The Wrapper of SDL_CreateWindow
     * @param win           if create success, win will indicate the window created now
     * the other param is same as SDL_CreateWindow
     * @return
     */
    SDL2Helper *
    createWindow(
            const std::string &title, int x = SDL_WINDOWPOS_CENTERED, int y = SDL_WINDOWPOS_CENTERED,
            int w = 640, int h = 480, Uint32 flags = SDL_WINDOW_SHOWN
    ) noexcept(false);

    SDL2Helper *
    createRenderer(int index = -1, Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) noexcept(false);


    ///////////////////////////////////////////////////////////
    ///////// Getter
    ///////////////////////////////////////////////////////////
    SDL_Window *getWindow();

    SDL_Renderer *getRenderer();


    ///////////////////////////////////////////////////////////
    ///////// SDL2_image's wrapper functions
    ///////////////////////////////////////////////////////////
#ifdef SDL2_HELPER_USE_SDL2_IMAGE

    /**
     * init SDL2_Image library
     * @param flags see IMG_InitFlags
     * @return
     */
    SDL2Helper *initSDL2Image(int flags);
    SDL_Surface *loadUseSDL2Image(const std::string &path);
    SDL_Texture *loadTextureUseSDL2Image(const std::string &path);
    SDL_Surface *loadFromMemUseSDL2Image(void *buffer, int size, int freesrc = 1);
    SDL_Texture *loadTextureFromMemUseSDL2Image(void *buffer, int size, int freesrc = 1);

#endif

    //////////////////////////////////////////////////////////
    //////// SDL2's wrapper functions
    //////////////////////////////////////////////////////////
    SDL_Surface *loadBMP(const std::string &filePath) noexcept(false);

    SDL_Texture *createTexture(Uint32 format, int access, int w, int h);

    SDL_Texture *createTextureFromSurface(SDL_Surface *surface) noexcept(false);

    SDL2Helper *renderClear();

    SDL2Helper *renderCopy(SDL_Texture *texture, const SDL_Rect *srcRect = nullptr, const SDL_Rect *dstRect = nullptr);

    SDL2Helper *renderPresent();

    SDL_Texture *loadTextureFromBMP(const std::string &filePth) noexcept(false);

    SDL2Helper *renderTexture(SDL_Texture *texture, int x, int y, int w, int h);

    SDL2Helper *renderTexture(SDL_Texture *texture, int x, int y);

    SDL2Helper *resize(int width, int height);

    ///////////////////////////////////////////////////////////
    /////// SDL + FFmpeg wrapper functions
    ///////////////////////////////////////////////////////////
#ifdef USE_FFMPEG
    SDL2Helper *updateYUVTexture(SDL_Texture *texture, const SDL_Rect *rect, const AVFrame *pFrame);
#endif


    //////////////////////////////////////////////////////////
    ///////// Utils
    //////////////////////////////////////////////////////////
    SDL2Helper *logSDLError(std::ostream &os, const std::string &msg);

    SDL2Helper *quit();

    void throwSDLFailedException(const std::string &what);

    SDL2Helper *delay(Uint32 ms);

    /**
     * 析构时释放所有的资源
     */
    ~SDL2Helper() {
        // Destroy
        this->quit();
    }
};

#ifdef USE_FFMPEG
SDL2Helper *SDL2Helper::updateYUVTexture(SDL_Texture *texture, const SDL_Rect *rect, const AVFrame *pFrame) {
    SDL_UpdateYUVTexture(texture, rect, pFrame->data[0], pFrame->linesize[0], pFrame->data[1], pFrame->linesize[1],
                         pFrame->data[2], pFrame->linesize[2]);
    return this;
}
#endif


#ifdef SDL2_HELPER_USE_SDL2_IMAGE


SDL2Helper *SDL2Helper::initSDL2Image(int flags) {
    auto result = IMG_Init(flags);
    if((result & flags) != flags) {
        this->throwSDLFailedException("IMG_Init");
    } else {
        isSDL2ImageInit = true;
    }
    return this;
}

SDL_Texture *SDL2Helper::loadTextureUseSDL2Image(const std::string &path) {
    SDL_Texture *texture = IMG_LoadTexture(this->renderer, path.c_str());
    if(texture == nullptr) {
        this->logSDLError(std::cerr, "Use SDL2_image load texture");
    }
    return texture;
}


SDL_Surface *SDL2Helper::loadUseSDL2Image(const std::string &path) {
    SDL_Surface *surface = IMG_Load(path.c_str());
    if(surface == nullptr) {
        this->logSDLError(std::cerr, "Use SDL2_image load surface");
    }
    return IMG_Load(path.c_str());
}

SDL_Surface *SDL2Helper::loadFromMemUseSDL2Image(void *buffer, int size, int freesrc) {
    return IMG_Load_RW(SDL_RWFromMem(buffer, size), freesrc);
}

SDL_Texture *SDL2Helper::loadTextureFromMemUseSDL2Image(void *buffer, int size, int freesrc) {
    return IMG_LoadTexture_RW(this->renderer, SDL_RWFromMem(buffer, size), freesrc);
}
#endif

#endif //SDL_TOURIALS_SDL2HELPER_H
