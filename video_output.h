#ifndef VIDEO_OUTPUT_H
#define VIDEO_OUTPUT_H

#include "hevc_qimage_engine.h"
#include <QObject>
#include <string>

/**
 * @brief Saves .hevc file to .avi with or without SEI
 */
class VideoOutput : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Сonstructor creates HevcQImageEngine object of opened file (again)
     * to ensure independent work of the player and saving
     *
     * @param[in] string is a path to the folder where the current open video will be saved
     * @param[in] string is a full path to the open file
     * @param[in] parent needed to access the QT framework functionality
     */
    explicit VideoOutput(std::string, std::string, QObject* parent = 0);

    /**
     * @brief Freeing memory of engine_player_;
     */
    ~VideoOutput();

    /**
     * @brief Main save function, manages side processes of saving a file
     */
    void saveVideo();

private:
    /**
     * @brief Checking whether metadata needs to be saved
     *
     * @return 1 if need to save at least one type of SEI, 0 otherwise
     */
    bool saveSei();

    /**
     * @brief Configures codecs, frame buffer, stream, saved file name
     *
     * @return 1 on success, 0 on error
     */
    bool initializeOutputStream();

    /**
     * @brief Create name of saved video file
     * @return full path with a unique name for the saved file
     */
    std::string makeOutputURL();

    //!@{
    //!  Subfunctiong of saveVieo with ffmpeg
    void addStream();
    bool openVideo();
    void allocateFrameBuffer();
    void encodeFrameAndPutToStream();
    int ffmpegEncode(AVFrame* frame);
    int putFrameToStream();
    //!@}

    /**
     * @brief Calculates what percentage of the video was saved and sends the value to GUI
     * @param int is a number of last saved frame
     */
    void updateProgress(int);

    //!@{
    //! Correct completion of saving
    void stopOutputStream();
    void closeStream();
    //!@}

signals:
    /**
     * @brief Singal from saveVideo() to GUI to display saving progress
     *
     * @param[out] video saving percentage
     */
    void savingProgress(int progress);

public:
    /**
     * @brief Class for working with a .hevc video file through a ffmpeg library
     */
    HevcQImageEngine* engine_player_;

    /**
     * @brief Set true when press SAVE button in GUI,
     * set false when press SAVE button in GUI while process of saving to stop saving early
     */
    bool saving_ = false;

private:
    /**
     * @brief Full path to the open file
     */
    std::string open_file_path_;

    /**
     * @brief Path to the folder where the current open video will be saved
     */
    std::string save_file_path_;

    /** @name part of code taken from IIPCT
    */
    //@{
    uint64_t width_;
    uint64_t height_;
    uint64_t channels_;
    bool output_video_stream_initialized_ = false;
    int total_frames_number_			  = 0;

    struct OutputStream
    {
        AVStream* stream_;
        AVCodecContext* codec_context_;
        int64_t next_pts_;
        AVFrame* frame_;
    };
    OutputStream video_output_stream_ = OutputStream();

    AVPacket out_packet_;
    AVFormatContext* out_format_context_;
    AVOutputFormat* output_format_;
    AVCodecID output_stream_codec_ID_{AV_CODEC_ID_H264};
    AVCodec* video_codec_;

    int bit_rate_	   = 7500;
    AVDictionary* Opt_ = nullptr;
    int in_line_size_[1]{};
    SwsContext* SwsCtx_;
    bool got_output_{};
    char error_buf_[AV_ERROR_MAX_STRING_SIZE] = {};
    //@}
};

#endif
