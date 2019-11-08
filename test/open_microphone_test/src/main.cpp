//
// Created by mingj on 2019/10/31.
//

#include <iostream>
#include <EasyMicroPhone.h>
#include <EasyAudioEncoder.h>
#include <SDL2Helper.h>

using namespace std;

int main() {

    int num = 5000;
    FILE *fp_pcm = fopen("output.pcm", "wb+");
    EasyMicroPhone easyMicroPhone;
    easyMicroPhone.openMicrophone()
            ->prepare();

    EasyAudioEncoder easyAudioEncoder(AV_CODEC_ID_MP3);
    EasyAudioEncoder::CodecContextParam codecContextParam{};
    codecContextParam.sampleFormat = AV_SAMPLE_FMT_S16;
    codecContextParam.bitRate = 64000;
    codecContextParam.sampleRate = 48000;
    codecContextParam.channelLayout = AV_CH_LAYOUT_MONO;
    easyAudioEncoder.initCodecParam(codecContextParam)
            ->prepareEncode();


    easyMicroPhone.begin([=, &num, &easyAudioEncoder](AVFrame *frame) {
        // 在这里发送
        fwrite(frame->data[0], 1, frame->pkt_size, fp_pcm);
//        while(audio_len>0)//Wait until finish
//            SDL_Delay(1);
//        frame->data[0]
//        easyAudioEncoder.encode(frame, [=](AVPacket *pkt) {
//            cout << "callback out: " << pkt->size << endl;
//            fwrite(pkt->data, 1, pkt->size, fp_pcm);
//        });

        bool exit = (num--) <= 0;
        return exit;
    });
    fclose(fp_pcm);
    SDL_Quit();
}