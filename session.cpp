#include "session.h"

#include <QDir>
#include <QString>
#include <string>
#include <unistd.h>

#include <chrono>
#include <thread>

#define STREAM_TV 0

Session::Session(QObject *parent) : QObject(parent)
{
    camera_ = new ImageProvider(this);

    hevc_engine_ = new HevcQImageEngine(STREAM_TV);

    connect(hevc_engine_, SIGNAL(signalQImageReady(int, QImage)), camera_, SLOT(slotChangeQImage(int, QImage)));

    playing_status_ = PLAYING_STATUS::PAUSE;
}

Session::~Session()
{
    delete camera_;
    delete hevc_engine_;
    delete player_;
}

void Session::init_thread(QUrl url)
{
    // convert file path from QUrl to std::string
    open_file_path_ = url.toLocalFile().toStdString();
    thread_init_	= std::thread(&Session::open, this);
    thread_init_.detach();
}

void Session::open()
{
    // int ret;)
    // ret = change to try-catch))
    hevc_engine_->initialization(open_file_path_);
    // return ret;

    player_ = new Player(hevc_engine_);
    player_->engine_player_->play(player_->show_sei_, player_->sei_data_);

    emit totalFramesChanged(player_->engine_player_->total_frames_);
    emit initializationCompleted();
}

//Используется при закрытии основного окна GUI и перед открытием нового файла
void Session::reset()
{
    playing_status_ = PLAYING_STATUS::PAUSE;

    // and main thread should not wait the image_updating_thread if it was ran
    if (thread_player_.joinable())
        thread_player_.detach();

    hevc_engine_->resetVideo();
    //добавить деструктор для Play и Save?
    delete player_;
}

void Session::playButtonClicked()
{
    connect(player_, SIGNAL(qImagePlayer(int, QImage)), camera_, SLOT(slotChangeQImage(int, QImage)));
    thread_player_ = std::thread(&Session::play_thread, this);
}

void Session::play_thread()
{
    playing_status_ = PLAYING_STATUS::PLAY;

    int flag = 1;
    while (playing_status_ == PLAYING_STATUS::PLAY)
    {
        flag = player_->engine_player_->play(player_->show_sei_, player_->sei_data_);
        std::this_thread::sleep_for(std::chrono::microseconds(40000));	  // 25 fps => 1 frame per 40000 microsecond в первый раз не надо
        if (flag == 0)
        {
            emit videoWasOver();
            break;
        }
        ++(player_->player_current_frame_);

        emit currentFrameChanged(player_->player_current_frame_);

        if (nextFrameClicked)
            break;
    }
}

void Session::pauseButtonClicked()
{
    playing_status_ = PLAYING_STATUS::PAUSE;

    // And we need to wait the thread with image updating
    if (thread_player_.joinable())
        thread_player_.join();
}

//======================OK
void Session::nextFrameButtonClicked()
{
    nextFrameClicked = 1;
    playButtonClicked();
    pauseButtonClicked();
    nextFrameClicked = 0;
}
//=====================

void Session::prevFrameButtonClicked()
{
    player_->setFrame(player_->player_current_frame_ - 1);
    emit currentFrameChanged(player_->player_current_frame_);
}

void Session::changeFrameFromSlider(int target_frame)
{
    bool was_playing = false;
    if (playing_status_ == PLAYING_STATUS::PLAY)
    {
        pauseButtonClicked();
        was_playing = true;
    }
    player_->setFrame(target_frame);
    emit currentFrameChanged(player_->player_current_frame_);
    if (was_playing)
        playButtonClicked();
}

//переводим плеер в режим показа  sei
void Session::showSei(bool checked)
{
    player_->show_sei_ = checked;
}
/*
void Session::saveVideo(QUrl url, bool save_SEI)
{

    //тут создаем объект класса для сохранения

    std::string save_file_path = url.toLocalFile().toStdString();

    video_file_->saveVideo(save_file_path, save_SEI);

    //url адрес сохранения

    //поставить на паузу?
}
*/
