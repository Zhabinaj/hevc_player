#include "session.h"

#include <QDir>
#include <QString>
#include <iostream>
#include <string>
#include <unistd.h>

#define STREAM_TV 0

Session::Session(QObject *parent) : QObject(parent)
{
    camera = new ImageProvider(this);

    video_file_ = new ffmpeg(STREAM_TV);

    connect(video_file_, SIGNAL(signalQImageReady(int, QImage, Data_sei_str *)), camera,
            SLOT(slotChangeQImage(int, QImage, Data_sei_str *)));

    playing_status_ = PLAYING_STATUS::PAUSE;
}

Session::~Session()
{
    delete camera;
    delete video_file_;
}

//Используется при закрытии основного окна GUI и перед открытием нового файла
void Session::reset()
{
    playing_status_ = PLAYING_STATUS::PAUSE;

    // and main thread should not wait the image_updating_thread if it was ran
    if (player_.joinable())
        player_.detach();

    video_file_->resetVideo();
}

void Session::open()
{
    // int ret;
    // ret = change to try-catch
    video_file_->initialization(file_path);
    emit totalFramesChanged(video_file_->total_frames_);
    emit initializationCompleted();
    // return ret;
}

void Session::init_thread(QUrl url)
{
    // convert file path from QUrl to std::string
    file_path = url.toLocalFile().toStdString();
    init_	  = std::thread(&Session::open, this);
    init_.detach();
}

void Session::playButtonClicked()
{
    player_ = std::thread(&Session::play_thread, this);
}

void Session::play_thread()
{
    playing_status_ = PLAYING_STATUS::PLAY;

    int flag = 1;
    while (playing_status_ == PLAYING_STATUS::PLAY)
    {
        flag = video_file_->play();
        if (flag == 0)
        {
            emit videoWasOver();
            break;
        }
        ++(video_file_->current_frame_);

        emit currentFrameChanged(video_file_->current_frame_);

        if (nextFrameClicked)
            break;
    }
}

void Session::pauseButtonClicked()
{
    playing_status_ = PLAYING_STATUS::PAUSE;

    // And we need to wait the thread with image updating
    if (player_.joinable())
        player_.join();
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
    video_file_->setFrame(video_file_->current_frame_ - 1);
    emit currentFrameChanged(video_file_->current_frame_);
}

void Session::changeFrameFromSlider(int target_frame)
{
    bool was_playing = false;	 //
    if (playing_status_ == PLAYING_STATUS::PLAY)
    {
        pauseButtonClicked();
        was_playing = true;
    }
    video_file_->setFrame(target_frame);
    emit currentFrameChanged(video_file_->current_frame_);
    if (was_playing)
        playButtonClicked();
}

void Session::saveVideo(QUrl url, bool save_SEI)
{

    std::string save_file_path = url.toLocalFile().toStdString();
    std::cout << "save path: " << save_file_path << std::endl;
    std::cout << "save option: " << save_SEI << std::endl;

    //url адрес сохранения
    /*
    if (save_SEI)
        video_file_.save_mode_ = SAVE_WITH_SEI;
    else
        video_file_.save_mode_ = SAVE_SAVE_WITHOUT_SEI_SEI;

*/
    //поставить на паузу

    // std::unique_lock<std::mutex> lck(imageMutexWriteImage);

    //переводим QIMAGE image_to_save в ImageViev imageBufferToWrite
    // parseQimageForImage(image_to_save, imageBufferToWrite);
    // lck.unlock();

    //запуск
    //video_output_ заменить на video_file_
    //video_output_->writeFrame(imageBufferToWrite, videoSource_->getPlayingVideoName(), videoSource_->getCurrentFrameNum());
}
