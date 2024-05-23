// #ifndef VIDEOOUTPUT_H
// #define VIDEOOUTPUT_H

// #include <mutex>
// #include <string>

// //#include "Primitives/Headers/ImageView.hpp"
// //#include "Utility/Headers/enum.h"
// //#include "VideoProcessing/Headers/configVideoIO.h"

// extern "C"
// {
// #include <libavcodec/avcodec.h>
// #include <libavfilter/avfilter.h>
// #include <libavformat/avformat.h>
// #include <libswscale/swscale.h>
// }
// #define DEFAULT_BITRATE 7500
// #define DEFAULT_FRAMERATE			   25
// //  SAVE_VIDEOS

// /**
//  * @brief Write frames into video or images.
//  * Call "setCurrentOutputMode" to change output result.
//  * Main function to work is "writeFrame".
//  */
// class VideoOutput
// {
//     /**
//      * @brief This is used to allow create instances only from "Workers" class.
//      */
//     //?? friend class Workers;
//     //?? static std::shared_ptr<VideoOutput> make_shared() { return std::shared_ptr<VideoOutput>(new VideoOutput); }

//     explicit VideoOutput();

// public:
//     ~VideoOutput();

//     enum class SAVE_MODE
//     {
//         SAVE_WITH_SEI,
//         SAVE_WITHOUT_SEI
//     };

//     SAVE_MODE save_mode_;

//     /**
//      * @brief Will write frame into result file.
//      * @param video_name and @param frame_num will be used for creating result file name.
//      */
//     void writeFrame(const ImageView& frame, const std::string& video_name, int64_t frame_num);

//     void setBitRate(const int bit_rate);
//     int getBitRate() const;

//     void setFolder(const std::string& folder);
//     std::string getFolder() const;

//     void setImageFrameRate(const int frame_rate);
//     int getImageFrameRate() const;

//     void setFrameRate(const int frame_rate);
//     int getFrameRate() const;

//     void setImageCompression(int new_compression);
//     int getImageCompression() const;

//     void setCurrentOutputMode(SAVE_MODE new_mode);

// private:
//     struct OutputStream
//     {
//         AVStream* Stream;
//         AVCodecContext* codecContext;
//         int64_t NextPts;
//         AVFrame* frame;
//         struct SwsContext* swsContext;
//     };

//     void stop_output_stream();
//     void write_frame_to_video_stream(const ImageView& frame);
//     bool initializeOutputStream(const ImageView& frame);

//     void encode_video_frame_and_put_to_stream(size_t const& width, size_t const& height, size_t const& channels, const uint8_t* imageData);

//     int put_frame_to_stream();

//     void AddStream();

//     bool OpenVideo();

//     void CloseStream();
//     void allocate_frame_buffer();

//     int FFmpegEncode(AVFrame* frame);

//     std::mutex output_video_stream_write_mutex;
//     std::mutex output_image_stream_write_mutex;

//     OutputStream video_output_stream_ = OutputStream();

//     AVOutputFormat* output_format_{};
//     AVCodec* video_codec_{};

//     AVFormatContext* format_context_{};
//     AVCodecID output_stream_codec_ID_{AV_CODEC_ID_H264};
//     AVDictionary* Opt_ = nullptr;
//     SwsContext* SwsCtx_{};
//     AVPacket packet_{};
//     std::string output_image_url_{};
//     std::string output_video_url_{};
//     // std::string folder_path_{DEFAULT_OUTPUT_VIDEO_PATH};
//     // std::string sequence_name_{DEFAULT_SEQUENCE_NAME};
//     std::string video_name_{};

//     const int default_image_sequence_prefix_size_ = 7;

//     long long total_frames_number_{};
//     long long frames_processed_{};

//     int GotOutput{};
//     int InLineSize[1]{};

//     bool output_video_stream_initialized_{false};
//     bool recording_ongoing_{false};

//     char errorBuf_[AV_ERROR_MAX_STRING_SIZE] = {};

//     int bit_rate_{DEFAULT_BITRATE};
//     int width_{};
//     int height_{};
//     int channels_{};
//     int frame_rate_{DEFAULT_FRAMERATE};
//     int image_frame_rate_{DEFAULT_IMG_STREAM_FRAMERATE};
//     int image_compression_ratio_{DEFAULT_JPEG_COMPRESSION_RATIO};
//     int video_number_{};
//     int image_number_{};
// };

// #endif	  // VIDEOOUTPUT_H
