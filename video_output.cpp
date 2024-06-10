#include "video_output.h"

#include <filesystem>
#include <iostream>

VideoOutput::VideoOutput(std::string save_path, bool save_sei, QObject* parent) : QObject(parent)
{
    save_file_path_ = save_path;
    save_SEI_		= save_sei;
    sei_data_		= new Data_sei_str;
    engine_player_	= new HevcQImageEngine(0);
}

VideoOutput::~VideoOutput()
{
    delete sei_data_;
    delete engine_player_;
}

void VideoOutput::saveVideo()
{
    std::cout << "=============== Save path: " << save_file_path_ << " ===============" << std::endl;
    std::cout << "=============== Save option: " << ((save_SEI_ == 1) ? ("With SEI") : ("Without SEI")) << " ===============" << std::endl;

    saving = true;

    avio_seek(engine_player_->formatContext->pb, 0, SEEK_SET);

    current_frame_ = -1;
    bool need_init = true;
    // где-то тут начинаем while пока файл не закончился

    int one_percent = ((engine_player_->total_frames_) / 100);
    int progress;

    for (int i = engine_player_->first_keyframe_; i < engine_player_->total_frames_; ++i)
    {

        engine_player_->readFrame();
        engine_player_->processingFrame(img_);
        if (save_SEI_)
        {
            //сохраняем каждый фрейм с сеи
            if (engine_player_->getSei(sei_data_))					  //вернет 1 если всё ок
                engine_player_->drawDataOnFrame(sei_data_, &img_);	  //отправляем в рисовашку
            else
                std::cout << "Error get sei" << std::endl;
        }
        ++current_frame_;

        //инициализация выходного стрима
        if (need_init)
        {
            initializeOutputStream();
            need_init = false;
        }

        if (output_video_stream_initialized_)
            encode_video_frame_and_put_to_stream();	   //где-то тут ошибка с таймшптамом

        if (i != 0)
        {
            progress = i / one_percent;
            if (progress >= 100)
                progress = 99;
            emit savingProgress(progress);
        }
        if (!saving)
            i = engine_player_->total_frames_;	  // пиздец костыль, переделать
                                                  // std::this_thread::sleep_for(std::chrono::microseconds(10000));
    }
    stop_output_stream();
    progress = -1;	  //отправляем -1 когда сохранение завершено и поток остановлен
    emit savingProgress(progress);
}
void VideoOutput::encode_video_frame_and_put_to_stream()
{

    if (img_.width() != video_output_stream_.codecContext->width || img_.height() != video_output_stream_.codecContext->height ||
        static_cast<int>(channels_) != video_output_stream_.codecContext->channels)
    {
        std::cout << "VideoOutput::EncodeAndWrite wrong size" << std::endl;
        output_video_stream_initialized_ = false;
        return;
    }

    const uint8_t* sws_data[1] = {(uint8_t*)img_.constBits()};

    sws_scale(SwsCtx_, sws_data, InLineSize, 0, video_output_stream_.codecContext->height, video_output_stream_.frame->data, video_output_stream_.frame->linesize);

    video_output_stream_.frame->pts = video_output_stream_.NextPts++;

    auto status = FFmpegEncode(video_output_stream_.frame);

    if (status < 0)
        std::cout << "Error encoding frame" << total_frames_number_ << std::endl;

    status = put_frame_to_stream();

    if (status < 0)
    {
        //================РЕШИТЬ ЭТУ ХЕРЬ по факту всё работает std::cout << "Error while writing video frame" << std::endl;
    }

    av_packet_unref(&out_packet_);
}

int VideoOutput::put_frame_to_stream()
{
    av_packet_rescale_ts(&out_packet_, video_output_stream_.codecContext->time_base, video_output_stream_.Stream->time_base);
    out_packet_.stream_index = video_output_stream_.Stream->index;
    return av_interleaved_write_frame(out_format_context_, &out_packet_);
}

int VideoOutput::FFmpegEncode(AVFrame* frame)
{
    GotOutput = 0;
    auto ret  = avcodec_send_frame(video_output_stream_.codecContext, frame);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        auto error = av_make_error_string(errorBuf_, AV_ERROR_MAX_STRING_SIZE, ret);
        std::cout << "error during sending frame, error :" << error << std::endl;
        return -1;
    }

    ret = avcodec_receive_packet(video_output_stream_.codecContext, &out_packet_);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return 0;

    if (ret < 0)
    {
        auto error = av_make_error_string(errorBuf_, AV_ERROR_MAX_STRING_SIZE, ret);
        std::cout << "Error during receiving frame, error : " << error << std::endl;
        av_packet_unref(&out_packet_);
        return -1;
    }

    GotOutput = 1;
    return 0;
}

