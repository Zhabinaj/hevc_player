#include <QFile>
#include <chrono>
#include <iostream>
#include <thread>

#include "ffmpeg.h"
#include "string.h"

ffmpeg::ffmpeg(int id_str, QObject *parent) : QObject(parent)
{
    id_stream_ = id_str;
    vFrameRGB_ = 0;
    vbuffer_   = NULL;
    sei_data_  = new Data_sei_str;
}

ffmpeg::~ffmpeg()
{
    // Free the RGB image
    av_frame_free(&vFrameRGB_);
    av_free(vbuffer_);
    delete sei_data_;
}

int ffmpeg::initialization(std::string path)
{

    av_register_all();	  // Регистрация всех доступных форматов и кодеков

    const char *filename = path.c_str();	//Имя вашего HEVC файла
    std::cout << "=============== Filename: " << filename
              << " ===============" << std::endl;

    //===================================================Обработка ошибок: возврат
    //от -1 до -9
    formatContext = avformat_alloc_context();
    if (!formatContext)
        return -1;

    // Открываем файл
    if (avformat_open_input(&formatContext, filename, nullptr, nullptr) != 0)
        return -2;

    // Получаем информацию о потоке
    if (avformat_find_stream_info(formatContext, nullptr) < 0)
        return -3;

    // Находим видео поток
    vCodecCtx			= nullptr;
    AVCodec *vcodec		= nullptr;
    img_convert_context = nullptr;

    std::cout << "=============== Number of streams: "
              << formatContext->nb_streams
              << " ===============" << std::endl;

    if (formatContext->streams[id_stream_]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
        return -4;

    // Получаем декодер для кодека видео потока
    vcodec = avcodec_find_decoder(formatContext->streams[id_stream_]->codecpar->codec_id);

    // Декодер не найден
    if (!vcodec)
        return -5;

    vCodecCtx = avcodec_alloc_context3(vcodec);	   // Создаем контекст кодека

    if (!vCodecCtx)
        return -6;

    // Настраиваем контекст кодека
    if (avcodec_parameters_to_context(
            vCodecCtx, formatContext->streams[id_stream_]->codecpar) < 0)
        return -7;

    // Открываем кодек
    if (avcodec_open2(vCodecCtx, vcodec, nullptr) < 0)
        return -8;

    img_convert_context = sws_getCachedContext(
        NULL, vCodecCtx->width, vCodecCtx->height, vCodecCtx->pix_fmt,
        vCodecCtx->width, vCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL,
        NULL, NULL);

    if (vbuffer_ != 0)
        av_free(vbuffer_);

    // Determine required buffer size and allocate buffer
    int numBytes =
        avpicture_get_size(AV_PIX_FMT_RGB24, vCodecCtx->width, vCodecCtx->height);
    vbuffer_ = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    if (vFrameRGB_ != NULL)
        av_frame_free(&vFrameRGB_);
    vFrameRGB_ = av_frame_alloc();
    avpicture_fill((AVPicture *)vFrameRGB_, vbuffer_, AV_PIX_FMT_RGB24,
                   vCodecCtx->width, vCodecCtx->height);

    std::cout << "=============== Time base video codec tbc: "
              << vCodecCtx->time_base.num << "/" << vCodecCtx->time_base.den
              << " ===============" << std::endl;
    std::cout << "=============== Time base video codec tbn: "
              << formatContext->streams[id_stream_]->time_base.num << "/"
              << formatContext->streams[id_stream_]->time_base.den
              << " ===============" << std::endl;

    frame_ = av_frame_alloc();
    avpicture_fill((AVPicture *)frame_, vbuffer_, AV_PIX_FMT_RGB24,
                   vCodecCtx->width, vCodecCtx->height);
    if (!frame_)
        return -9;

    av_init_packet(&packet_);

    // выбираем наш поток  заданным id = 0
    //(установлено в момент создания ffmpeg)
    stream_ = formatContext->streams[id_stream_];

    fps_ = stream_->avg_frame_rate.num / stream_->avg_frame_rate.den;
    std::cout << "=============== FPS: " << fps_
              << " ===============" << std::endl;

    //=================Прогоняем видос, считаем кол-во фреймов

    playing_mode_ = PLAYING_MODE::FRAMECOUNT;

    int end_of_file = 1;
    while (end_of_file)
    {
        end_of_file = play();
        ++total_frames_;
    }
    --total_frames_;	//из-за последнего входа в цикл, когда файл пуст
    playing_mode_ = PLAYING_MODE::GENERIC;
    //===========================================================

    std::cout << "=============== Total Frames: " << total_frames_
              << " ===============" << std::endl;

    double seconds = total_frames_ / fps_;
    int minutes	   = seconds / 60;
    seconds		   = seconds - (60 * minutes);
    int hours	   = minutes / 60;
    minutes		   = minutes - (hours * 60);

    std::cout << "=============== Time: " << ((hours < 10) ? ("0") : (""))
              << hours << ":" << ((minutes < 10) ? ("0") : ("")) << minutes << ":"
              << seconds << " ===============" << std::endl;

    avio_seek(formatContext->pb, 0,
              SEEK_SET);	//при всех 0 сбрасывается на начало видео

    play();
    return 0;
}

void ffmpeg::resetVideo()
{
    total_frames_  = 0;
    current_frame_ = 0;
    av_packet_unref(&packet_);
    av_frame_free(&frame_);
    avcodec_free_context(&vCodecCtx);
    avformat_close_input(&formatContext);
}

int ffmpeg::play()
{
    /* функция делит содержимой файла formatContext на фреймы
   * и возвроащает по одному через packet_ за каждый вызов
   * В packet_ как-то должна хранится ссылка на следующий фрейм
   */
    int ret_read_frame = av_read_frame(formatContext, &packet_);

    //=====Проверка на конец файла
    if (ret_read_frame < 0)
    {
        if (playing_mode_ != PLAYING_MODE::FRAMECOUNT)
            std::cout << "=============== End Of File ===============" << std::endl;
        return 0;
    }
    //==============

    if (playing_mode_ == PLAYING_MODE::GENERIC ||
        playing_mode_ == PLAYING_MODE::PROCESSING)
    {
        if (packet_.stream_index == id_stream_)
        {
            int frame_finished;

            /*
       *  vCodecCtx - кодек
       *  frame_ - фрейм packet_ после декодирования
       *  frame_finished - флаг, не 0 если фрейм декодирован
       *  packet_ - пакет с данными (недекодированый фрейм), полученными из
       * av_read_frame
       */
            avcodec_decode_video2(vCodecCtx, frame_, &frame_finished, &packet_);
            if (frame_finished)
            {
                // Convert the image from its native format to RGB
                sws_scale(img_convert_context, frame_->data, frame_->linesize, 0,
                          vCodecCtx->height, vFrameRGB_->data, vFrameRGB_->linesize);

                timg_ = QImage((uchar *)vFrameRGB_->data[0], vCodecCtx->width,
                               vCodecCtx->height, QImage::Format_RGB888);
                //===================SEI
                if (frame_->nb_side_data > 0)
                {
                    AVFrameSideData *sd = frame_->side_data[0];
                    if (sd->type == AV_FRAME_DATA_SEI_UNREGISTERED)
                    {
                        memcpy(sei_data_, sd->data, sizeof(Data_sei_str));
                    }
                }

                if (playing_mode_ == PLAYING_MODE::GENERIC)
                {
                    emit signalQImageReady(id_stream_, timg_, sei_data_);
                    std::this_thread::sleep_for(std::chrono::microseconds(
                        40000));	// 25 fps => 1 frame per 40000 microseconds
                }
                //if save mode
                // конвертируем QImage
                const auto channels = static_cast<uint64_t>(timg_.format() == QImage::Format_RGB888 ? 3 : timg_.format() == QImage::Format_Grayscale8 ? 1 : 0);
                const auto width	= static_cast<uint64_t>(timg_.width());
                const auto height	= static_cast<uint64_t>(timg_.height());

                const uint64_t stride = width * channels * sizeof(uint8_t);

                /*
                 *
                 *
                 *
                 *
                 * */
            }
        }
    }
    av_free_packet(&packet_);
    return 1;
};

void ffmpeg::setFrame(int target_frame_)
{

    // int new_current_frame = current_frame_;
    if (target_frame_ == 0)
    {
        avio_seek(formatContext->pb, 0, SEEK_SET);
        current_frame_ = 0;
        play();
    }
    else
    {
        playing_mode_ = PLAYING_MODE::JUMP;

        closest_key_frame = target_frame_ / fps_;
        closest_key_frame = closest_key_frame * fps_;
        if (closest_key_frame <= 24)
            closest_key_frame = 0;

        if (target_frame_ < current_frame_)
        {
            avio_seek(formatContext->pb, 0, SEEK_SET);
            current_frame_ =
                -1;	   //-1 потому что нумерация фреймов идет с 0
                       //после обработки 0-го фрейма currentFrame должен стать 0,
                       //после обработки 1го фрейма currentFrame должен стать 1 итд
        }
        if (closest_key_frame != 0)
        {
            while (current_frame_ != closest_key_frame - 1)
            {
                play();
                ++current_frame_;
            }
        }
        playing_mode_ = PLAYING_MODE::PROCESSING;
        while (current_frame_ != target_frame_ - 1)
        {
            play();
            ++current_frame_;
        }
        playing_mode_ = PLAYING_MODE::GENERIC;
        play();
        ++current_frame_;
    }
}
