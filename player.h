#ifndef PLAYER_H
#define PLAYER_H

#include "hevc_qimage_engine.h"
#include <QObject>

/**
 * @brief Provides player functionality such as video playback, pause and rewind
 */
class Player : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Сonstructor creates a HevcQImageEngine object of file we want to open,
     * prepares it for working with the player and outputs additional metadata to the terminal/console
     *
     * @param[in] string is a full path to the file to open
     * @param[in] parent needed to access the QT framework functionality
     */
    explicit Player(std::string, QObject* parent = 0);

    /**
     * @brief freeing memory of engine_player_;
     */
    ~Player();

    /**
     * @brief Rewinds the player to the number of the desired frame
     *
     * @param[in] int number of the frame we want to go jump
     */
    void setFrame(int);

private:
    /**
     * @brief Only every 25th frame (because 25 FPS) has complete information about the frame,
     * the next 24 are built on its basis.
     * To correctly display the frame to which we are jumping,
     * we need to find the previous closest key frame
     *
     * @param[in] int number of the frame we want to go jump
     */
    void findClosestKeyFrame(int);

public:
    /**
     * @brief Class for working with a .hevc video file through a ffmpeg library
     */
    HevcQImageEngine* engine_player_;

    /**
     * @brief Number of the last processed frame. Frame counting starts from 1.
     */
    int player_current_frame_ = 1;

    /**
     * @brief From this frame begins processingFrame() when rewinding
     */
    int closest_key_frame_;

    /**
     * @brief Full path to the file to open
     */
    std::string open_file_path_;
};

#endif