bool VideoOutput::initializeOutputStream()
{
    channels_ = static_cast<uint64_t>(img_.format() == QImage::Format_RGB888 ? 3 : img_.format() == QImage::Format_Grayscale8 ? 1 : 0);
    width_	  = static_cast<uint64_t>(img_.width());
    height_	  = static_cast<uint64_t>(img_.height());

    output_video_stream_initialized_ = false;
    total_frames_number_			 = 0;

    // create filename
    std::string output_video_url_ = makeOutputURL();

    // init ffmpeg
    out_packet_ = AVPacket();
    av_init_packet(&out_packet_);
    avformat_alloc_output_context2(&out_format_context_, nullptr, nullptr, output_video_url_.c_str());
    if (!out_format_context_)
    {
        // LOG_ERROR << "cannot alloc format context";
        return false;
    }
    output_format_	 = out_format_context_->oformat;
    const auto codec = avcodec_find_encoder(output_stream_codec_ID_);

    out_format_context_->video_codec = codec;

    if (output_format_->video_codec != AV_CODEC_ID_NONE)
    {
        output_stream_codec_ID_ = output_format_->video_codec;
        AddStream();
    }

    if (!OpenVideo())
    {
        return false;
    }

    video_output_stream_.NextPts = 0;
    av_dump_format(out_format_context_, 0, output_video_url_.c_str(), 1);

    if (!(output_format_->flags & AVFMT_NOFILE))
    {
        if (avio_open(&out_format_context_->pb, output_video_url_.c_str(), AVIO_FLAG_WRITE) < 0)
        {
            // LOG_ERROR<<"Could not open "<< output_stream_file_name.c_str();
            return false;
        }
    }

    if (avformat_write_header(out_format_context_, &Opt_) < 0)
    {
        // LOG_ERROR << "Error occurred when writing header to: " << output_stream_file_name.c_str();
        return false;
    }

    InLineSize[0] = width_ * channels_;

    SwsCtx_							 = sws_getContext(width_, height_,
                              AV_PIX_FMT_RGB24, width_,
                              height_, video_output_stream_.codecContext->pix_fmt,
                              SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
    output_video_stream_initialized_ = true;

    return true;
}

bool VideoOutput::OpenVideo()
{
    AVDictionary* opt = nullptr;

    if (av_dict_copy(&opt, Opt_, 0) != 0)
        std::cout << "av_dict_copy Error" << std::endl;

    const auto ret = avcodec_open2(video_output_stream_.codecContext, video_codec_, &opt);
    av_dict_free(&opt);

    if (ret < 0)
    {
        std::cout << "Could not open video codec" << ret << std::endl;
    }

    allocate_frame_buffer();

    if (!video_output_stream_.frame)
    {
        std::cout << "Could not allocate video frame" << std::endl;
        return false;
    }

    if (avcodec_parameters_from_context(video_output_stream_.Stream->codecpar, video_output_stream_.codecContext))
    {
        std::cout << "Could not copy the stream parameters" << std::endl;
        return false;
    }

    return true;
}
void VideoOutput::allocate_frame_buffer()
{
    video_output_stream_.frame = av_frame_alloc();
    if (!video_output_stream_.frame)
    {
        // LOG_ERROR<<"av_frame_alloc failed.";
        return;
    }

    video_output_stream_.frame->format = video_output_stream_.codecContext->pix_fmt;
    video_output_stream_.frame->width  = width_;
    video_output_stream_.frame->height = height_;

    if (av_frame_get_buffer(video_output_stream_.frame, 32) < 0)
    {
        // LOG_ERROR<<"Could not allocate frame data";
    }
}
void VideoOutput::AddStream()
{
    video_codec_ = avcodec_find_encoder(output_stream_codec_ID_);
    if (!video_codec_)
    {
        std::cout << "Could not find encoder  " << std::endl;
    }

    video_output_stream_.Stream = avformat_new_stream(out_format_context_, nullptr);
    if (!video_output_stream_.Stream)
    {
        std::cout << "Could not allocate stream" << std::endl;
    }

    video_output_stream_.Stream->id	  = out_format_context_->nb_streams - 1;
    video_output_stream_.codecContext = avcodec_alloc_context3(video_codec_);

    if (!video_output_stream_.codecContext)
    {
        std::cout << "Could not alloc an encoding context" << std::endl;
    }

    video_output_stream_.codecContext->bit_rate = 1024 * bit_rate_;
    video_output_stream_.codecContext->codec_id = output_stream_codec_ID_;
    video_output_stream_.codecContext->width	= width_;
    video_output_stream_.codecContext->height	= height_;
    video_output_stream_.codecContext->channels = channels_;
    video_output_stream_.Stream->time_base = video_output_stream_.codecContext->time_base = {1, engine_player_->fps_};
    video_output_stream_.codecContext->gop_size											  = 10;
    video_output_stream_.codecContext->max_b_frames										  = 1;
    video_output_stream_.codecContext->pix_fmt											  = AV_PIX_FMT_YUV420P;

    if (out_format_context_->oformat->flags & AVFMT_GLOBALHEADER)
        video_output_stream_.codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
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

    auto temp_name	  = save_file_path_ + std::filesystem::path::preferred_separator + video_name + "_saved_" + ".avi";
    int existed_names = 0;
    while (std::filesystem::exists(temp_name))
    {
        temp_name = save_file_path_ + std::filesystem::path::preferred_separator + video_name + "_saved_" + std::to_string(existed_names) + ".avi";
        existed_names++;
    };
    return temp_name;
}

void VideoOutput::stop_output_stream()
{
    if (!output_video_stream_initialized_)
        return;

    /* get the delayed frames ???????*/
    for (GotOutput = 1; GotOutput; total_frames_number_++)
    {
        fflush(stdout);

        FFmpegEncode(nullptr);

        if (GotOutput)
        {
            const auto ret = put_frame_to_stream();
            if (ret < 0)
            {
                // LOG_ERROR << "Error while writing video frame:" << ret;
            }
            av_packet_unref(&out_packet_);
        }
    }

    const auto ret = av_write_trailer(out_format_context_);
    if (ret < 0)
    {
        // LOG_ERROR<<"writing trailer error: "<< ret;
    }

    CloseStream();

    output_video_stream_initialized_ = false;
}

void VideoOutput::CloseStream()
{
    avcodec_free_context(&video_output_stream_.codecContext);
    av_frame_free(&video_output_stream_.frame);
    sws_freeContext(SwsCtx_);

    if (!(output_format_->flags & AVFMT_NOFILE))
    {
        const auto ret = avio_closep(&out_format_context_->pb);
        if (ret < 0)
        {
            //			LOG_ERROR << "CloseStream close failed";
        }
    }

    avformat_free_context(out_format_context_);
}
