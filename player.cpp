#include "player.h"

Player::Player(HevcQImageEngine* obj, QObject* parent) : QObject(parent)
{
    engine_player_ = obj;
    sei_data_	   = new Data_sei_str;
}

Player::~Player()
{
    delete sei_data_;
}

void Player::setFrame(int target_frame_)
{
    if (target_frame_ == 0)
    {
        avio_seek(engine_player_->formatContext->pb, 0, SEEK_SET);
        player_current_frame_ = 0;
        engine_player_->play(show_sei_, sei_data_);
    }
    else
    {
        closest_key_frame = target_frame_ / engine_player_->fps_;
        closest_key_frame = closest_key_frame * engine_player_->fps_;
        if (closest_key_frame <= 24)
            closest_key_frame = 0;

        if (target_frame_ < player_current_frame_)
        {
            avio_seek(engine_player_->formatContext->pb, 0, SEEK_SET);
            player_current_frame_ = -1;	   //-1 потому что нумерация фреймов идет с 0
                                           //после обработки 0-го фрейма currentFrame должен стать 0,
                                           //после обработки 1го фрейма currentFrame должен стать 1 итд
        }
        if (closest_key_frame != 0)
        {
            while (player_current_frame_ != closest_key_frame - 1)
            {
                engine_player_->readFrame();
                ++player_current_frame_;
            }
        }
        while (player_current_frame_ != target_frame_ - 1)
        {
            engine_player_->readFrame();
            engine_player_->processingFrame();
            ++player_current_frame_;
        }
        engine_player_->play(show_sei_, sei_data_);
        ++player_current_frame_;
    }
}
