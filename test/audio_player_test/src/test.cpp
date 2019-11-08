#include <SDL2Helper.h>
#include <iostream>

using namespace std;

int main() {
    SDL2Helper sdl2Helper(SDL_INIT_AUDIO | SDL_INIT_TIMER);
    sdl2Helper.setAudioSpecFreq(48000)
            ->setAudioSpecFormat(AUDIO_S16SYS)
            ->setAudioSpecChannels(2)
            ->setAudioSpecSilence(0)
            ->setAudioSpecSamples(1024)
            ->openAudio();
    FILE *fp = fopen("output.pcm", "rb+");
    if (fp == NULL) {
        printf("cannot open this file\n");
        return -1;
    }
    int pcm_buffer_size = 4096;
    char *pcm_buffer = (char *) malloc(pcm_buffer_size);
    sdl2Helper.pauseAudio(0);

    size_t size = 0;
    while ((size = fread(pcm_buffer, 1, pcm_buffer_size, fp)) > 0) {
        sdl2Helper.feedPCM(pcm_buffer, size);
        cout << size << endl;
    }
    SDL_Delay(10000);
}