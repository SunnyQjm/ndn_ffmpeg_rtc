//
// Created by mingj on 2019/10/19.
//
#include <iostream>
#include <FFmpegHelper.h>
#include <SDL2Helper.h>
#include <EasyEncoder.h>
#include <EasyDecoder.h>
#include <chrono>

using namespace std;

time_t getTimeStamp() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
    return timestamp;

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

    AVFormatContext *pFormatContext = fFmpegHelper.getFormatContext();

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
    int i = 0;
    int firstFrameNum = 0;
    int encodedPacketNum = 0;
    int secondFrameNum = 0;
    for (i = 0; i < 1000; i++) {
        cout << getTimeStamp() << " -> ";
//        cout << getTimeStamp() << "(before read frame) -> ";
        if (av_read_frame(pFormatContext, packet) >= 0) {
            cout << getTimeStamp() << endl;
//            cout << getTimeStamp() << "(after read frame) -> ";
            fFmpegHelper.decode(pCodecContext, packet, pFrame,
                                [=, &sdl2Helper, &easyEncoder, &easyDecoder, &firstFrameNum, &encodedPacketNum, &secondFrameNum](
                                        AVFrame *frame) {
//                                    cout << getTimeStamp() << "(after first decode) -> ";
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
//                                    cout << getTimeStamp() << "(after scale) -> ";
                                    firstFrameNum++;
//                                    cout << frame->pkt_size << " -> ";
                                    easyEncoder.encode(pFrameYUV,
                                                       [=, &easyDecoder, &sdl2Helper, &encodedPacketNum, &secondFrameNum](
                                                               AVPacket *pkt) {
//                                                           cout << pkt->size << endl;
//                                                           cout << getTimeStamp() << "(after encode) -> ";
                                                           encodedPacketNum++;
                                                           easyDecoder.decode(pkt, [=, &sdl2Helper, &secondFrameNum](
                                                                   AVFrame *frame1) {
                                                               secondFrameNum++;
                                                               sdl2Helper.updateYUVTexture(texture, &rect, frame1);
                                                               sdl2Helper.renderClear()
                                                                       ->renderCopy(texture, nullptr, &rect)
                                                                       ->renderPresent();
                                                           });
//                                                           cout << getTimeStamp() << "(after decode) -> ";
                                                       });

                                });
        }
//        cout << getTimeStamp() << "(finish one iterator)" << endl;

//        cout << firstFrameNum << " -> " << encodedPacketNum << " -> " << secondFrameNum << endl;
    }
    av_free(pFrameYUV);
    sws_freeContext(imageConvertCtx);
    sdl2Helper.quit();
    avcodec_close(pCodecContext);
}

