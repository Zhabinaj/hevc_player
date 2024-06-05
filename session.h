#ifndef SESSION_H
#define SESSION_H

#include "image_provider.h"

#include "hevc_qimage_engine.h"
#include "player.h"
#include "video_output.h"

#include <QDebug>
#include <QObject>

#include <string>
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
    std::thread thread_player_;	   //поток для плеера
    std::thread thread_init_;	   //поток для инициализации
    std::thread thread_save_;
    bool nextFrameClicked = 0;
    bool prevFrameClicked = 0;

public:
    explicit Session(QObject *parent = 0);
    ~Session();

    //class for showing frame in player
    ImageProvider *camera_;

    std::string open_file_path_;	//перевести в класс hevc_qimage_engine

    Player *player_;

    VideoOutput *video_output_ = nullptr;

signals:
    void videoWasOver();
    void totalFramesChanged(int total_frames_count);
    void currentFrameChanged(int current_frame_);
    void initializationCompleted();

private:
    void open();	//слот для открытия по адресу
    void saveVideo();

public slots:
    void initThread(QUrl url);

    void playThread();
    void reset();	 // break the loop and clear all buffers

    void showSei(bool checked);

    void playButtonClicked();	 //жмякаем кнопку плэй
    void pauseButtonClicked();
    void nextFrameButtonClicked();
    void prevFrameButtonClicked();
    void changeFrameFromSlider(int target_frame);
    void saveThread(QUrl, bool);
};

#endif	  // SESSION_H
