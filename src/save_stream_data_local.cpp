#include "save_stream_data_local.h"


std::shared_ptr<std::ofstream> Save_stream_data_local::m_output_file;

Save_stream_data_local::Save_stream_data_local(/* args */)
{
    m_default_save_data_interval = T_10MIN;
}

E_STATUS Save_stream_data_local::saveFileInit()
{
    std::stringstream ss;
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S.mp4");
    m_file_name = ss.str();
    SPDLOG_INFO("fileName: {}", m_file_name);

    m_output_file = std::make_shared<std::ofstream>(m_file_name, std::ios::out | std::ios::binary);

    int timer_interval = 0;
    switch (m_default_save_data_interval)
    {
    case T_5MIN:
        timer_interval = 300;
        break;
    case T_10MIN:
        timer_interval = 600;
        break;
    case T_30MIN:
        timer_interval = 1800;
        break;
    case T_1HOUR:
        timer_interval = 3600;
        break;
    default:
        timer_interval = 300;
        SPDLOG_WARN("default save video time interal: T_5MIN"); 
        break;
    }

    pthread_t test;
    pthread_create(&test, 0, saveVideoDataFunc, NULL);
}

E_STATUS Save_stream_data_local::saveFileDinit()
{
    if(m_output_file->is_open())
    {
        SPDLOG_INFO("saveFileDinit");
        m_output_file->flush();
        m_output_file->close();
    }
    
}

E_STATUS Save_stream_data_local::setSaveDataTimeInterval(E_TIME_SAVE_INTERVAL interval)
{
    m_default_save_data_interval = interval;
}

#if 0
E_STATUS Save_stream_data_local::saveLocalData(const AVPacket* pkg)
{
    if(nullptr == pkg)
    {
        SPDLOG_WARN("saveLocalData input param is NULL");
        return F_FAILED;
    }
    
    m_output_file->write((char *)(pkg->data), pkg->size);
    m_output_file->flush();
}
#endif

void* Save_stream_data_local::saveVideoDataFunc(void* param)
{
    SPDLOG_WARN("saveVideoDataFunc");
    int save_video_fifo_name_fd = 0;
    if (access(SAVE_VIDEO_FIFO_NAME, F_OK) == -1) {
        int res = mkfifo(SAVE_VIDEO_FIFO_NAME, 0777);
        if (res != 0) {
            SPDLOG_WARN("mkfifo error {}", SAVE_VIDEO_FIFO_NAME); 
            return NULL;
        }
    }
    char buffer[4096];
    AVPacket *p =(AVPacket *)buffer;
    SPDLOG_WARN("saveVideoDataFunc");
    save_video_fifo_name_fd = open(SAVE_VIDEO_FIFO_NAME, O_RDONLY);
    if( -1 == save_video_fifo_name_fd)
    {
        SPDLOG_WARN("saveVideoDataFunc");
        SPDLOG_WARN("can not open {}", SAVE_VIDEO_FIFO_NAME); 
        return NULL;
    }
    SPDLOG_WARN("saveVideoDataFunc");
    int res = 0;
    do{
        SPDLOG_WARN("{}", SAVE_VIDEO_FIFO_NAME); 
        res = read(save_video_fifo_name_fd, buffer, sizeof(buffer));
        m_output_file->write((char*)(p->data), p->size);
        m_output_file->flush();

    }while(1);

}