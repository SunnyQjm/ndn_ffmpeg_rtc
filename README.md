# ndn_ffmpeg_rtc

> ## 依赖安装

```shell script
# 安装ffmpeg相关依赖
sudo apt install libavcodec-dev libavformat-dev libswscale-dev libavdevice-dev libavutil-dev 

# 安装sdl相关依赖
sudo apt install libsdl2-dev libsdl2-image-dev
```

> ## Ubuntu 下安装最新的FFmpeg依赖库

```shell
sudo add-apt-repository ppa:jonathonf/ffmpeg-4
sudo apt-get update
sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libavdevice-dev libavutil-dev
```

> ## 编译运行

```shell script
cmake .
make
```