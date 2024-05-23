#ifndef FFMPEG_H
#define FFMPEG_H

#include <QImage>
#include <QObject>
#include <pthread.h>
#include <string>

#include "sei_data.h"

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
 * @brief The ffmpeg class provides pocessing of the selected .hevc
 * video file, such as initialization, decoding, getting SEI data, navigation on video stream
 */
class ffmpeg : public QObject
{
    Q_OBJECT

public:
    /**
     * sei_data_ struct for storage of Supplemental Enhancement Information (SEI), received from each frame
     */
    Data_sei_str *sei_data_;

    /**
     * current_frame_ is a number of the last processed frame
     * total_frames_ is a total number of frames in current .hevc video file
     */
    int current_frame_ = 0;
    int total_frames_  = 0;

private:
    /**
     * packet_ is current frame of stream before decoding
     * frame_ is packet_ after decoding, native fromat
     * vFrameRGB_ is frame_ after convert to RGB
     */
    AVPacket packet_;
    AVFrame *frame_;
    AVFrame *vFrameRGB_;

    /**
     * timg_ one image for Image Provider, built from a given vFrameRGB_
     */
    QImage timg_;

    uint8_t *vbuffer_;
    int id_stream_;

    //для инициализации и декодирования
    AVCodecContext *vCodecCtx;		   //кодек
    AVFormatContext *formatContext;	   //private

    struct SwsContext *img_convert_context;	   //private

    enum class PLAYING_MODE
    {

        /**
        * Defaul mode. Decoding, processing and displaying each frame one by one
        * from start to end of file
        */
        GENERIC,

        JUMP,	 //режим перехода к фрейму без декодирования, обработки и отображения

        /**
     * Mode for counting total numbers of frames in video. Getting each
     * frame one by one without decoding, processing and displaying from start
     * to end of file
     */
        FRAMECOUNT,

        /**
     * Mode for transition from closest key frame to target frame. Decoding and
     * processing without displaying.
     */
        PROCESSING
    };

    PLAYING_MODE playing_mode_ = PLAYING_MODE::GENERIC;	   //private

    AVStream *stream_;

    // for jumping to frame
    int target_frame_;
    int closest_key_frame;
    double fps_;

public:
    explicit ffmpeg(int id_str, QObject *parent = 0);
    ~ffmpeg();
    /**
   * @brief Opening, setting, preparation for further decoding, getting
   * additional data such a Total Frames, FPS, Duration
   *
   * @param path path to chosen hevc file
   *
   * @return zero on success, from -1 to -10 on failure
   */
    int initialization(std::string path);

    /**
   * @brief Decoding, converting and getting SEI data from one frame
   *
   * @return 1 when processing of current frame is completed, 0 when the end of
   * the file is reached
   */
    int play();

    /**
   * @brief Reset before opening a new hevc file or after closing the main
   * window of GUI
   *
   */
    void resetVideo();

    void setFrame(int);

signals:
    void signalQImageReady(int, QImage, Data_sei_str *);
};

#endif	  // FFMPEG_H
