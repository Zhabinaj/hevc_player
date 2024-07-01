#ifndef HEVCQIMAGEENGINE_H
#define HEVCQIMAGEENGINE_H

#include <QImage>
#include <QObject>

#include <sei_data.h>
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

/**
 * @brief The HevcQImageEngine class for opening, initialization, decoding and painting .hevc file
 */

class HevcQImageEngine : public QObject
{
    Q_OBJECT

public:
    explicit HevcQImageEngine(int id_str, QObject *parent = 0);
    ~HevcQImageEngine();

    /**
    * @brief Opening, setting, preparation for further decoding, getting
    * additional data such a Total Frames, FPS, Duration
    *
    * @param path path to chosen hevc file
    *
    * @return zero on success, from -1 to -5 on failure
    */
    int initialization(std::string);

    //вытягиваем по 1 фрейму за раз
    //возвращ 0 когда достигнуть конец файла
    bool readFrame();

    //вернет 1 если создан QImage, 0 если не удалось
    bool processingFrame();	   //private?

    //получаем sei из фрейма и записываем в ту структуру, которая передается через параметры
    //вернет 1 если всё ок
    bool getSei();	  //private?

    void drawDataOnFrame();
    void drawCorners(QPainter *, int, int, int, int);

    /**
   * @brief Reset before opening a new hevc file or after closing the main
   * window of GUI
   *
   */
    void resetVideo();

    //return 1 when ok, 0 when EOF
    bool play(bool);

private:
    //for initialization

    //для инициализации и декодирования
    AVCodecContext *vCodecCtx;				   //кодек
    struct SwsContext *img_convert_context;	   //private
    uint8_t *vbuffer_;

    /**
     * packet_ is current frame of stream before decoding
     * frame_ is packet_ after decoding, native fromat
     * vFrameRGB_ is frame_ after convert to RGB
     */
    AVFrame *frame_;
    AVFrame *vFrameRGB_;

public:
    //packet_ - пакет с данными (недекодированый фрейм), полученными из av_read_frame
    AVPacket packet_;
    /**
     * q_img_ one image for Image Provider, built from a given vFrameRGB_
     */
    QImage q_img_;

    int id_stream_;

    //total_frames_ is a total number of frames in current .hevc video file
    int total_frames_ = 0;

    //для инициалищации и декодирования
    AVFormatContext *formatContext;	   //public

    //Additional metadata
    int fps_;						//may use for save?
    std::string open_file_name_;	//Имя открытого HEVC файла
    int first_keyframe_;
    /**
     * sei_data_ struct for storage of Supplemental Enhancement Information (SEI), received from each frame
     */
    Data_sei_str *sei_data_;

signals:
    void signalQImageReady(int, QImage);

private:
    //вернет 1 если всё ок, от -1 до -5 если ошибка
    int setCodecCtx();
    // подготавливем массивы для хранения изображения frame_ и vFrameRGB_
    // вернет 0 если не удалось подготовить массив для frame_
    bool preparePictureArray();

    //вывод данных, полученных во время инициализации
    void initializationPrintData();
    void getTotalFrames();
    void findFirstKeyFrame();
};

#endif	  // HEVCQIMAGEENGINE_H
