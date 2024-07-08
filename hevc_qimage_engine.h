#ifndef HEVCQIMAGEENGINE_H
#define HEVCQIMAGEENGINE_H

#include <QImage>
#include <QObject>

#include <sei_data.h>
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

/**
 * @brief Class built on working with ffmpeg.
 * Opens, reads frame by frame .hevc file and paints SEI data
 */
class HevcQImageEngine : public QObject
{
    Q_OBJECT

public:
    explicit HevcQImageEngine(int id_str, QObject *parent = 0);

    /**
     * @brief freeing memory of sei_data_, v_frame_rgb_ and v_buffer_;
     */
    ~HevcQImageEngine();

    /**
    * @brief Opening .hevc file, setting, preparation for further decoding, getting
    * additional data such a Total Frames, FPS, Duration
    *
    * @param[in] path to chosen hevc file
    *
    * @return 1 on success, from -1 to -5 on failure
    */
    int initialization(std::string);

    /**
     * @brief Outputs additional metadata in terminal afther initialization
     */
    void initializationPrintData();

    /**
     * @brief Getting one packet_ of raw data from current .hevc file for each call
     *
     * @return 1 on success, 0 on error or end of file
     */
    bool readFrame();

    /**
     * @brief Decodes and convert the packet_ to QImage
     *
     * @return 1 if QImage was create and write, 0 on error
     */
    bool processingFrame();

    /**
     * @brief Get SEI from frame_ and write it to sei_data_
     *
     * @return 1 on success, 0 if there is no SEI data in the frame_ or it is of the wrong type
     */
    bool getSei();

    /**
     * @brief Function manage process from get one raw packet_
     * to emit signal to image_provider that QImage is painted
     *
     * @return 1 on success, 0 if End of File
     */
    bool play();

    /**
     * @brief Draw SEI on QImage
     */
    void drawDataOnQImage();

    /**
     * @brief Copies the array to sei_options_
     *
     * @param[in] boolean array
     */
    void copyMass(bool[12]);

    /**
     * @brief Free data before opening a new .hevc file or after closing the main
     * window of GUI
     */
    void resetVideo();

private:
    /**
     * @brief Find decoder, create and set parameters to codec context
     *
     * @return 1 on success, from -1 to -5 on failure
     */
    int setCodecCtx();

    /**
     * @brief Prepares arrays for storing frame_ and v_frame_rgb_
     *
     * @return 1 on success, 0 if failed to prepare array for frame
     */
    bool preparePictureArray();

    /**
     * @brief Counting all packets in .hevc file to calculate total frames of video
     */
    void getTotalFrames();

    /**
     * @brief Looks for the last of a sequence of key frames at the beginning of a video,
     * followed by a non-key
     */
    void findFirstKeyFrame();

    /**
     * @brief Creates QStrings with data from sei_data_ for further painting
     */
    void makeQString();

    /**
     * @brief Calculates the size of the rectangle that is drawn behind the SEI QStrings and draw it
     *
     * @param[in] current QPainter for all draw stuff
     */
    void drawBackgroundRect(QPainter *);

    /**
     * @brief Creates rectangle of tracker for further painting
     *
     * @param[in] current QPainter for all draw stuff
     */
    void drawTracker(QPainter *);

    /**
     * @brief Creates corners of rectangle of tracker for further painting
     *
     * @param[in] QPainter* current QPainter for all draw stuff
     * @param[in ] int,int,int,int is coordinates for corners
     */
    void drawCorners(QPainter *, int, int, int, int);

    /**
     * @brief Draw prepared SEI data (QStrings and tracker rectangle)
     *
     * @param[in] current QPainter for all draw stuff
     */
    void selectDataToDraw(QPainter *);

signals:
    /**
     * @brief singal from play() that QImage ready to displayed in player
     *
     * @param[out] id_stream_
     * @param[out] q_img_
     */
    void signalQImageReady(int, QImage);

public:
    /**
     * @brief One part of raw data from current .hevc file
     */
    AVPacket packet_;

    /**
     * @brief One image for Image Provider, built from a given v_frame_rgb_
     */
    QImage q_img_;

    /**
     * @brief Current stream number. 0 if there only one stream
     */
    int id_stream_;

    /**
     * @brief A total number of frames in current .hevc file
     */
    int total_frames_ = 0;

    /**
     * @brief A Format I/O context. This is where the work with ffmpeg begins
     */
    AVFormatContext *format_context_;

    /**
     * @brief FPS of current .hevc file
     */
    int fps_;

    /**
     * @brief Path and name your opened .hevc file
     */
    std::string open_file_name_;

    /**
     * @brief
     * In some .hevc video file first frame is keyframe, next 24 (or fps-1) is based on keyframe.
     * In other .hevc video file keyframe may be first AND second AND third and so on.
     *
     * In case of playing (watching video) number of first keyframe is not important,
     * but in case of saving video wrong number of first keyframe may lead to incorrect operation of video saving
     */
    int first_keyframe_;

    /**
     * @brief Struct for storage of Supplemental Enhancement Information (SEI), received from each frame
     */
    Data_sei_str *sei_data_;

    /**
     * @brief Each index refers to specific metadata: [0] for timeStr, [1] for latitude_ and so on.
     * Flag 1 - draw data, 0 - not to draw. By defaulf all 0.
     */
    bool sei_options_[12] = {0};

private:
    /**
     * @brief packet_ after decoding, native fromat
     */
    AVFrame *frame_;

    /**
     * @brief frame_ after convert to RGB
     */
    AVFrame *v_frame_rgb_;

    /** @name For decoding
    */
    //@{
    AVCodecContext *v_codec_ctx_;
    struct SwsContext *img_convert_context_;
    uint8_t *v_buffer_;
    //@}

    /** @name Relevant part of SEI data, prepared to draw on QImage_
    */
    //@{
    QString timeStr_, latitude_, longitude_, altitude_, yaw_ops_,
        pitch_ops_, yaw_bla_, pitch_bla_, roll_bla_, fov_, dist_;
    //@}
};

#endif
