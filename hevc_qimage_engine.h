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
 * @class The HevcQImageEngine is base class for opening, initialization, decoding .hevc file and painting on QImage
 */
class HevcQImageEngine : public QObject
{
    Q_OBJECT

public:
    explicit HevcQImageEngine(int id_str, QObject *parent = 0);
    ~HevcQImageEngine();

    /**
    * @brief Opening .hevc file, setting, preparation for further decoding, getting
    * additional data such a Total Frames, FPS, Duration
    *
    * @param path to chosen hevc file
    *
    * @return return 1 on success, from -1 to -5 on failure
    */
    int initialization(std::string);

    /**
     * @brief initializationPrintData outputs additional metadata in terminal afther initialization
     */
    void initializationPrintData();

    /**
     * @brief readFrame is getting one packet_ of raw data from current .hevc file for each call
     *
     * @return return 1 on success, 0 on error or end of file
     */
    bool readFrame();

    /**
     * @brief processingFrame decodes and convert the packet_ to QImage
     *
     * @return return 1 if QImage was create and write, 0 on error
     */
    bool processingFrame();

    /**
     * @brief getSei get SEI from frame_ and write it to sei_data_
     *
     * @return return 1 on success, 0 if there is no SEI data in the frame_ or it is of the wrong type
     */
    bool getSei();

    /**
     * @brief play function manage process from get one raw packet_
     * to emit signal to image_provider that QImage is painted
     *
     * @return return 1 on success, 0 if End of File
     */
    bool play();

    /**
     * @brief drawDataOnQImage draw SEI on QImage
     */
    void drawDataOnQImage();

    /**
     * @brief copyMass is copies the array to sei_options_
     *
     * @param param is boolean array
     */
    void copyMass(bool[12]);

    /**
     * @brief Free data before opening a new .hevc file or after closing the main
     * window of GUI
     */
    void resetVideo();

private:
    /**
     * @brief setCodecCtx find decoder, create and set parameters to codec context
     * @return return 1 on success, from -1 to -5 on failure
     */
    int setCodecCtx();

    /**
     * @brief preparePictureArray prepares arrays for storing frame_ and v_frame_rgb_
     * @return return 1 on success, 0 if failed to prepare array for frame
     */
    bool preparePictureArray();

    /**
     * @brief getTotalFrames is counting all packets in .hevc file to calculate total frames of video
     */
    void getTotalFrames();

    /**
     * @brief findFirstKeyFrame looks for the last of a sequence of key frames at the beginning of a video,
     * followed by a non-key
     */
    void findFirstKeyFrame();

    /**
     * @brief makeQString creates QStrings with data from sei_data_ for further painting
     */
    void makeQString();

    /**
     * @brief drawBackgroundRect calculates the size of the rectangle that is drawn behind the SEI QStrings and draw it
     *
     * @param is current QPainter for all draw stuff
     */
    void drawBackgroundRect(QPainter *);

    /**
     * @brief drawTracker creates rectangle of tracker for further painting
     *
     * @param is current QPainter for all draw stuff
     */
    void drawTracker(QPainter *);

    /**
     * @brief drawCorners creates corners of rectangle of tracker for further painting
     *
     * @param QPainter is current QPainter for all draw stuff, all int parameters is coordinates for corners
     */
    void drawCorners(QPainter *, int, int, int, int);

    /**
     * @brief selectDataToDraw draw prepared SEI data (QStrings and tracker rectangle)
     *
     * @param is current QPainter for all draw stuff
     */
    void selectDataToDraw(QPainter *);

signals:
    /**
     * @brief signalQImageReady singal from @see bool HevcQImageEngine::play() that QImage ready to displayed in player
     */
    void signalQImageReady(int, QImage);

public:
    //packet_ - пакет с данными (недекодированый фрейм), полученными из av_read_frame

    /**
     * @brief packet_ is a one part of raw data from current .hevc file
     */
    AVPacket packet_;

    /**
     * @brief q_img_ one image for Image Provider, built from a given v_frame_rgb_
     */
    QImage q_img_;

    /**
     * @brief id_stream_ current stream number. 0 if there only one stream
     */
    int id_stream_;

    /**
     * @brief total_frames_ is a total number of frames in current .hevc file
     */
    int total_frames_ = 0;

    /**
     * @brief format_context_ is a Format I/O context. This is where the work with ffmpeg begins
     */
    AVFormatContext *format_context_;

    //Additional metadata

    /**
     * @brief fps_ of current .hevc file
     */
    int fps_;

    /**
     * @brief open_file_name_ is path and name your opened .hevc file
     */
    std::string open_file_name_;

    /**
     * @brief first_keyframe_
     * In some .hevc video file first frame is keyframe, next 24 (or fps-1) is based on keyframe.
     * In other .hevc video file keyframe may be first AND second AND third and so on.
     *
     * In case of playing (watching video) number of first keyframe is not important,
     * but in case of saving video wrong number of first keyframe may lead to incorrect operation of video saving
     */
    int first_keyframe_;

    /**
     * sei_data_ struct for storage of Supplemental Enhancement Information (SEI), received from each frame
     */
    Data_sei_str *sei_data_;

    /**
     * @brief sei_options_ each index refers to specific metadata: [0] for timeStr, [1] for latitude_ and so on.
     * Flag 1 - draw data, 0 - not to draw. By defaulf all 0.
     */
    bool sei_options_[12] = {0};

private:
    AVCodecContext *v_codec_ctx_;
    struct SwsContext *img_convert_context_;
    uint8_t *v_buffer_;

    /**
     * @brief frame_ is packet_ after decoding, native fromat
     */
    AVFrame *frame_;

    /**
     * @brief v_frame_rgb_ is frame_ after convert to RGB
     */
    AVFrame *v_frame_rgb_;

    /**
     * @brief QStrings is SEI data, prepared to draw on QImage
     */
    QString timeStr_, latitude_, longitude_, altitude_, yaw_ops_,
        pitch_ops_, yaw_bla_, pitch_bla_, roll_bla_, fov_, dist_;
};

#endif
