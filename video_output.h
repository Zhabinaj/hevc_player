#ifndef VIDEO_OUTPUT_H
#define VIDEO_OUTPUT_H

#include <string>

class VideoOutput
{
private:
    //===============SAVE VIDEO SECTION

    //to private
    enum class SAVE_MODE
    {
        SAVE_WITH_SEI,
        SAVE_WITHOUT_SEI
    };

    SAVE_MODE save_mode_;

    int width_;
    int height_;
    int channels_;
    bool output_video_stream_initialized_ = false;
    int total_frames_number_			  = 0;

public:
    /**
   * @brief Decoding, converting and getting SEI data from one frame
   *
   * @return 1 when processing of current frame is completed, 0 when the end of
   * the file is reached
   */
    void saveVideo(std::string, bool);
};

#endif	  // VIDEO_OUTPUT_H
