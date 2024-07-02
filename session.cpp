#include "session.h"

#include <QDir>
#include <QString>
#include <string>
#include <unistd.h>

#define STREAM_TV 0

Session::Session(QObject *parent) : QObject(parent)
{
    camera_			= new ImageProvider(this);
    playing_status_ = PLAYING_STATUS::PAUSE;
}

Session::~Session()
{
    delete camera_;
    delete player_;
    delete video_output_;
}

void Session::initThread(QUrl url)
{
    // convert file path from QUrl to std::string
    open_file_path_ = url.toLocalFile().toStdString();
    thread_init_	= std::thread(&Session::open, this);
    thread_init_.detach();
}

void Session::open()
{
    // int ret;
    // ret = change to try-catch
    // return ret;

    player_ = new Player(open_file_path_);
    connect(player_->engine_player_, SIGNAL(signalQImageReady(int, QImage)), camera_, SLOT(slotChangeQImage(int, QImage)));
    player_->copyMass(sei_options_s_);
    player_->engine_player_->play();

    emit totalFramesChanged(player_->engine_player_->total_frames_);
    emit currentFrameChanged(player_->player_current_frame_);
    emit initializationCompleted();
}

void Session::reset()
{
    playing_status_ = PLAYING_STATUS::PAUSE;

    if (thread_player_.joinable())
        thread_player_.detach();

    player_->engine_player_->resetVideo();

    if (video_output_ != nullptr)
        video_output_->engine_player_->resetVideo();

    delete player_;
    delete video_output_;
    video_output_ = nullptr;
}

void Session::playButtonClicked()
{
    connect(player_, SIGNAL(qImagePlayer(int, QImage)), camera_, SLOT(slotChangeQImage(int, QImage)));
    if (thread_player_.joinable())
        thread_player_.detach();
    thread_player_ = std::thread(&Session::playThread, this);
}

void Session::playThread()
{
    playing_status_ = PLAYING_STATUS::PLAY;

    int flag = 1;
    while (playing_status_ == PLAYING_STATUS::PLAY)
    {
        flag = player_->engine_player_->play();

        // 25 fps => 1 frame per 40000 microsecond
        std::this_thread::sleep_for(std::chrono::microseconds(40000));
        if (flag == 0)
        {
            emit videoWasOver();
            break;
        }

        ++(player_->player_current_frame_);

        emit currentFrameChanged(player_->player_current_frame_);

        if (nextFrameClicked)
            playing_status_ = PLAYING_STATUS::PAUSE;
    }
}

void Session::pauseButtonClicked()
{
    playing_status_ = PLAYING_STATUS::PAUSE;

    // And we need to wait the thread with image updating
    if (thread_player_.joinable())
        thread_player_.join();
}

void Session::nextFrameButtonClicked()
{
    nextFrameClicked = 1;
    playThread();
    nextFrameClicked = 0;
}

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

    //=== костыль, иначе со слайдера не работает перемещение на 1й фрейм
    if (target_frame == 1)
    {
        target_frame = 2;
        player_->setFrame(target_frame);
        prevFrameButtonClicked();
    }
    //===

    else
    {
        player_->setFrame(target_frame);
        emit currentFrameChanged(player_->player_current_frame_);
    }

    if (was_playing)
        playButtonClicked();
}

void Session::showSei(bool checked)
{
    player_->show_sei_ = checked;
}

void Session::saveThread(QUrl url, bool save_SEI)
{
    std::string save_path = url.toLocalFile().toStdString();
    video_output_		  = new VideoOutput(save_path, save_SEI);
    thread_save_		  = std::thread(&Session::saveVideo, this);
    thread_save_.detach();
}

void Session::saveVideo()
{
    connect(video_output_, SIGNAL(savingProgress(int)), this, SLOT(savingProcess(int)));
    video_output_->engine_player_->initialization(open_file_path_);
    video_output_->saveVideo();
}

void Session::savingProcess(int pop)
{
    emit savingProcessChanged(pop);
}

void Session::stopSaving()
{
    video_output_->saving = false;
}

void Session::showSei2(int ind, bool flag)
{
    sei_options_s_[ind] = flag;
    player_->copyMass(sei_options_s_);
    //записываем всё в массив
}
