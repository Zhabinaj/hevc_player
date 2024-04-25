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

    formatContext = avformat_alloc_context();
    if (!formatContext)
    {
        // Обработка ошибок
        return -1;
    }

    // Открываем файл
    if (avformat_open_input(&formatContext, filename, nullptr, nullptr) != 0)
    {
        // Обработка ошибок
        return -2;
    }

    // Получаем информацию о потоке
    if (avformat_find_stream_info(formatContext, nullptr) < 0)
    {
        // Обработка ошибок
        return -3;
    }

    // Находим видео поток
    int videoStreamIndex = -1;
    vCodecCtxOrig		 = nullptr;
    vCodecCtx			 = nullptr;
    AVCodec *vcodec		 = nullptr;
    img_convert_context	 = nullptr;
    std::cout << "nb_streams: " << formatContext->nb_streams << std::endl;
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

    if (videoStreamIndex == -1)
    {
        // Видео поток не найден
        return -5;
        //asdfsfsf
    }

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

    std::cout << "time_base video codec=" << vCodecCtx->time_base.num << "/"
              << vCodecCtx->time_base.den << " "
              << formatContext->streams[id_stream]->time_base.num << "/"
              << formatContext->streams[id_stream]->time_base.den << std::endl;

    // Получаем декодер для видео потока
    AVCodec *codec = avcodec_find_decoder(
        formatContext->streams[videoStreamIndex]->codecpar->codec_id);

    if (!codec)
    {
        // Декодер не найден
        return -6;
    }

    // Создаем контекст кодека
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext)
    {
        // Обработка ошибок
        return -7;
    }

    // Настраиваем контекст кодека
    if (avcodec_parameters_to_context(
            codecContext, formatContext->streams[videoStreamIndex]->codecpar) <
        0)
    {
        // Обработка ошибок
        return -8;
    }

    // Открываем кодек
    if (avcodec_open2(codecContext, codec, nullptr) < 0)
    {
        // Обработка ошибок
        return -9;
    }

    frame = av_frame_alloc();
    avpicture_fill((AVPicture *)frame, vbuffer, AV_PIX_FMT_RGB24,
                   vCodecCtx->width, vCodecCtx->height);
    if (!frame)
    {
        // Обработка ошибок
        return -10;
    }

    av_init_packet(&packet);	//AVPacket type this function is deprecated?

    //использовать в пресс плэй?
    // int frames = 0;
    // ++frames;
    // std::cout << "Frames:" << frames << std::endl;
    stream_ = formatContext->streams[videoStreamIndex];	   //видеопоток один, поэтому videoStreamIndex = 0, id_stream = 0 (установлено в момент создания ffmpeg)

    if (stream_->nb_frames > 0)
        totalFrames = stream_->nb_frames - 1;
    else
    {

        //тотал фреймс
        std::cout << "duration:" << formatContext->duration << std::endl;				   //int
        std::cout << "avg_frame_rate.num:" << stream_->avg_frame_rate.num << std::endl;	   //int
        std::cout << "avg_frame_rate.den:" << stream_->avg_frame_rate.den << std::endl;	   //int
        std::cout << "AV_TIME_BASE:" << AV_TIME_BASE << std::endl;						   //int

        totalFrames = static_cast<unsigned long>(formatContext->duration * (stream_->avg_frame_rate.num / stream_->avg_frame_rate.den) / AV_TIME_BASE);
    }
    std::cout << "totalFrames: " << totalFrames << std::endl;
    play();
    return 0;
}

void ffmpeg::resetVideo()
{
    av_packet_unref(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}

int ffmpeg::play()
{
    // для скорости воспроизведения
    //frame_timer	= av_gettime(); //возвращает текущее время в мс

    /* функция делит содержимой файла formatContext на фреймы
     * и возвроащает по одному через packet за каждый вызов
     * В packet как-то должна хранится ссылка на следующий фрейм
     */
    auto ret_read_frame = av_read_frame(formatContext, &packet);

    //=====Проверка на конец файла
    if (ret_read_frame < 0)
    {
        std::cout << "av_read_frame EOF" << ret_read_frame << std::endl;
        return 0;
    }
    //==============

    else
    {
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

                emit signalQImageReady(id_stream, timg);
                /*
                //=============Скорость воспроизведения видео
                usleep(20000); //текущий поток приостановлен на 20000мкс либо пока не поступит сигнал по которому вызывается функция обработки сигналов или программа завершает свою работу

                frame_pts_ms = frame->pkt_pts * 1e6 * time_base_video;                      //frame_pts_ms = 0
                delay		 = frame_pts_ms - frame_last_pts;	 // the pts from last time  //delay = 0 //frame_last_pts = 0
                if (delay <= 0 || delay >= 1e6)
                    delay = frame_last_delay;	 // if incorrect delay, use previous one    //frame_last_delay = 40000
                frame_timer += delay;                                                       //frame_timer текущее время в мс, засеченное выше
                actual_delay = frame_timer - av_gettime();                                  //actual_delay = 0
                if (actual_delay < 10000)
                    actual_delay = 10000;
                usleep(actual_delay); // фриз фрейма на actual_delay, для эмуляции режима с другим фпс
                // save for next time
                frame_last_delay = delay;
                frame_last_pts	 = frame_pts_ms;
                //======================================
                */
            }
        }
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
        return 1;
    }
};
