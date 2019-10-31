//
// Created by mingj on 2019/10/31.
//

#include <iostream>
#include <EasyMicroPhone.h>
using namespace std;

int main() {
    int num = 2000;
    FILE *fp_pcm = fopen("output.pcm", "wb+");
    EasyMicroPhone easyMicroPhone;
    easyMicroPhone.openMicrophone()
            ->prepare()
            ->begin([=, &num](AVFrame *frame) {
                fwrite(frame->data, 1, frame->pkt_size, fp_pcm);
                bool exit = (num--) <= 0;
                cout << "exit: " << exit << endl;
                return exit;
            });
    fclose(fp_pcm);
}