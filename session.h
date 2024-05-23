#ifndef SESSION_H
#define SESSION_H

#include "ffmpeg.h"
#include "image_provider.h"

#include <QDebug>
#include <QObject>

#include <string>
#include <thread>

//#include "VideoOutput.h"

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
    std::thread init_;
    bool nextFrameClicked = 0;
    bool prevFrameClicked = 0;

public:
    explicit Session(QObject *parent = 0);
    ~Session();

    ImageProvider *camera;
    ffmpeg *video_file_;	//переименовать этот ужас public
    std::string file_path;
    // VideoOutput video_output_;

signals:
    void videoWasOver();
    void totalFramesChanged(int total_frames_count);
    void currentFrameChanged(int current_frame_);
    void initializationCompleted();

public slots:
    void init_thread(QUrl url);
    void play_thread();
    void reset();				 // break the loop and clear all buffers
    void open();				 //слот для открытия по адресу
    void playButtonClicked();	 //жмякаем кнопку плэй
    void pauseButtonClicked();
    void nextFrameButtonClicked();
    void prevFrameButtonClicked();
    void changeFrameFromSlider(int target_frame);
    void saveVideo(QUrl, bool);
};

#endif	  // SESSION_H
