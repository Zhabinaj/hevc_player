#include "video_output.h"

//void ffmpeg::saveVideo(std::string path_to_save, bool save_mode)
//{
//     std::cout << "save path: " << path_to_save << std::endl;
//     std::cout << "save option: " << save_mode << std::endl;

//     if (save_mode)
//         save_mode_ = SAVE_MODE::SAVE_WITH_SEI;
//     else
//         save_mode_ = SAVE_MODE::SAVE_WITHOUT_SEI;

//     avio_seek(formatContext->pb, 0, SEEK_SET);
//     playing_mode_ = PLAYING_MODE::PROCESSING;

//     current_frame_ = 0;
//     play();	   //получили и обработали нулевой фрейм QImage timg_

//     // std::unique_lock<std::mutex> lck(imageMutexWriteImage); ???
//     // lck.unlock();???

//     //=============запуск
//     //video_output_ заменить на video_file_
//     //const ImageView& frame переведенный QImage
//     //const std::string& video_name это file_path_
//     //int64_t frame_num это текущий фрейм

//     //остатки writeFrame
//     /**
//      * @brief Will write frame into result file.
//      * @param video_name and @param frame_num will be used for creating result file name.
//      */

//     //const uint64_t stride = width * channels * sizeof(uint8_t);

//     if (save_mode_ == SAVE_MODE::SAVE_WITHOUT_SEI)
//     {

//         //  if (video_name_.compare(file_path_))
//         // {
//         //  stop_output_stream(); паузим стрим
//         //video_name_		   = file_path_; ??нахуя
//         bool recording_ongoing_ = initializeOutputStream(this->timg_);	  // ImageView& frame = timg_ ==================HERE
//         //}

//         //std::lock_guard lock(output_video_stream_write_mutex);
//         if (recording_ongoing_)	   //если инициализация успешна
//         {
//             if (output_video_stream_initialized_)	 //переменная внутри initializeOutputStream
//             {
//                 encode_video_frame_and_put_to_stream(frame->width(), frame->height(), frame->channels(), frame.ptr<uint8_t>());
//             }
//         }
//     }
//     else
//         std::cout << "Error writeFrame" << std::endl;	 //место для реализации с sei
//}

// bool ffmpeg::initializeOutputStream(QImage frame_to_save)	 // ImageView& frame = timg_
// {
//     width_	  = static_cast<uint64_t>(frame_to_save.width());
//     height_	  = static_cast<uint64_t>(frame_to_save.height());
//     channels_ = static_cast<uint64_t>(frame_to_save.format() == QImage::Format_RGB888 ? 3 : frame_to_save.format() == QImage::Format_Grayscale8 ? 1 : 0);

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
