#ifndef PLAYER_H
#define PLAYER_H

#include "hevc_qimage_engine.h"
#include <QObject>
/**
 * @brief The Player class ...go to frame, image privider
 */

class Player : public QObject
{
    Q_OBJECT
public:
    HevcQImageEngine* engine_player_;

    // player_current_frame_ is a number of the last processed frame
    int player_current_frame_ = 0;

    // for jumping to frame
    int closest_key_frame;

    std::string open_file_path_;

    bool show_sei_ = 0;

    /**
     * img_ one image for Image Provider, built from a given vFrameRGB_
     */
    QImage img_;

public:
    explicit Player(std::string, QObject* parent = 0);
    ~Player();

    void setFrame(int);

signals:
    void qImagePlayer(int, QImage);
};

#endif	  // PLAYER_H
