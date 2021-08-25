#pragma once

extern "C"
{
// ffmpeg
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
}

#include "common.h"

class Ffmpeg_filter
{
private:
    /* data */
public:
    Ffmpeg_filter(/* args */);
    ~Ffmpeg_filter();

    E_STATUS addTimeFilter(E_TIME_POSITION position);
    
};
