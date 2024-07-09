#ifndef SESSION_H
#define SESSION_H

#include "image_provider.h"
#include "player.h"
#include "video_output.h"

#include <QDebug>
#include <QObject>
#include <string>
#include <thread>

/**
 * @brief Class - connector between GUI and backend
 */
class Session : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor creates a ImageProvider object and set PLAYING_STATUS::PAUSE
     * @param[in] parent needed to access the QT framework functionality
     */
    explicit Session(QObject *parent = 0);

    /**
     * @brief Freeing memory of camera_, player_, video_output_;
     */
    ~Session();

    /**
     * @brief Opens a video file and changes GUI after initialization
     */
    void open();

    /**
     * @brief Saves the currently open video
     */
    void saveVideo();

public slots:
    /**
     * @brief Сreates a separate thread in which it runs the open()
     * @param[in] url is a full path of the file to open from GUI
     */
    void initThread(QUrl url);

    /**
     * @brief Сreates a separate thread in which it runs the saveVideo()
     * @param[in] url is a path to the folder where the current open video will be saved
     */
    void saveThread(QUrl);

    /**
     * @brief When clicked Playback button in GUI creates a separate thread which it runs the play()
     */
    void playButtonClicked();

    /**
     * @brief Plays in GUI the current video from start to finish and updates number of current frame
     */
    void play();

    /**
     * @brief Resets GUI, backend settings, frees up memory before opening a new video
     */
    void reset();

    /**
     * @brief Stops loop in play() when clicked Playback button (PAUSE) in GUI
     */
    void pauseButtonClicked();

    /**
     * @brief When clicked Next Frame button in GUI jump video to next frame
     */
    void nextFrameButtonClicked();

    /**
     * @brief When clicked Previous Frame button in GUI jump video to previous frame
     */
    void prevFrameButtonClicked();

    /**
     * @brief Rewind video to a specific frame selected via the slider in GUI
     *
     * @param[in] target_frame is number of frame we want to jump to
     */
    void changeFrameFromSlider(int target_frame);

    /**
     * @brief Get information about saving progress from video_output_ and sent it to GUI
     * @param[in] percent video saving percentage
     */
    void savingProcess(int percent);

    /**
     * @brief Stops saving when requested from GUI
     */
    void stopSaving();

    /**
     * @brief Gets from GUI selection wich SEI need to display.
     * @param[in] int specific SEI number
     * @param[in] bool true - show, false - do not show
     */
    void seiToShow(int, bool);

    /**
     * @brief Gets from GUI selection wich SEI need to save.
     * @param[in] int specific SEI number
     * @param[in] bool true - save, false - do not save
     */
    void seiToSave(int, bool);

signals:
    /**
     * @brief Signal to the GUI that the end of the file has been reached
     */
    void videoWasOver();

    /**
     * @brief Updating GUI after initialization
     */
    void initializationCompleted();

    /**
     * @brief Updating GUI after initialization
     * @param[out] total_frames_count same as total_frames_ @see HevcQImageEngine::total_frames_
     */
    void totalFramesChanged(int total_frames_count);

    /**
     * @brief Updating GUI after every processed frame
     * @param[out] current_frame_ same as player_current_frame_ @see Player::player_current_frame_
     */
    void currentFrameChanged(int current_frame_);

    /**
     * @brief Updating GUI after every processed frame while saving
     * @param[out] percent video saving percentage @see savingProcess()
     */
    void savingProcessChanged(int percent);

public:
    /**
     * @brief New object dynamically created in Session() @see ImageProvider
     */
    ImageProvider *camera_;

    /**
     * @brief New object dynamically created in open() @see Player
     */
    Player *player_ = nullptr;

    /**
     * @brief New object dynamically created in saveVideo() @see VideoOutput
     */
    VideoOutput *video_output_ = nullptr;

    /**
     * @brief Full path to the open file
     */
    std::string open_file_path_;

    /**
     * @brief Path to the folder where the current open video will be saved
     */
    std::string save_path_;

    /**
     * @brief Each index refers to specific metadata: [0] for timeStr, [1] for latitude_ and so on.
     * Flag 1 - draw data, 0 - not to draw.
     * Stores the selection for the next open video
     * @see seiToShow()
     */
    bool sei_to_show_[12] = {0};

    /**
     * @brief Each index refers to specific metadata: [0] for timeStr, [1] for latitude_ and so on.
     * Flag 1 - save data, 0 - not to save.
     * Stores the selection for the next saved video
     * @see seiToSave()
     */
    bool sei_to_save_[12] = {0};

private:
    /**
     * @brief Represents the current status of the playback stratus. May use bool, but enum is more noticeable in the code
     */
    enum class PLAYING_STATUS
    {
        PLAY,
        PAUSE,
    };

    /**
     * @brief @see PLAYING_STATUS
     */
    PLAYING_STATUS playing_status_;

    /**
     * @brief For run play() onece, not in loop
     */
    bool next_frame_clicked_ = 0;

    /**
     * @brief Thread for player
     */
    std::thread thread_player_;

    /**
     * @brief Thread for initialization while video file opens
     */
    std::thread thread_init_;

    /**
     * @brief Thread for saving
     */
    std::thread thread_save_;
};

#endif
