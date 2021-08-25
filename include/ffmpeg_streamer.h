#pragma once

// c++
#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <atomic>

extern "C"
{
// ffmpeg
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavutil/time.h>
}

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO


#include "common.h"

class Ffmpeg_streamer
{

public:
    Ffmpeg_streamer(){};
    ~Ffmpeg_streamer();

    E_STATUS FfmpegStreamerInit();
    E_STATUS FfmpegStreamerDinit();

    void setFileLog(const std::string& name);

    void setInputFile(const std::string &input_file);
    void setOutputUrl(const std::string &output_url);

    E_STATUS pushStream();



private:
    void getVideoAudioStreamIndex();
    int getVideoAudioStream();

    int copyStreamParamIntoOut();

    int publishStream();

    int pushStreamThreadFun();

private:
    int getStreamId(int *stream_idx, AVCodecContext **dec_ctx, 
                    AVFormatContext *fmt_ctx, enum AVMediaType type);

private:
    AVOutputFormat* m_out_format;
    AVFormatContext* m_input_format_content;
    AVFormatContext* m_output_format_content;

    AVCodecContext* m_video_code_content;
    AVCodecContext* m_audio_code_content;

    AVStream* m_video_stream;
    AVStream* m_audio_stream;

    std::string m_input_file;
    std::string m_output_url;

    int m_video_index;
    int m_audio_index;

    std::atomic<bool> m_start_stop_push_stream;
};
