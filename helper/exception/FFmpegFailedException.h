//
// Created by mingj on 2019/10/18.
//

#ifndef NDN_FFMPEG_RTC_FFMPEGFAILEDEXCEPTION_H
#define NDN_FFMPEG_RTC_FFMPEGFAILEDEXCEPTION_H


class FFmpegFailedException : public std::runtime_error {
public:
    FFmpegFailedException(const std::string &what) : std::runtime_error(what) {

    }
};



#endif //NDN_FFMPEG_RTC_FFMPEGFAILEDEXCEPTION_H
