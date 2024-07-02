#ifndef VIDEO_OUTPUT_H
#define VIDEO_OUTPUT_H

#include "hevc_qimage_engine.h"
#include <QObject>
#include <string>

#define DEFAULT_BITRATE 7500

class VideoOutput : public QObject
{
    Q_OBJECT

private:
    uint64_t width_;
    uint64_t height_;
    uint64_t channels_;
    bool output_video_stream_initialized_ = false;
    int total_frames_number_			  = 0;	  //зачем отдельно в этом классе, почему не использовать от engine_player_?

    struct OutputStream
    {
        AVStream* Stream;
        AVCodecContext* codecContext;
        int64_t NextPts;
        AVFrame* frame;
        struct SwsContext* swsContext;
    };
    OutputStream video_output_stream_ = OutputStream();

public:
    HevcQImageEngine* engine_player_;

    std::string save_file_path_;	//перевести в класс hevc_qimage_engine
    std::string open_file_path_;
    //std::string output_video_url_;

    //для инициализации выходного стрима
    AVPacket out_packet_;
    AVFormatContext* out_format_context_;	 //public
    AVOutputFormat* output_format_;
    AVCodecID output_stream_codec_ID_{AV_CODEC_ID_H264};
    AVCodec* video_codec_;
    int bit_rate_{DEFAULT_BITRATE};
    AVDictionary* Opt_ = nullptr;
    int InLineSize[1]{};
    SwsContext* SwsCtx_;
    int GotOutput{};
    char errorBuf_[AV_ERROR_MAX_STRING_SIZE] = {};
    bool saving								 = false;

public:
    explicit VideoOutput(std::string, QObject* parent = 0);
    ~VideoOutput();
    /**
   * @brief Decoding, converting and getting SEI data from one frame
   *
   * @return 1 when processing of current frame is completed, 0 when the end of
   * the file is reached
   */
    void saveVideo();

    bool initializeOutputStream();

    std::string makeOutputURL();

    void AddStream();	 //переименовать в addStream;

    bool OpenVideo();

    void allocate_frame_buffer();

    void encode_video_frame_and_put_to_stream();

    int FFmpegEncode(AVFrame* frame);

    int put_frame_to_stream();

    void stop_output_stream();

    void CloseStream();

private:
    void updateProgress(int);
    bool saveSei();
signals:
    void savingProgress(int progress);
};

#endif	  // VIDEO_OUTPUT_H
