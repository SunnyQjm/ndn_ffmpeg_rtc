//
// Created by mingj on 2019/10/22.
//

#include <iostream>
#define USE_FFMPEG
#include <SDL2Helper.h>
#include <EasyDecoder.h>
#include "ndn_rtpc.h"

using namespace std;
int main(int argc, char** argv) {
    if(argc != 2) {
        cerr << "usage: ./ndn_transport_source <prefix>" << endl;
        return 1;
    }
	ndn_rtpc myconsumer(argv[1]);
    av_register_all();
    avformat_network_init();
    avdevice_register_all();
	char buff[200000] ;
	int recvLen ;

    int screenW = 640;
    int screenH = 480;
    SDL2Helper sdl2Helper(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    sdl2Helper.createWindow("My Camera Capture test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            screenW, screenH)
            ->createRenderer();

    SDL_Texture *texture = sdl2Helper.createTexture(SDL_PIXELFORMAT_YV12,
                                                    SDL_TEXTUREACCESS_STREAMING, screenW,
                                                    screenH);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = screenW;
    rect.h = screenH;
    EasyDecoder easyDecoder(AV_CODEC_ID_H264);
    easyDecoder.prepareDecode();
    SDL_Event e;
    bool exit = false;
    while(!exit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit = true;
            }
        }

        // 在这里接收数据，并得到Packet
		recvLen = myconsumer.readobj(buff) ;
		std::cout << "recvLen = " << recvLen << std::endl ;
		AVPacket *pkt = easyDecoder.parse((const uint8_t*)buff,recvLen) ;
		if(pkt == nullptr){
		    std::cout << "pkt is nullptr" << std::endl;
            continue;
		}
        easyDecoder.decode(pkt, [=, &sdl2Helper, &screenW, &screenH](
                AVFrame *frame1) {
            cout << "===>" << frame1->width << " -> " << frame1->height << endl;
            if(screenW != frame1->width || screenH != frame1->height) {
                screenW = frame1->width;
                screenH = frame1->height;
                sdl2Helper.resize(screenW, screenH);
            }
            sdl2Helper.updateYUVTexture(texture, &rect, frame1);
            sdl2Helper.renderClear()
                    ->renderCopy(texture, nullptr, &rect)
                    ->renderPresent();
        });
		std::cout << "finish decode" << std::endl;
    }
    std::cout << "exit: " << exit << std::endl;
}
