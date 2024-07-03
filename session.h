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

    Player *player_ = nullptr;

    VideoOutput *video_output_ = nullptr;

    bool sei_options_s_[12] = {0};
    bool sei_to_save_[12]	= {0};

signals:
    void videoWasOver();
    void totalFramesChanged(int total_frames_count);
    void currentFrameChanged(int current_frame_);
    void initializationCompleted();
    void savingProcessChanged(int percent);

private:
    void open();	//слот для открытия по адресу
    void saveVideo();

public slots:
    void initThread(QUrl url);

    void playThread();

    //Используется при закрытии основного окна GUI и перед открытием нового файла clear all buffers
    void reset();

    void playButtonClicked();	 //жмякаем кнопку плэй
    void pauseButtonClicked();
    void nextFrameButtonClicked();
    void prevFrameButtonClicked();
    void changeFrameFromSlider(int target_frame);
    void saveThread(QUrl);
    void savingProcess(int pop);
    void stopSaving();
    void seiToShow(int, bool);
    void seiToSave(int, bool);
};

#endif	  // SESSION_H
