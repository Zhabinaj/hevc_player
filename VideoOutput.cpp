// #include "VideoOutput.h"

// #include "Utility/Headers/ImageSequencer.h"
// #include "Utility/Headers/LogKeeper.h"

// #include <filesystem>

// #include <iostream>

// VideoOutput::VideoOutput()
// {

//     av_register_all();

//     total_frames_number_			 = 0;
//     output_video_stream_initialized_ = false;
//     recording_ongoing_				 = false;
//     image_compression_ratio_		 = DEFAULT_JPEG_COMPRESSION_RATIO;
// }

// VideoOutput::~VideoOutput()
// {
//     //	LOG_INFO << "STOP VideoOutput";
//     stop_output_stream();
// }

// bool VideoOutput::initializeOutputStream(const ImageView& frame)
// {
//     width_							 = frame.width();
//     height_							 = frame.height();
//     channels_						 = frame.channels();
//     output_video_stream_initialized_ = false;
//     total_frames_number_			 = 0;

//     // create filename
//     auto temp_name	  = folder_path_ + std::filesystem::path::preferred_separator + video_name_ + "_saved_" + ".avi";
//     int existed_names = 0;
//     while (std::filesystem::exists(temp_name))
//     {
//         temp_name = folder_path_ + std::filesystem::path::preferred_separator + video_name_ + "_saved_" + std::to_string(existed_names) + ".avi";

//         existed_names++;
//     };
//     output_video_url_ = temp_name;

//     // init ffmpeg
//     packet_ = AVPacket();
//     av_init_packet(&packet_);

//     avformat_alloc_output_context2(&format_context_, nullptr, nullptr, output_video_url_.c_str());
//     if (!format_context_)
//     {
//         // LOG_ERROR << "cannot alloc format context";
//         return false;
//     }
//     output_format_ = format_context_->oformat;

//     // CodecID = AV_CODEC_ID_H264;
//     const auto codec = avcodec_find_encoder(output_stream_codec_ID_);	 // by ID

//     // const char codec_name[32] = "h264_nvenc";
//     // auto codec = avcodec_find_encoder_by_name(codec_name);//by NAME

//     format_context_->video_codec = codec;

//     if (output_format_->video_codec != AV_CODEC_ID_NONE)
//     {
//         output_stream_codec_ID_ = output_format_->video_codec;
//         AddStream();
//     }

//     if (!OpenVideo())
//     {
//         return false;
//     }

//     video_output_stream_.NextPts = 0;
//     av_dump_format(format_context_, 0, output_video_url_.c_str(), 1);

//     if (!(output_format_->flags & AVFMT_NOFILE))
//     {
//         if (avio_open(&format_context_->pb, output_video_url_.c_str(), AVIO_FLAG_WRITE) < 0)
//         {
//             // LOG_ERROR<<"Could not open "<< output_stream_file_name.c_str();
//             return false;
//         }
//     }

//     if (avformat_write_header(format_context_, &Opt_) < 0)
//     {
//         // LOG_ERROR << "Error occurred when writing header to: " << output_stream_file_name.c_str();
//         return false;
//     }

//     //init Sws
//     InLineSize[0] = width_ * channels_;
//     SwsCtx_		  = sws_getContext(width_, height_,
//                                AV_PIX_FMT_RGB24, width_,
//                                height_, video_output_stream_.codecContext->pix_fmt,
//                                SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

//     output_video_stream_initialized_ = true;

//     return true;
// }

// bool VideoOutput::OpenVideo()
// {
//     AVDictionary* opt = nullptr;

//     av_dict_copy(&opt, Opt_, 0);

//     const auto ret = avcodec_open2(video_output_stream_.codecContext, video_codec_, &opt);
//     av_dict_free(&opt);

//     if (ret < 0)
//     {
//         std::cout << "Could not open video codec" << std::endl;
//     }

//     allocate_frame_buffer();

//     if (!video_output_stream_.frame)
//     {
//         std::cout << "Could not allocate video frame" << std::endl;
//         return false;
//     }

//     if (avcodec_parameters_from_context(video_output_stream_.Stream->codecpar, video_output_stream_.codecContext))
//     {
//         std::cout << "Could not copy the stream parameters" << std::endl;
//         return false;
//     }

//     return true;
// }

