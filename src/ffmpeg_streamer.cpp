#include "ffmpeg_streamer.h"

Ffmpeg_streamer::~Ffmpeg_streamer()
{
    avformat_close_input(&m_input_format_content);
    /* close output */
    if (m_output_format_content && !(m_output_format_content->flags & AVFMT_NOFILE))
    {
        avio_close(m_output_format_content->pb);
    }
    avformat_free_context(m_output_format_content);
    SPDLOG_INFO("Ffmpeg_streamer finished!");
};

void Ffmpeg_streamer::setFileLog(const std::string& name)
{
    // auto time = system("date");
    auto my_logger = spdlog::basic_logger_mt("Ffmpeg_streamer", "logs/" + name);
    spdlog::set_default_logger(my_logger);
}

void Ffmpeg_streamer::setInputFile(const std::string &input_file)
{
    m_input_file = input_file;
    SPDLOG_INFO("m_input_file : {}", m_input_file);
}

void Ffmpeg_streamer::setOutputUrl(const std::string &output_url)
{
    m_output_url = output_url;
    SPDLOG_INFO("m_output_url : {}", m_output_url);
}

E_STATUS Ffmpeg_streamer::pushStream()
{ 
    int ret = 0;
    ret = publishStream();
    if(ret < 0)
    {
        SPDLOG_WARN("Error occurred when publish stream to output URL");
        return F_FAILED;
    }

    return F_SUCCESS;
}

E_STATUS Ffmpeg_streamer::FfmpegStreamerInit()
{
    SPDLOG_INFO("Ffmpeg_streamer_init");

    m_input_format_content = nullptr;
    m_output_format_content = nullptr;
    m_out_format = nullptr;
    m_video_stream = nullptr;
    m_audio_stream = nullptr;
    m_video_code_content = nullptr;
    m_audio_code_content = nullptr;

    m_start_stop_push_stream = false;

    av_register_all();
    avdevice_register_all();
    avcodec_register_all();
    avformat_network_init();

    int ret = avformat_open_input(&m_input_format_content, m_input_file.c_str(), nullptr, nullptr);
    if(ret < 0)
    {
        SPDLOG_ERROR("Error: Could not open source file {0}", m_input_file.c_str());
        return F_FAILED;
    }

    ret = avformat_find_stream_info(m_input_format_content, nullptr);
    if(ret < 0)
    {
        SPDLOG_ERROR("Error: Could not find stream information");
        return F_FAILED;
    }

    av_dump_format(m_input_format_content, 0, m_input_file.c_str(), 0);

     // getVideoAudioStreamIndex();
    ret = getVideoAudioStream();
    if(ret < 0)
    {
        SPDLOG_ERROR("Error: Could not get  video or audio stream information");
        return F_FAILED;
    }

    ret = avformat_alloc_output_context2(&m_output_format_content, NULL, "flv", m_output_url.c_str());
    if(ret < 0)
    {
        SPDLOG_ERROR("Error: Could not Allocate an AVFormatContext for an output format.");
        return F_FAILED;
    }

    ret = copyStreamParamIntoOut();
    if(ret < 0)
    {
        SPDLOG_ERROR("Error: Could not Copy Input Stream parameter to output.");
        return F_FAILED;
    }

	av_dump_format(m_output_format_content, 0, m_output_url.c_str(), 1);

    return F_SUCCESS;
}

E_STATUS Ffmpeg_streamer::FfmpegStreamerDinit()
{
    m_start_stop_push_stream = true;
}

#if 0
void Ffmpeg_streamer::getVideoAudioStreamIndex()
{
  for(int i = 0; i < m_input_format_content->nb_streams; i++)
    {
        if(m_input_format_content->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            m_video_index = i;
            SPDLOG_INFO("m_video_index : {}", m_video_index);
            continue;
        }
        if(m_input_format_content->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO){
            m_audio_index = i;
            SPDLOG_INFO("m_audio_index : {}", m_audio_index);
            continue;
        }
    }
}
#endif

int Ffmpeg_streamer::getVideoAudioStream()
{
    int ret = 0;
    ret = getStreamId(&m_video_index, &m_video_code_content, m_input_format_content, AVMEDIA_TYPE_VIDEO);
    if(ret < 0)
    {
        return ret;
    }
    SPDLOG_INFO("m_video_index : {}", m_video_index);
    m_video_stream = m_input_format_content->streams[m_video_index];

    ret = getStreamId(&m_audio_index, &m_audio_code_content, m_input_format_content, AVMEDIA_TYPE_AUDIO);
    if(ret < 0)
    {
        return ret;
    }
    SPDLOG_INFO("m_audio_index : {}", m_audio_index);
    m_audio_stream = m_input_format_content->streams[m_audio_index];

    return ret;  
}

