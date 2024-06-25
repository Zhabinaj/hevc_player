#include "player.h"

Player::Player(std::string open_f, QObject* parent) : QObject(parent)
{
    open_file_path_ = open_f;
    engine_player_	= new HevcQImageEngine(0);
    engine_player_->initialization(open_file_path_);
}

Player::~Player()
{
    delete engine_player_;
}

void Player::findClosestKeyFrame(int target)
{
    closest_key_frame = target / engine_player_->fps_;
    closest_key_frame = (closest_key_frame * engine_player_->fps_) + 1;

    if (closest_key_frame <= 25)
        closest_key_frame = 1;
}

void Player::setFrame(int target_frame)
{
    findClosestKeyFrame(target_frame);

    if (target_frame < player_current_frame_)
    {
        avio_seek(engine_player_->formatContext->pb, 0, SEEK_SET);
        player_current_frame_ = 0;	  //счетчик переключится ПОСЛЕ обработки и выплевывания фрейма
    }
    if (closest_key_frame != 1)
    {
        while (player_current_frame_ != closest_key_frame - 1)
        {
            engine_player_->readFrame();
            av_packet_unref(&engine_player_->packet_);
            ++player_current_frame_;
        }
    }
    while (player_current_frame_ != target_frame - 1)
    {
        engine_player_->readFrame();
        engine_player_->processingFrame();
        ++player_current_frame_;
    }
    engine_player_->play(show_sei_);
    ++player_current_frame_;
}
