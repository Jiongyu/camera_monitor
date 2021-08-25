#include "ffmpeg_streamer.h"
#include <ctime>
#include <csignal>
#include <atomic>

std::shared_ptr<Ffmpeg_streamer> demo = std::make_shared<Ffmpeg_streamer>();
std::atomic<bool> gb_catch_signal(false); 

void signalHandler( int signum )
{
    demo->FfmpegStreamerDinit();
    gb_catch_signal = true;
}

int main(int argc, char const *argv[])
{
    E_STATUS ret = F_SUCCESS;
    signal(SIGINT, signalHandler);  

    // demo->setFileLog("textLog.txt");

    demo->setInputFile("./../video/test.mp4");

    // camera
    
    demo->setOutputUrl("rtmp://192.168.1.8:1935/live");

    ret = demo->FfmpegStreamerInit();
    if(ret != F_SUCCESS)
    {    
        SPDLOG_WARN("FfmpegStreamerInit error!");
        return -1;
    }

    ret = demo->pushStream();
    if(ret != F_SUCCESS)
    {
        SPDLOG_WARN("pushStream error!");
        return -1;
    }

    if(! gb_catch_signal)
    {
        SPDLOG_WARN("FfmpegStreamerDini tgb_catch_signal  start!");
        ret = demo->FfmpegStreamerDinit();
        if(ret != F_SUCCESS)
        {
            SPDLOG_WARN("FfmpegStreamerDinit error!");
            return -1;
        }
    }

    SPDLOG_WARN("test end!");

    return 0;
}
