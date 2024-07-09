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
    open_file_path_ = url.toLocalFile().toStdString();
    thread_init_	= std::thread(&Session::open, this);
    thread_init_.detach();
}

void Session::open()
{
    player_ = new Player(open_file_path_);
    player_->engine_player_->connect(player_->engine_player_, SIGNAL(signalQImageReady(int, QImage)), camera_, SLOT(slotChangeQImage(int, QImage)));
    player_->engine_player_->copyMass(sei_to_show_);
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
    if (thread_player_.joinable())
        thread_player_.detach();
    thread_player_ = std::thread(&Session::play, this);
}

void Session::play()
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

        if (next_frame_clicked_)
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
    next_frame_clicked_ = 1;
    play();
    next_frame_clicked_ = 0;
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

    /* When moving from frame 2 to frame 1 using prevFrameButtonClicked(), everything works correctly.
     * When moving from frame 2 to frame 1 using slyder first frame not displayed correctly.
     * Could not find the cause of the error, this is a workaround.
     */
    if (target_frame == 1)
    {
        target_frame = 2;
        player_->setFrame(target_frame);
        prevFrameButtonClicked();
    }

    else
    {
        player_->setFrame(target_frame);
        emit currentFrameChanged(player_->player_current_frame_);
    }

    if (was_playing)
        playButtonClicked();
}

void Session::saveThread(QUrl url)
{
    save_path_	 = url.toLocalFile().toStdString();
    thread_save_ = std::thread(&Session::saveVideo, this);
    thread_save_.detach();
}

void Session::saveVideo()
{
    video_output_ = new VideoOutput(save_path_, open_file_path_);
    video_output_->engine_player_->copyMass(sei_to_save_);
    connect(video_output_, SIGNAL(savingProgress(int)), this, SLOT(savingProcess(int)));
    video_output_->saveVideo();
}

void Session::savingProcess(int percent)
{
    emit savingProcessChanged(percent);
}

void Session::stopSaving()
{
    video_output_->saving_ = false;
}

void Session::seiToShow(int ind, bool flag)
{
    sei_to_show_[ind] = flag;
    if (player_ != nullptr)
        player_->engine_player_->copyMass(sei_to_show_);
}

void Session::seiToSave(int ind, bool flag)
{
    sei_to_save_[ind] = flag;
}
