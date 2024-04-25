#include "session.h"

#include <QDate>
#include <QDir>
#include <QString>

#include <iostream>
#include <unistd.h>

#define STREAM_TV 0

Session::Session(QObject *parent) : QObject(parent)
{
    camera = new ImageProvider(this);

    f1 = new ffmpeg(STREAM_TV);

    connect(f1, SIGNAL(signalQImageReady(int, QImage)), camera,
            SLOT(slotChangeQImage(int, QImage)));

    playing_status_ = PLAYING_STATUS::PAUSE;
}

Session::~Session()
{
}

//Используется при закрытии основного окна GUI и перед открытием нового файла
void Session::reset()
{
    playing_status_ = PLAYING_STATUS::PAUSE;

    // and main thread should not wait the image_updating_thread if it was ran
    if (player_.joinable())
        player_.detach();

    f1->resetVideo();
}

//сократить позже
int Session::open(QUrl url)
{
    // convert file path from QUrl to std::string
    std::string file_path = url.toLocalFile().toStdString();
    std::cout << "Path: " << file_path << std::endl;
    int ret;
    ret = f1->initialization(
        file_path);								 // initialization вернет от -1 до -10 в зависимости от типа
                                                 // ошибки. Если ошибок не будет вернет 0
    emit totalFramesChanged(f1->totalFrames);	 //THIS
    return ret;
}

void Session::playButtonClicked()
{
    player_ = std::thread(&Session::play_thread, this);
}

void Session::play_thread()
{
    playing_status_ = PLAYING_STATUS::PLAY;

    int flag = 1;
    while (playing_status_ == PLAYING_STATUS::PLAY && flag == 1)
    {
        flag = f1->play();	  //play() return 0 if EOF
    }
    if (flag == 0)
        emit videoWasOver();
}

void Session::pauseButtonClicked()
{
    playing_status_ = PLAYING_STATUS::PAUSE;

    // And we need to wait the thread with image updating
    if (player_.joinable())
        player_.join();
}
