#include "session.h"

#include <QDate>
#include <QDir>
#include <QString>

#include <iostream>

#define STREAM_TV 0

Session::Session(QObject *parent) : QObject(parent)
{
    camera = new ImageProvider(this);

    // hevc
    // START_HEVC_THREAD
    hevc_thread = new QThread;

    f1 = new ffmpeg(STREAM_TV);
    f1->moveToThread(hevc_thread);

    // delete threads later
    connect(hevc_thread, SIGNAL(finished()), f1, SLOT(deleteLater()));
    connect(hevc_thread, SIGNAL(finished()), hevc_thread, SLOT(deleteLater()));

    connect(f1, SIGNAL(signalQImageReady(int, QImage)), camera,
            SLOT(slotChangeQImage(int, QImage)));
}

Session::~Session()
{
    // Stop thread
    hevc_thread->quit();
    hevc_thread->wait();
}

void Session::reset() { f1->resetVideo(); }

//сократить позже
int Session::open(QUrl url)
{
    // convert file path from QUrl to std::string
    std::string file_path = url.toLocalFile().toStdString();
    std::cout << "Path: " << file_path << std::endl;
    int ret;
    ret = f1->initialization(
        file_path);	   // initialization вернет от -1 до -10 в зависимости от типа
                       // ошибки. Если ошибок не будет вернет 0
    return ret;
}

void Session::playButtonClicked()
{
    playing_status_ = PLAYING_STATUS::PLAY;
    int flag;

    while (playing_status_ == PLAYING_STATUS::PLAY)
    {
        flag = f1->play();	  //1 is ok
        if (flag == 0)
            playing_status_ = PLAYING_STATUS::PAUSE;
        qApp->processEvents();
    }
    emit pause();
}

void Session::pauseButtonClicked()
{
    std::cout << "pauseButtonClicked" << std::endl;
    playing_status_ = PLAYING_STATUS::PAUSE;
}
