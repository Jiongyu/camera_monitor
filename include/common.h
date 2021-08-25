#pragma once

// log
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


#ifndef SAVE_VIDEO_FIFO_NAME
#define SAVE_VIDEO_FIFO_NAME "/tmp/save_video_fifo_name"
#endif

// 函数返回状态
typedef enum
{
    F_FAILED = -1,
    F_SUCCESS = 0
} E_STATUS;


// 时间滤镜位置
typedef enum
{
    LEFT_UP = 0,
    LEFT_DOWN,
    RIGHT_UP,
    RIGHT_DOWN, 

} E_TIME_POSITION;


// 本地存储时间间隔
typedef enum
{
    T_5MIN = 0,
    T_10MIN,
    T_20MIN,
    T_30MIN,
    T_1HOUR,

} E_TIME_SAVE_INTERVAL;