int Ffmpeg_streamer::getStreamId(int *stream_idx, AVCodecContext **dec_ctx,
                    AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret = 0;

    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0)
    {
        SPDLOG_WARN("Could not find {0} stream in input file '{1}'",
                av_get_media_type_string(type), m_input_file.c_str());
        return -1;
    } 
    else 
    {
        *stream_idx = ret;
        #if 0
        st = fmt_ctx->streams[*stream_idx];
        
        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }
        
        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            SPDLOG_WARN("Failed to copy {0} codec parameters to decoder context\n",
                av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders */
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        #endif

        return 0;
    }
}

int Ffmpeg_streamer::copyStreamParamIntoOut()
{
    int stream_index = 0;
    int stream_mapping_size = m_input_format_content->nb_streams;
    int ret = 0;

    std::vector<int>stream_mapping;
    stream_mapping.resize(stream_mapping_size);
	m_out_format = m_output_format_content->oformat;

    for (int i = 0; i < m_input_format_content->nb_streams; i++) {
        AVStream *out_stream;
        AVStream *in_stream = m_input_format_content->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;
 
        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[i] = -1;
            continue;
        }
        stream_mapping[i] = stream_index++;

        out_stream = avformat_new_stream(m_output_format_content, NULL);
        if (!out_stream)
        {
            SPDLOG_WARN("Failed allocating output stream");
            return -1;
        }

        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0) {
            SPDLOG_WARN("Failed to copy codec parameters");
            return -1;
        }
        out_stream->codecpar->codec_tag = 0;
    }
    return 0;
}

int Ffmpeg_streamer::publishStream()
{
    int ret = 0;
    int frame_index = 0;
    long long start_time = av_gettime();
    AVStream* in_stream = nullptr;
    AVStream* out_stream = nullptr;
    AVPacket pkg;
    AVRational time_base1;
    long long  calc_duration;
    AVRational time_base_q = {1, AV_TIME_BASE};
    long long pts_time;
    long long now_time;

    if(!(m_out_format->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&m_output_format_content->pb, m_output_url.c_str(), AVIO_FLAG_WRITE);
        if(ret < 0)
        {
            SPDLOG_WARN("Could not open output URL {0}", m_output_url.c_str());
            return F_FAILED;
        }
    }

    ret = avformat_write_header(m_output_format_content, nullptr);
	if (ret < 0) {
        SPDLOG_WARN("Error occurred when opening output URL");
        return F_FAILED;
	}
    
    while(1)
    {   

        if(m_start_stop_push_stream)
        {
            SPDLOG_WARN( "ctrl + c push stream stop");
            break;
        }
 
        ret = av_read_frame(m_input_format_content, &pkg);
        if(ret < 0)
        {
            SPDLOG_WARN("Failed to read frame");
            return -1;
        }

        if(pkg.pts == AV_NOPTS_VALUE)
        {
            time_base1 = m_input_format_content->streams[m_video_index]->time_base;
            calc_duration = (double) AV_TIME_BASE / av_q2d(m_input_format_content->streams[m_video_index]->r_frame_rate);
            // 两帧之间的持续时间
            pkg.pts = (double) (frame_index * calc_duration) / (double)(av_q2d(time_base1) * AV_TIME_BASE);
            pkg.dts = pkg.pts;
            pkg.duration = (double)calc_duration / (double)(av_q2d(time_base1) * AV_TIME_BASE);
        }

        // 延迟
        if(pkg.stream_index == m_video_index)
        {
            time_base1 = m_input_format_content->streams[m_video_index]->time_base;
            pts_time = av_rescale_q(pkg.dts, time_base1, time_base_q);
            now_time = av_gettime() - start_time;
            if(pts_time > now_time)
            {
                av_usleep(pts_time - now_time);
            }
        }

        in_stream = m_input_format_content->streams[pkg.stream_index];
        out_stream = m_output_format_content->streams[pkg.stream_index];

        // 转换pts/dts
        pkg.pts = av_rescale_q_rnd(pkg.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkg.dts = av_rescale_q_rnd(pkg.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkg.duration = av_rescale_q(pkg.duration, in_stream->time_base, out_stream->time_base);
        pkg.pos = -1;

        if(pkg.stream_index == m_video_index)
        {
            SPDLOG_INFO("Send {} video frame to output URL", frame_index);
            frame_index ++;
        }

        ret = av_interleaved_write_frame(m_output_format_content, &pkg);
        if(ret < 0)
        {
			SPDLOG_ERROR( "Error muxing packet");
			break;
        }
		av_packet_unref(&pkg);
	}
	//Write file trailer
	av_write_trailer(m_output_format_content);
    return 0;
}
