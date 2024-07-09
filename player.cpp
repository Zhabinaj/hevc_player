#include "player.h"

Player::Player(std::string open_f, QObject* parent) : QObject(parent)
{
    open_file_path_ = open_f;
    engine_player_	= new HevcQImageEngine(0);
    engine_player_->initialization(open_file_path_);
    engine_player_->initializationPrintData();
}

Player::~Player()
{
    delete engine_player_;
}

void Player::setFrame(int target_frame)
{
    findClosestKeyFrame(target_frame);

    if (target_frame < player_current_frame_)
    {
        avio_seek(engine_player_->format_context_->pb, 0, SEEK_SET);
        //the counter will switch AFTER processing
        player_current_frame_ = 0;
    }
    if (closest_key_frame_ != 1)
    {
        while (player_current_frame_ != closest_key_frame_ - 1)
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
    engine_player_->play();

    ++player_current_frame_;
}

void Player::findClosestKeyFrame(int target)
{
    if (target % 25 == 0)
        closest_key_frame_ = (target - 1) / engine_player_->fps_;
    else
        closest_key_frame_ = target / engine_player_->fps_;

    closest_key_frame_ = (closest_key_frame_ * engine_player_->fps_) + 1;

    if (closest_key_frame_ <= 25)
        closest_key_frame_ = 1;
}
