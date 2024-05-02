#ifndef FFMPEG_H
#define FFMPEG_H

#include <QObject>
#include <QtGui>
#include <pthread.h>
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

struct AVCodecContext;
struct AVFrame;

class ffmpeg : public QObject
{
    Q_OBJECT
public:
    explicit ffmpeg(int id_str, QObject *parent = 0);
    ~ffmpeg();

    AVFrame *vFrameRGB;
    uint8_t *vbuffer;
    int id_stream;
    QImage timg;

    double time_base_video;

    //для инициализации
    AVCodecContext *codecContext;	 //для создание контекста кодека
    AVCodecContext *vCodecCtxOrig;
    AVCodecContext *vCodecCtx;	  //кодек
    AVFormatContext *formatContext;
    AVPacket packet;
    AVFrame *frame;
    int videoStreamIndex;
    struct SwsContext *img_convert_context;
    //for slide and navigation
    AVStream *stream_{};
    int totalFrames = 0;
    int targetFrame;
    int currentFrame  = 1;
    bool setFrameMode = 0;

    /* для скорости воспроизведения
    int64_t frame_timer;
    int64_t frame_last_delay = 40000;
    int64_t frame_pts_ms = 0, frame_last_pts = 0;
    int64_t actual_delay = 0, delay = 0;
    */
    int initialization(std::string path);	 //открытие, настройка, т.е. вся хурма для подготовки вытаскивания, декодирования и выплевывания фрейма

    int play();	   //обработка и выплевывание одного фрейма, возвращает 0, когда видео закончилось

    void resetVideo();	  //сброс всего перед открытием нового файла или после закрытия окна GUI

    void setFrame(int);

signals:
    void signalQImageReady(int, QImage);
};

#endif	  // FFMPEG_H

