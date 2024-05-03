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

    enum class PLAYING_MODE
    {
        GENERIC,
        JUMP,
        FRAMECOUNT
    };

    PLAYING_MODE playing_mode_ = PLAYING_MODE::GENERIC;	   //по умолчанию обычный режим

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

    //metadata
    int totalFrames_ = 0;
    double fps_;
    int time_ms_;	  //сейчас не используется, но потом возможно пригодится
    int duration_;	  //сейчас не используется, но потом возможно пригодится

    int currentFrame  = 1;
    bool setFrameMode = 0;
    int targetFrame;
    bool jumpToFrame = 0;	 //0 если обычный режим проигрывания, 1 если режим прыжка

    int initialization(std::string path);	 //открытие, настройка, т.е. вся хурма для подготовки вытаскивания, декодирования и выплевывания фрейма

    int play();	   //обработка и выплевывание одного фрейма, возвращает 0, когда видео закончилось

    void resetVideo();	  //сброс всего перед открытием нового файла или после закрытия окна GUI

    void setFrame(int);

signals:
    void signalQImageReady(int, QImage);
};

#endif	  // FFMPEG_H