// void VideoOutput::allocate_frame_buffer()
// {
//     video_output_stream_.frame = av_frame_alloc();
//     if (!video_output_stream_.frame)
//     {
//         // LOG_ERROR<<"av_frame_alloc failed.";
//         return;
//     }

//     video_output_stream_.frame->format = video_output_stream_.codecContext->pix_fmt;
//     video_output_stream_.frame->width  = width_;
//     video_output_stream_.frame->height = height_;

//     if (av_frame_get_buffer(video_output_stream_.frame, 32) < 0)
//     {
//         // LOG_ERROR<<"Could not allocate frame data";
//     }
// }

// void VideoOutput::AddStream()
// {
//     video_codec_ = avcodec_find_encoder(output_stream_codec_ID_);
//     if (!video_codec_)
//     {
//         LOG_ERROR << "Could not find encoder  ";
//     }

//     video_output_stream_.Stream = avformat_new_stream(format_context_, nullptr);
//     if (!video_output_stream_.Stream)
//     {
//         LOG_ERROR << "Could not allocate stream";
//     }

//     video_output_stream_.Stream->id	  = format_context_->nb_streams - 1;
//     video_output_stream_.codecContext = avcodec_alloc_context3(video_codec_);

//     if (!video_output_stream_.codecContext)
//     {
//         LOG_ERROR << "Could not alloc an encoding context";
//     }

//     video_output_stream_.codecContext->bit_rate = 1024 * bit_rate_;
//     video_output_stream_.codecContext->codec_id = output_stream_codec_ID_;
//     video_output_stream_.codecContext->width	= width_;
//     video_output_stream_.codecContext->height	= height_;
//     video_output_stream_.codecContext->channels = channels_;
//     video_output_stream_.Stream->time_base = video_output_stream_.codecContext->time_base = {1, frame_rate_};
//     video_output_stream_.codecContext->gop_size											  = 10;
//     video_output_stream_.codecContext->max_b_frames										  = 1;
//     video_output_stream_.codecContext->pix_fmt											  = AV_PIX_FMT_YUV420P;

//     // av_opt_set(videoOutputStream.codecContext->priv_data, "cq", TCHAR_TO_ANSI(*H264Crf), 0);  // change `cq` to `crf` if using libx264
//     // av_opt_set(videoOutputStream.codecContext->priv_data, "gpu", TCHAR_TO_ANSI(*DeviceNum), 0); // comment this line if using libx264

//     if (format_context_->oformat->flags & AVFMT_GLOBALHEADER)
//         video_output_stream_.codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
// }

// void VideoOutput::encode_video_frame_and_put_to_stream(size_t const& width, size_t const& height, size_t const& channels, const uint8_t* imageData)
// {
//     const uint8_t* inData[1] = {imageData};

//     if (static_cast<int>(width) != video_output_stream_.codecContext->width || static_cast<int>(height) != video_output_stream_.codecContext->height ||
//         static_cast<int>(channels) != video_output_stream_.codecContext->channels)
//     {
//         LOG_ERROR << "VideoOutput::EncodeAndWrite wrong size";
//         output_video_stream_initialized_ = false;
//         return;
//     }

//     sws_scale(SwsCtx_, inData, InLineSize, 0, video_output_stream_.codecContext->height, video_output_stream_.frame->data, video_output_stream_.frame->linesize);

//     video_output_stream_.frame->pts = video_output_stream_.NextPts++;

//     auto status = FFmpegEncode(video_output_stream_.frame);

//     if (status < 0)
//         LOG_ERROR << "Error encoding frame" << total_frames_number_;

//     status = put_frame_to_stream();

//     if (status < 0)
//     {
//         LOG_ERROR << "Error while writing video frame";
//     }

//     av_packet_unref(&packet_);
// }

// int VideoOutput::put_frame_to_stream()
// {
//     av_packet_rescale_ts(&packet_, video_output_stream_.codecContext->time_base, video_output_stream_.Stream->time_base);
//     packet_.stream_index = video_output_stream_.Stream->index;
//     return av_interleaved_write_frame(format_context_, &packet_);
// }

// int VideoOutput::FFmpegEncode(AVFrame* frame)
// {
//     GotOutput = 0;
//     auto ret  = avcodec_send_frame(video_output_stream_.codecContext, frame);
//     if (ret < 0 && ret != AVERROR_EOF)
//     {
//         auto error = av_make_error_string(errorBuf_, AV_ERROR_MAX_STRING_SIZE, ret);
//         LOG_ERROR << "error during sending frame, error :" << error;
//         return -1;
//     }

