#include <QFile>
#include <iostream>
#include <unistd.h>

#include "ffmpeg.h"
#include "string.h"

ffmpeg::ffmpeg(int id_str, QObject *parent) : QObject(parent)
{
    id_stream = id_str;
    vFrameRGB = 0;
    vbuffer	  = NULL;
}

ffmpeg::~ffmpeg()
{
    // Free the RGB image
    av_frame_free(&vFrameRGB);
    av_free(vbuffer);
}

int ffmpeg::initialization(std::string path)
{
    av_register_all();	  // Регистрация всех доступных форматов и кодеков

    const char *filename = path.c_str();	//Имя вашего HEVC файла
    std::cout << "=============== Filename: " << filename << " ===============" << std::endl;
    //===================================================Обработка ошибок: возврат от -1 до -10
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
    videoStreamIndex	= -1;
    vCodecCtxOrig		= nullptr;
    vCodecCtx			= nullptr;
    AVCodec *vcodec		= nullptr;
    img_convert_context = nullptr;
    std::cout << "=============== Number of streams: " << formatContext->nb_streams << " ===============" << std::endl;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++)	//formatContext->nb_streams = 1
    {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStreamIndex = i;
            vCodecCtxOrig	 = formatContext->streams[videoStreamIndex]->codec;
            vcodec			 = avcodec_find_decoder(vCodecCtxOrig->codec_id);
            vCodecCtx		 = avcodec_alloc_context3(vcodec);
            if (avcodec_copy_context(vCodecCtx, vCodecCtxOrig) != 0)
            {
                fprintf(stderr, "Couldn't copy vcodec context");
                return -4;
            }
            vCodecCtx->thread_count = 1;
            avcodec_open2(vCodecCtx, vcodec, NULL);
            break;
        }
    }

    // Видео поток не найден
    if (videoStreamIndex == -1)
        return -5;

    img_convert_context = sws_getCachedContext(
        NULL, vCodecCtx->width, vCodecCtx->height, vCodecCtx->pix_fmt,
        vCodecCtx->width, vCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL,
        NULL, NULL);

    // int numBytes;
    if (vbuffer != 0)
        av_free(vbuffer);
    // Determine required buffer size and allocate buffer
    int numBytes =
        avpicture_get_size(AV_PIX_FMT_RGB24, vCodecCtx->width, vCodecCtx->height);
    vbuffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    if (vFrameRGB != NULL)
        av_frame_free(&vFrameRGB);
    vFrameRGB = av_frame_alloc();
    avpicture_fill((AVPicture *)vFrameRGB, vbuffer, AV_PIX_FMT_RGB24,
                   vCodecCtx->width, vCodecCtx->height);

    time_base_video = (double)formatContext->streams[id_stream]->time_base.num /
                      (double)formatContext->streams[id_stream]->time_base.den;

    std::cout << "=============== Time base video codec tbc: " << vCodecCtx->time_base.num << "/"
              << vCodecCtx->time_base.den << " ===============" << std::endl;
    std::cout << "=============== Time base video codec tbn: "
              << formatContext->streams[id_stream]->time_base.num << "/"
              << formatContext->streams[id_stream]->time_base.den << " ===============" << std::endl;

    // Получаем декодер для видео потока
    AVCodec *codec = avcodec_find_decoder(
        formatContext->streams[videoStreamIndex]->codecpar->codec_id);

    // Декодер не найден
    if (!codec)
        return -6;

    // Создаем контекст кодека
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext)
        return -7;

    // Настраиваем контекст кодека
    if (avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar) < 0)
        return -8;

    // Открываем кодек
    if (avcodec_open2(codecContext, codec, nullptr) < 0)
        return -9;

    frame = av_frame_alloc();
    avpicture_fill((AVPicture *)frame, vbuffer, AV_PIX_FMT_RGB24,
                   vCodecCtx->width, vCodecCtx->height);
    if (!frame)
        return -10;

    av_init_packet(&packet);	//AVPacket type this function is deprecated?

    stream_ = formatContext->streams[videoStreamIndex];	   //видеопоток один, поэтому videoStreamIndex = 0, id_stream = 0 (установлено в момент создания ffmpeg)

    fps_ = stream_->avg_frame_rate.num / stream_->avg_frame_rate.den;
    std::cout << "=============== FPS: " << fps_ << " ===============" << std::endl;

    //=================Прогоняем видос, считаем кол-во фреймов

    playing_mode_ = PLAYING_MODE::FRAMECOUNT;

    int end_of_file = 1;
    while (end_of_file)
    {
        end_of_file = play();
        ++totalFrames_;
    }
    --totalFrames_;	   //из-за последнего входа в цикл, когда файл пуст
    playing_mode_ = PLAYING_MODE::GENERIC;
    //===========================================================

    // std::cout << "av_read_frame EOF" << ret_read_frame << std::endl;
    std::cout << "=============== Total Frames: " << totalFrames_ << " ===============" << std::endl;
    double seconds = totalFrames_ / fps_;
    int minutes	   = seconds / 60;
    seconds		   = seconds - (60 * minutes);
    int hours	   = minutes / 60;
    minutes		   = minutes - (hours * 60);

    time_ms_  = (totalFrames_ / fps_) * 1000;
    duration_ = time_ms_ * 1000;

    std::cout << "=============== Time: " << ((hours < 10) ? ("0") : ("")) << hours << ":" << ((minutes < 10) ? ("0") : ("")) << minutes << ":" << seconds << " ===============" << std::endl;

    avio_seek(formatContext->pb, 0, SEEK_SET);	  //при всех 0 сбрасывается на начало видео

    play();
    return 0;
}

