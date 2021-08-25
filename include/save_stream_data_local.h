#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <memory>

extern "C"
{
// ffmpeg
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "common.h"
#include "timer.h"

class Save_stream_data_local
{

public:
    Save_stream_data_local(/* args */);
    ~Save_stream_data_local(){};

    E_STATUS saveFileInit();
    E_STATUS saveFileDinit();

    E_STATUS setSaveDataTimeInterval(E_TIME_SAVE_INTERVAL interval);

    E_STATUS saveLocalData(const AVPacket* pkg);

private:
    static void* saveVideoDataFunc(void* param);

     
private:
    E_TIME_SAVE_INTERVAL m_default_save_data_interval;
    static std::shared_ptr<std::ofstream> m_output_file;
    std::string m_file_name;
    Timer m_timer;

};
