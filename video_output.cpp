#include "video_output.h"

#include <boost/filesystem.hpp>
#include <iostream>

#define SAVE_COMPLETED 100

VideoOutput::VideoOutput(std::string save_path, std::string open_path, QObject* parent) : QObject(parent)
{
    save_file_path_ = save_path;
    engine_player_	= new HevcQImageEngine(0);
    engine_player_->initialization(open_path);
}

VideoOutput::~VideoOutput()
{
    delete engine_player_;
}

void VideoOutput::saveVideo()
{
    saving_ = true;

    avio_seek(engine_player_->format_context_->pb, 0, SEEK_SET);

    bool save_sei = saveSei();

    for (int i = engine_player_->first_keyframe_; i < engine_player_->total_frames_; ++i)
    {
        engine_player_->readFrame();
        engine_player_->processingFrame();
        if (save_sei)
        {
            engine_player_->getSei();
            engine_player_->drawDataOnQImage();
        }

        if (i == engine_player_->first_keyframe_)
            initializeOutputStream();

        if (output_video_stream_initialized_)
            encodeFrameAndPutToStream();

        updateProgress(i);

        //if saving is interrupted from the GUI, set the current frame = last frame to correctly complete the save
        if (!saving_)
            i = engine_player_->total_frames_;
    }
    stopOutputStream();
    emit savingProgress(SAVE_COMPLETED);
}

bool VideoOutput::saveSei()
{
    for (int i = 0; i < 12; ++i)
    {
        if (engine_player_->sei_options_[i])
            return 1;
    }
    return 0;
}

bool VideoOutput::initializeOutputStream()
{
    channels_ = static_cast<uint64_t>(engine_player_->q_img_.format() == QImage::Format_RGB888 ? 3 : engine_player_->q_img_.format() == QImage::Format_Grayscale8 ? 1 : 0);
    width_	  = static_cast<uint64_t>(engine_player_->q_img_.width());
    height_	  = static_cast<uint64_t>(engine_player_->q_img_.height());

    output_video_stream_initialized_ = false;
    total_frames_number_			 = 0;

    std::string output_video_url_ = makeOutputURL();

    out_packet_ = AVPacket();
    av_init_packet(&out_packet_);

    avformat_alloc_output_context2(&out_format_context_, nullptr, nullptr, output_video_url_.c_str());
    if (!out_format_context_)
        return false;

    output_format_	 = out_format_context_->oformat;
    const auto codec = avcodec_find_encoder(output_stream_codec_ID_);

    out_format_context_->video_codec = codec;

    if (output_format_->video_codec != AV_CODEC_ID_NONE)
    {
        output_stream_codec_ID_ = output_format_->video_codec;
        addStream();
    }

    if (!openVideo())
        return false;

    video_output_stream_.next_pts_ = 0;
    av_dump_format(out_format_context_, 0, output_video_url_.c_str(), 1);

    if (!(output_format_->flags & AVFMT_NOFILE))
    {
        if (avio_open(&out_format_context_->pb, output_video_url_.c_str(), AVIO_FLAG_WRITE) < 0)
            return false;
    }

    if (avformat_write_header(out_format_context_, &Opt_) < 0)
        return false;

    in_line_size_[0] = width_ * channels_;

    SwsCtx_							 = sws_getContext(width_, height_,
                              AV_PIX_FMT_RGB24, width_,
                              height_, video_output_stream_.codec_context_->pix_fmt,
                              SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
    output_video_stream_initialized_ = true;

    return true;
}

std::string VideoOutput::makeOutputURL()
{
    std::string video_name = "";
    int start_pos		   = engine_player_->open_file_name_.size() - 6;
    while (1)
    {
        if (engine_player_->open_file_name_[start_pos] == '/')
            break;
        else
            video_name += engine_player_->open_file_name_[start_pos];
        --start_pos;
    }
    reverse(video_name.begin(), video_name.end());

    std::string temp_name;
    int existed_names = 0;

#ifdef WINDOWS
    temp_name = save_file_path_ + '\\' + video_name + "_saved_" + ".avi";

    while (boost::filesystem::exists(temp_name))
    {
        temp_name = save_file_path_ + '\\' + video_name + "_saved_" + std::to_string(existed_names) + ".avi";
        existed_names++;
    };

#elif linux
    temp_name = save_file_path_ + boost::filesystem::path::preferred_separator + video_name + "_saved_" + ".avi";

    while (boost::filesystem::exists(temp_name))
    {
        temp_name = save_file_path_ + boost::filesystem::path::preferred_separator + video_name + "_saved_" + std::to_string(existed_names) + ".avi";
        existed_names++;
    };
#endif

    return temp_name;
}

void VideoOutput::updateProgress(int frame)
{
    int one_percent = ((engine_player_->total_frames_) / 100);

    int progress = frame / one_percent;
    if (progress >= 100)
        progress = 99;

    emit savingProgress(progress);
}

void VideoOutput::encodeFrameAndPutToStream()
{
    if (engine_player_->q_img_.width() != video_output_stream_.codec_context_->width || engine_player_->q_img_.height() != video_output_stream_.codec_context_->height ||
        static_cast<int>(channels_) != video_output_stream_.codec_context_->channels)
    {
        std::cout << "VideoOutput::EncodeAndWrite wrong size" << std::endl;
        output_video_stream_initialized_ = false;
        return;
    }

    const uint8_t* sws_data[1] = {(uint8_t*)engine_player_->q_img_.constBits()};

    sws_scale(SwsCtx_, sws_data, in_line_size_, 0, video_output_stream_.codec_context_->height, video_output_stream_.frame_->data, video_output_stream_.frame_->linesize);

    video_output_stream_.frame_->pts = video_output_stream_.next_pts_++;

    auto status = ffmpegEncode(video_output_stream_.frame_);

    if (status < 0)
        std::cout << "Error encoding frame" << total_frames_number_ << std::endl;

    status = putFrameToStream();

    if (status < 0)
    {
        //everything works, but av_interleaved_write_frame return error
        //std::cout << "Error while writing video frame" << std::endl;
    }

    av_packet_unref(&out_packet_);
}

int VideoOutput::putFrameToStream()
{
    av_packet_rescale_ts(&out_packet_, video_output_stream_.codec_context_->time_base, video_output_stream_.stream_->time_base);
    out_packet_.stream_index = video_output_stream_.stream_->index;
    return av_interleaved_write_frame(out_format_context_, &out_packet_);
}

int VideoOutput::ffmpegEncode(AVFrame* frame)
{
    got_output_ = 0;
    auto ret	= avcodec_send_frame(video_output_stream_.codec_context_, frame);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        auto error = av_make_error_string(error_buf_, AV_ERROR_MAX_STRING_SIZE, ret);
        std::cout << "error during sending frame, error :" << error << std::endl;
        return -1;
    }

    ret = avcodec_receive_packet(video_output_stream_.codec_context_, &out_packet_);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return 0;

    if (ret < 0)
    {
        auto error = av_make_error_string(error_buf_, AV_ERROR_MAX_STRING_SIZE, ret);
        std::cout << "Error during receiving frame, error : " << error << std::endl;
        av_packet_unref(&out_packet_);
        return -1;
    }

    got_output_ = 1;
    return 0;
}