void ffmpeg::resetVideo()
{
    totalFrames_ = 0;
    currentFrame = 0;
    av_packet_unref(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}

int ffmpeg::play()
{
    /* функция делит содержимой файла formatContext на фреймы
     * и возвроащает по одному через packet за каждый вызов
     * В packet как-то должна хранится ссылка на следующий фрейм
     */

    int ret_read_frame = av_read_frame(formatContext, &packet);

    //=====Проверка на конец файла
    if (ret_read_frame < 0)
    {
        std::cout << "=============== End Of File ===============" << std::endl;
        return 0;
    }
    //==============
    if (playing_mode_ == PLAYING_MODE::GENERIC)
    {
        std::cout << "Start processing frame" << std::endl;
        if (packet.stream_index == id_stream)
        {
            int frame_finished;

            /*
            *  vCodecCtx - кодек
            *  frame - фрейм packet после декодирования
            *  frame_finished - флаг, не 0 если фрейм декодирован
            *  packet - пакет с данными (недекодированый фрейм), полученными из
            * av_read_frame
            */
            avcodec_decode_video2(vCodecCtx, frame, &frame_finished, &packet);
            if (frame_finished)
            {
                // Convert the image from its native format to RGB
                sws_scale(img_convert_context, frame->data, frame->linesize, 0,
                          vCodecCtx->height, vFrameRGB->data, vFrameRGB->linesize);

                /* Constructs an image - img
                * with the given width vCodecCtx->width
                * height vCodecCtx->height
                * and format QImage::Format_RGB888 .
                * that uses an existing memory buffer, data vFrameRGB->data[0]
                */
                timg = QImage((uchar *)vFrameRGB->data[0], vCodecCtx->width,
                              vCodecCtx->height, QImage::Format_RGB888);
                std::cout << "Image Ready" << std::endl;
                if (setFrameMode == 0)
                {
                    emit signalQImageReady(id_stream, timg);
                }
            }
        }
    }
    av_free_packet(&packet);
    return 1;
};

void ffmpeg::setFrame(int targetFrame)
{
    setFrameMode = 1;
    std::cout << "Targer frame: " << targetFrame << std::endl;
    if (targetFrame < currentFrame)
    {
        avio_seek(formatContext->pb, 0, SEEK_SET);
        currentFrame = 0;
    }
    while (currentFrame != targetFrame - 1)	   //прогоняем видос до
    {
        play();
        ++currentFrame;
    }
    setFrameMode = 0;
    play();
    ++currentFrame;
}
