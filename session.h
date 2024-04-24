#ifndef SESSION_H
#define SESSION_H

#include "ffmpeg.h"
#include "imageProvider.h"

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>

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

public:
    explicit Session(QObject *parent = 0);
    ~Session();

    ImageProvider *camera;

    QThread *hevc_thread;
    QTimer updateWorkParamTimer;
    ffmpeg *f1;	   //переименовать этот ужас

signals:
    void pause();

public slots:
    void reset();				 // break the loop and clear all buffers
    int open(QUrl url);			 //слот для открытия по адресу
    void playButtonClicked();	 //жмякаем кнопку плэй
    void pauseButtonClicked();
};

#endif	  // SESSION_H