bool VideoOutput::openVideo()
{
    AVDictionary* opt = nullptr;

    if (av_dict_copy(&opt, Opt_, 0) != 0)
        std::cout << "av_dict_copy Error" << std::endl;

    const auto ret = avcodec_open2(video_output_stream_.codec_context_, video_codec_, &opt);
    av_dict_free(&opt);

    if (ret < 0)
    {
        std::cout << "Could not open video codec" << ret << std::endl;
    }

    allocateFrameBuffer();

    if (!video_output_stream_.frame_)
    {
        std::cout << "Could not allocate video frame" << std::endl;
        return false;
    }

    if (avcodec_parameters_from_context(video_output_stream_.stream_->codecpar, video_output_stream_.codec_context_))
    {
        std::cout << "Could not copy the stream parameters" << std::endl;
        return false;
    }

    return true;
}
void VideoOutput::allocateFrameBuffer()
{
    video_output_stream_.frame_ = av_frame_alloc();
    if (!video_output_stream_.frame_)
    {
        std::cout << "av_frame_alloc failed" << std::endl;
        return;
    }

    video_output_stream_.frame_->format = video_output_stream_.codec_context_->pix_fmt;
    video_output_stream_.frame_->width	= width_;
    video_output_stream_.frame_->height = height_;

    if (av_frame_get_buffer(video_output_stream_.frame_, 32) < 0)
        std::cout << "Could not allocate frame data" << std::endl;
}
void VideoOutput::addStream()
{
    video_codec_ = avcodec_find_encoder(output_stream_codec_ID_);
    if (!video_codec_)
        std::cout << "Could not find encoder  " << std::endl;

    video_output_stream_.stream_ = avformat_new_stream(out_format_context_, nullptr);
    if (!video_output_stream_.stream_)
        std::cout << "Could not allocate stream" << std::endl;

    video_output_stream_.stream_->id	= out_format_context_->nb_streams - 1;
    video_output_stream_.codec_context_ = avcodec_alloc_context3(video_codec_);

    if (!video_output_stream_.codec_context_)
        std::cout << "Could not alloc an encoding context" << std::endl;

    video_output_stream_.codec_context_->bit_rate = 1024 * bit_rate_;
    video_output_stream_.codec_context_->codec_id = output_stream_codec_ID_;
    video_output_stream_.codec_context_->width	  = width_;
    video_output_stream_.codec_context_->height	  = height_;
    video_output_stream_.codec_context_->channels = channels_;
    video_output_stream_.stream_->time_base = video_output_stream_.codec_context_->time_base = {1, engine_player_->fps_};
    video_output_stream_.codec_context_->gop_size											 = 10;
    video_output_stream_.codec_context_->max_b_frames										 = 1;
    video_output_stream_.codec_context_->pix_fmt											 = AV_PIX_FMT_YUV420P;

    if (out_format_context_->oformat->flags & AVFMT_GLOBALHEADER)
        video_output_stream_.codec_context_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

void VideoOutput::stopOutputStream()
{
    if (!output_video_stream_initialized_)
        return;

    /* get the delayed frames ???????*/
    for (got_output_ = 1; got_output_; total_frames_number_++)
    {
        fflush(stdout);

        ffmpegEncode(nullptr);

        if (got_output_)
        {
            const auto ret = putFrameToStream();
            if (ret < 0)
                std::cout << "Error while writing video frame: " << ret << std::endl;

            av_packet_unref(&out_packet_);
        }
    }

    const auto ret = av_write_trailer(out_format_context_);
    if (ret < 0)
        std::cout << "Writing trailer error: " << ret << std::endl;

    closeStream();

    output_video_stream_initialized_ = false;
}

void VideoOutput::closeStream()
{
    avcodec_free_context(&video_output_stream_.codec_context_);
    av_frame_free(&video_output_stream_.frame_);
    sws_freeContext(SwsCtx_);

    if (!(output_format_->flags & AVFMT_NOFILE))
    {
        const auto ret = avio_closep(&out_format_context_->pb);
        if (ret < 0)
            std::cout << "CloseStream close failed" << std::endl;
    }
    avformat_free_context(out_format_context_);
}