//     ret = avcodec_receive_packet(video_output_stream_.codecContext, &packet_);
//     if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
//         return 0;

//     if (ret < 0)
//     {
//         auto error = av_make_error_string(errorBuf_, AV_ERROR_MAX_STRING_SIZE, ret);
//         LOG_ERROR << "Error during receiving frame, error : " << error;
//         av_packet_unref(&packet_);
//         return -1;
//     }

//     GotOutput = 1;
//     return 0;
// }

// void VideoOutput::stop_output_stream()
// {
//     recording_ongoing_ = false;
//     video_name_		   = "";

//     if (!output_video_stream_initialized_)
//         return;

//     std::lock_guard lock(output_video_stream_write_mutex);
//     // LOG_INFO << " VideoOutput::finilize() ";
//     /* get the delayed frames */
//     for (GotOutput = 1; GotOutput; total_frames_number_++)
//     {
//         fflush(stdout);

//         FFmpegEncode(nullptr);

//         if (GotOutput)
//         {
//             const auto ret = put_frame_to_stream();
//             if (ret < 0)
//             {
//                 // LOG_ERROR << "Error while writing video frame:" << ret;
//             }
//             av_packet_unref(&packet_);
//         }
//     }

//     const auto ret = av_write_trailer(format_context_);
//     if (ret < 0)
//     {
//         // LOG_ERROR<<"writing trailer error: "<< ret;
//     }

//     CloseStream();

//     output_video_stream_initialized_ = false;
// }

// void VideoOutput::writeFrame(const ImageView& frame, const std::string& video_name, int64_t frame_num)
// {
//     if (save_mode_ == SAVE_MODE::SAVE_WITHOUT_SEI)
//     {

//         if (video_name_.compare(video_name))
//         {
//             stop_output_stream();
//             video_name_		   = video_name;
//             recording_ongoing_ = initializeOutputStream(frame);
//         }

//         std::lock_guard lock(output_video_stream_write_mutex);
//         if (recording_ongoing_)
//         {
//             if (output_video_stream_initialized_)
//             {
//                 encode_video_frame_and_put_to_stream(frame->width(), frame->height(), frame->channels(), frame.ptr<uint8_t>());
//             }
//         }
//     }
//     else
//         std::cout << "Error writeFrame" << std::endl;
// }

// void VideoOutput::CloseStream()
// {
//     //	LOG_INFO << " VideoOutput::CloseStream()";
//     avcodec_free_context(&video_output_stream_.codecContext);
//     av_frame_free(&video_output_stream_.frame);
//     sws_freeContext(SwsCtx_);

//     if (!(output_format_->flags & AVFMT_NOFILE))
//     {
//         const auto ret = avio_closep(&format_context_->pb);
//         if (ret < 0)
//         {
//             //			LOG_ERROR << "CloseStream close failed";
//         }
//     }

//     avformat_free_context(format_context_);
// }

// void VideoOutput::setFolder(const std::string& folder)
// {
//     folder_path_ = folder;
// }

// std::string VideoOutput::getFolder() const
// {
//     return folder_path_;
// }

// void VideoOutput::setImageFrameRate(const int frame_rate)
// {
//     image_frame_rate_ = frame_rate;
// }

// int VideoOutput::getImageFrameRate() const
// {
//     return image_frame_rate_;
// }

// void VideoOutput::setFrameRate(const int frame_rate)
// {
//     frame_rate_ = frame_rate;
// }

// void VideoOutput::setBitRate(const int bit_rate)
// {
//     bit_rate_ = bit_rate;
// }

// int VideoOutput::getBitRate() const
// {
//     return bit_rate_;
// }

// int VideoOutput::getFrameRate() const
// {
//     return frame_rate_;
// }

// void VideoOutput::setImageCompression(int new_compression)
// {
//     image_compression_ratio_ = new_compression;
// }

// int VideoOutput::getImageCompression() const
// {
//     return image_compression_ratio_;
// }

// void VideoOutput::setCurrentOutputMode(SAVE_MODE new_mode)
// {
//     stop_output_stream();

//     save_mode_ = new_mode;
// }
