//
// Created by mingj on 2019/10/18.
//

#include <iostream>
#include <FFmpegHelper.h>
#include <EasyEncoder.h>

using namespace std;

int main() {
    FFmpegHelper::init();
    FILE *fp_out;
    FFmpegHelper fFmpegHelper;
    AVCodecContext *pCodecContext = fFmpegHelper.initFormatContext()
            ->openCamera()
            ->openCodec(AVMEDIA_TYPE_VIDEO);
    AVFrame *pFrame, *pFrameYUV;
    pFrame = av_frame_alloc();
    pFrameYUV = FFmpegHelper::allocAVFrameAndDataBufferWithType(AV_PIX_FMT_YUV420P, pCodecContext->width,
                                                                pCodecContext->height);
    auto *packet = FFmpegHelper::allocAVPacket();
    SwsContext *imageConvertCtx = FFmpegHelper::SWS_GetContext(pCodecContext, AV_PIX_FMT_YUV420P);
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
    int i = 0;
    pFrameYUV->format = AV_PIX_FMT_YUV420P;
    pFrameYUV->width = pCodecContext->width;
    pFrameYUV->height = pCodecContext->height;
    string fileNameOut = "ds.h264";
    fp_out = fopen(fileNameOut.c_str(), "wb");
    if (!fp_out) {
        printf("Could not open %s\n", fileNameOut.c_str());
        return -1;
    }
    for (i = 0; i < 1000;) {
        // Wait
        if (av_read_frame(pFormatContext, packet) >= 0) {
            i++;
            fFmpegHelper.decode(pCodecContext, packet, pFrame,
                                [=, &easyEncoder](AVFrame *frame) {
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
                                    easyEncoder.encode(pFrameYUV, [=](AVPacket *pkt) {
                                        fwrite(pkt->data, 1, pkt->size, fp_out);
                                        cout << pkt->size << endl;
                                    });
                                });
        }
    }
    fclose(fp_out);
    av_free(pFrameYUV);
    sws_freeContext(imageConvertCtx);
    avcodec_close(pCodecContext);
}