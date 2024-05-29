#ifndef SESSION_H
#define SESSION_H

#include "image_provider.h"

#include "hevc_qimage_engine.h"
#include "player.h"

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
    bool nextFrameClicked = 0;
    bool prevFrameClicked = 0;

public:
    explicit Session(QObject *parent = 0);
    ~Session();

    //class for showing frame in player
    ImageProvider *camera_;

    //common part for player and saver
    HevcQImageEngine *hevc_engine_;

    std::string open_file_path_;

    Player *player_;

signals:
    void videoWasOver();
    void totalFramesChanged(int total_frames_count);
    void currentFrameChanged(int current_frame_);
    void initializationCompleted();

private:
    void open();	//слот для открытия по адресу

public slots:
    void init_thread(QUrl url);

    void play_thread();
    void reset();	 // break the loop and clear all buffers

    void showSei(bool checked);

    void playButtonClicked();	 //жмякаем кнопку плэй
    void pauseButtonClicked();
    void nextFrameButtonClicked();
    void prevFrameButtonClicked();
    void changeFrameFromSlider(int target_frame);
    // void saveVideo(QUrl, bool);
};

#endif	  // SESSION_H
