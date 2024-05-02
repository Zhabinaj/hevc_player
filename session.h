#ifndef SESSION_H
#define SESSION_H

#include "ffmpeg.h"
#include "imageProvider.h"

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>
#include <thread>

class Session : public QObject
{
    Q_OBJECT
private:
    enum class PLAYING_STATUS
    {
        PLAY,
        PAUSE,
    };

    // Status of video condition: PLAY or PAUSE
    PLAYING_STATUS playing_status_;
    std::thread player_;	//поток для плеера
    bool nextFrameClicked = 0;

public:
    explicit Session(QObject *parent = 0);
    ~Session();

    ImageProvider *camera;
    ffmpeg *f1;	   //переименовать этот ужас

signals:
    void videoWasOver();
    void totalFramesChanged(int total_frames_count);
    void currentFrameChanged(int currentFrame);

public slots:
    void play_thread();
    void reset();				 // break the loop and clear all buffers
    int open(QUrl url);			 //слот для открытия по адресу
    void playButtonClicked();	 //жмякаем кнопку плэй
    void pauseButtonClicked();
    void nextFrameButtonClicked();
    void prevFrameButtonClicked();
};

#endif	  // SESSION_H
