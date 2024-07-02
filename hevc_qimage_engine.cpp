#include "hevc_qimage_engine.h"

#include <QPainter>
#include <iostream>

HevcQImageEngine::HevcQImageEngine(int id_str, QObject *parent) : QObject(parent)
{
    sei_data_  = new Data_sei_str;
    id_stream_ = id_str;
    vbuffer_   = NULL;
    vFrameRGB_ = 0;
}

HevcQImageEngine::~HevcQImageEngine()
{
    delete sei_data_;
    // Free the RGB image
    av_frame_free(&vFrameRGB_);
    av_free(vbuffer_);
}

void HevcQImageEngine::copyMass(bool sei[12])
{
    for (int i = 0; i < 12; ++i)
    {
        sei_options_[i] = sei[i];
    }
}

bool HevcQImageEngine::readFrame()
{
    /* функция делит содержимое файла formatContext на фреймы
    * и возвроащает по одному через packet_ за каждый вызов
    * В packet_ как-то должна хранится ссылка на следующий фрейм
    */
    int ret_read_frame = av_read_frame(formatContext, &packet_);

    //=====Проверка на конец файла
    if (ret_read_frame < 0)
        return 0;

    return 1;
}

int HevcQImageEngine::setCodecCtx()
{
    // Находим видео поток
    vCodecCtx			= nullptr;
    AVCodec *vcodec		= nullptr;
    img_convert_context = nullptr;

    if (formatContext->streams[id_stream_]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
        return -1;

    // Получаем декодер для кодека видео потока
    vcodec = avcodec_find_decoder(formatContext->streams[id_stream_]->codecpar->codec_id);

    // Декодер не найден
    if (!vcodec)
        return -2;

    // Создаем контекст кодека
    vCodecCtx = avcodec_alloc_context3(vcodec);
    if (!vCodecCtx)
        return -3;

    // Настраиваем контекст кодека
    if (avcodec_parameters_to_context(
            vCodecCtx, formatContext->streams[id_stream_]->codecpar) < 0)
        return -4;

    // Открываем кодек
    if (avcodec_open2(vCodecCtx, vcodec, nullptr) < 0)
        return -5;

    img_convert_context = sws_getCachedContext(
        NULL, vCodecCtx->width, vCodecCtx->height, vCodecCtx->pix_fmt,
        vCodecCtx->width, vCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL,
        NULL, NULL);
    return 1;
}

int HevcQImageEngine::initialization(std::string path)
{
    av_register_all();	  // Регистрация всех доступных форматов и кодеков
    open_file_name_ = path;

    formatContext = avformat_alloc_context();
    if (!formatContext)
        return -1;

    // Открываем файл
    if (avformat_open_input(&formatContext, open_file_name_.c_str(), nullptr, nullptr) != 0)
        return -2;

    // Получаем информацию о потоке
    if (avformat_find_stream_info(formatContext, nullptr) < 0)
        return -3;

    if (setCodecCtx() < 0)
        return -4;

    if (!preparePictureArray())
        return -5;

    fps_ = formatContext->streams[id_stream_]->avg_frame_rate.num / formatContext->streams[id_stream_]->avg_frame_rate.den;

    getTotalFrames();
    findFirstKeyFrame();
    // initializationPrintData();

    return 0;
}

bool HevcQImageEngine::preparePictureArray()
{
    int numBytes =
        avpicture_get_size(AV_PIX_FMT_RGB24, vCodecCtx->width, vCodecCtx->height);

    //Очищаем от хлама
    if (vbuffer_ != 0)
        av_free(vbuffer_);

    vbuffer_ = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    //Очищаем от хлама
    if (vFrameRGB_ != NULL)
        av_frame_free(&vFrameRGB_);

    vFrameRGB_ = av_frame_alloc();
    avpicture_fill((AVPicture *)vFrameRGB_, vbuffer_, AV_PIX_FMT_RGB24,
                   vCodecCtx->width, vCodecCtx->height);

    frame_ = av_frame_alloc();
    avpicture_fill((AVPicture *)frame_, vbuffer_, AV_PIX_FMT_RGB24,
                   vCodecCtx->width, vCodecCtx->height);
    if (!frame_)
        return 0;

    av_init_packet(&packet_);

    return 1;
}

void HevcQImageEngine::getTotalFrames()
{
    //=================Прогоняем видео для подсчета общего количества фреймов
    while (1)
    {
        if (!readFrame())
            break;
        av_packet_unref(&packet_);
        ++total_frames_;
    }
    //при всех 0 сбрасывается на начало видео
    avio_seek(formatContext->pb, 0, SEEK_SET);
}
//сбросили на 0, обработали первый фрейм, сработал счетчик.
//в некоторых видео первый кадр - ключевой, а остальные 24 достроены на его основе. В других видео из-за особености записи ключевыми кадрами
//могут быть 1,2,3 и т.д. Наш первый ключевой кадр = последний из последовательности ключевых кадров в начале видео, за которым следует
//не ключевой
void HevcQImageEngine::findFirstKeyFrame()
{
    first_keyframe_ = 0;

    while (1)
    {
        readFrame();

        if (packet_.stream_index == id_stream_)
        {
            int frame_finished;
            avcodec_decode_video2(vCodecCtx, frame_, &frame_finished, &packet_);
            if (frame_->key_frame)
                ++first_keyframe_;
            else
                break;
        }
        av_packet_unref(&packet_);
    }
    avio_seek(formatContext->pb, 0, SEEK_SET);
}

void HevcQImageEngine::initializationPrintData()
{
    std::cout << "=============== Filename: " << open_file_name_.c_str()
              << " ===============" << std::endl;
    std::cout << "=============== Number of streams: "
              << formatContext->nb_streams
              << " ===============" << std::endl;
    std::cout << "=============== FPS: " << fps_
              << " ===============" << std::endl;
    std::cout << "=============== First keyframe: " << first_keyframe_ << " ===============" << std::endl;
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
}

bool HevcQImageEngine::processingFrame()
{
    if (packet_.stream_index == id_stream_)
    {
        int frame_finished;	   //флаг, не 0 если фрейм декодирован

        avcodec_decode_video2(vCodecCtx, frame_, &frame_finished, &packet_);
        if (frame_finished)
        {
            // Convert the image from its native format to RGB
            sws_scale(img_convert_context, frame_->data, frame_->linesize, 0,
                      vCodecCtx->height, vFrameRGB_->data, vFrameRGB_->linesize);

            q_img_ = QImage((uchar *)vFrameRGB_->data[0], vCodecCtx->width,
                            vCodecCtx->height, QImage::Format_RGB888);
            av_packet_unref(&packet_);
            return 1;
        }
        else
            return 0;
    }
}

bool HevcQImageEngine::getSei()
{
    if (frame_->nb_side_data > 0)
    {
        AVFrameSideData *sd = frame_->side_data[0];
        if (sd->type == AV_FRAME_DATA_SEI_UNREGISTERED)
        {
            memcpy(sei_data_, sd->data, sizeof(Data_sei_str));
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

bool HevcQImageEngine::play()	 //тут передаем массив сеи в плэй
{
    if (readFrame())
        processingFrame();
    else
    {
        std::cout << "=============== End Of File ===============" << std::endl;
        return 0;
    }

    bool get_sei_flag = getSei();
    if (get_sei_flag)
        drawDataOnFrame();	  //отправляем в рисовашку
    else
        std::cout << "Error get sei" << std::endl;

    emit signalQImageReady(id_stream_, q_img_);
    return 1;
}

void HevcQImageEngine::drawDataOnFrame()
{
    QPainter paint(&q_img_);

    makeQString();

    drawBackgroundRect(&paint);

    selectDataToDraw(&paint);
}

void HevcQImageEngine::makeQString()
{

    timeStr = QString("Таймштамп   : %1 ").arg(sei_data_->sys_time);
    latitude =
        QString("Широта      : ") + QString::number(sei_data_->latitude_bla, 'f', 8);
    longitude = QString("Долгота     : ") +
                QString::number(sei_data_->longitude_bla, 'f',
                                8);
    altitude = QString("Высота      : %1 ").arg(sei_data_->altitude_bla);
    yaw_bla	 = QString("Курс БЛА    : ") + QString::number(sei_data_->yaw_bla, 'f', 2);
    yaw_ops	 = QString("Курс OPS    : ") + QString::number(sei_data_->yaw, 'f', 2);
    pitch_bla =
        QString("Тангаж БЛА  : ") + QString::number(sei_data_->pitch_bla, 'f', 2);
    pitch_ops = QString("Тангаж OPS  : ") + QString::number(sei_data_->pitch, 'f', 2);
    roll_bla =
        QString("Крен БЛА    : ") + QString::number(sei_data_->roll_bla, 'f', 2);
    fov =
        QString("Поле зрения : ") + QString::number(sei_data_->camera.fov_h, 'f', 2);
    dist = QString("Дальность   : %1 ").arg(sei_data_->ld_distance);
}

void HevcQImageEngine::drawBackgroundRect(QPainter *p)
{

    //прогоняем массив и считаем сколько элементов надо отобразить. В зависимости от этого меняем длину прямоугольника
    //до 11, вотому что для сопровождения прямугольник не нужен
    int elements_in_rect = 0;

    for (int i = 0; i < 11; ++i)
    {
        if (sei_options_[i])
            ++elements_in_rect;
    }

    //для отрисовки 1 строки нужна высота 100 . За каждый доп элемент прибавляем +30
    if (elements_in_rect != 0)
    {
        int rect_height = 20 + (elements_in_rect * 30);

        QBrush background;
        background.setColor(Qt::gray);
        background.setStyle(Qt::Dense4Pattern);

        p->setBrush(background);	  //задаем текущему qpainter нашу кисть
        p->setPen(QPen(Qt::gray));	  //устанавливем текущую ручку серой
        p->drawRect(10, 10, 320,
                    rect_height);	 //функция рисует прямоугольник текущей кистью
        //в начальных координатах 10,10
        //шириной 300 высотой 400
        // границы прямоугольника рисуются текущей ручкой

        p->setPen(QPen(Qt::black));	   //переключаем цвет текущей ручки на черный
        p->setFont(QFont("Courier", 15, QFont::Normal));
    }
}

void HevcQImageEngine::selectDataToDraw(QPainter *p)
{
    //изменять динамически вторую координату
    int y = 0;
    for (int i = 0; i < 12; ++i)
    {
        if (sei_options_[i])
        {
            y += 30;
            switch (i)
            {
                case 0:

                    p->drawText(15, y, timeStr);
                    break;
                case 1:
                    p->drawText(15, y, latitude);
                    break;
                case 2:
                    p->drawText(15, y, longitude);
                    break;
                case 3:
                    p->drawText(15, y, altitude);
                    break;
                case 4:
                    p->drawText(15, y, yaw_bla);
                    break;
                case 5:
                    p->drawText(15, y, yaw_ops);
                    break;
                case 6:
                    p->drawText(15, y, pitch_bla);
                    break;
                case 7:
                    p->drawText(15, y, pitch_ops);
                    break;
                case 8:
                    p->drawText(15, y, roll_bla);
                    break;
                case 9:
                    p->drawText(15, y, fov);
                    break;
                case 10:
                    p->drawText(15, y, dist);
                    break;
                case 11:
                    drawTracker(p);
                    break;
            }
        }
    }
}

void HevcQImageEngine::drawTracker(QPainter *p)
{
    int dx = 0;
    int dy = 0;

    strob_struct *st;

    for (int i = 0; i < 5; i++)
    {
        st = sei_data_->strob + i;
        //==swich case
        if (st->type == 0)
            p->setPen(QPen(QBrush(Qt::blue), 6));
        else if (st->type == 1)
            p->setPen(QPen(QBrush(Qt::green), 6));
        else if (st->type == 2)
            p->setPen(QPen(QBrush(Qt::yellow), 6));
        else if (st->type == 3)
            p->setPen(QPen(QBrush(Qt::cyan), 6));
        else if (st->type == 4)
            p->setPen(QPen(QBrush(Qt::darkCyan), 6));
        else
            p->setPen(QPen(QBrush(Qt::gray), 6));
        //

        if (q_img_.width() == 1920 && q_img_.height() == 1080)
        {
            dx = (q_img_.width() - 1024) / 2 + (47.8112 - 90.3752 / sei_data_->camera.fov_h);
            dy = (q_img_.height() - 768) / 2 +
                 (0.0001882264 * sei_data_->camera.fov_h * sei_data_->camera.fov_h *
                      sei_data_->camera.fov_h -
                  0.0256920432 * sei_data_->camera.fov_h * sei_data_->camera.fov_h +
                  1.1316176497 * sei_data_->camera.fov_h - 2.1749032649);
        }

        float scale_x = 1.0f;
        float scale_y = 1.0f;

        if (q_img_.width() == 640 && q_img_.height() == 512)
        {
            scale_x = (q_img_.width() / 1024.0f);
            scale_y = (q_img_.height() / 768.0f);
        }

        p->drawRect(scale_x * (st->x + dx), scale_y * (st->y + dy),
                    scale_x * (st->width), scale_x * (st->height));

        if (st->track == 1)
            drawCorners(p, scale_x * (st->x + dx), scale_y * (st->y + dy),
                        scale_x * (st->width), scale_y * (st->height));
    }
}

void HevcQImageEngine::drawCorners(QPainter *p, int x, int y, int w, int h)
{
    int b  = 6;
    int x1 = x - b - 3;
    int y1 = y - b - 3;
    int x2 = x + w + b + 3;
    int y2 = y + h + b + 3;
    int dw = w * 0.15;
    int dh = h * 0.15;
    p->setPen(QPen(QBrush(Qt::red), 6));
    p->drawLine(x1, y1, x1, y1 + dh);
    p->drawLine(x1, y1, x1 + dw, y1);
    p->drawLine(x2, y1, x2, y1 + dh);
    p->drawLine(x2, y1, x2 - dw, y1);
    p->drawLine(x1, y2, x1, y2 - dh);
    p->drawLine(x1, y2, x1 + dw, y2);
    p->drawLine(x2, y2, x2, y2 - dh);
    p->drawLine(x2, y2, x2 - dw, y2);
}

void HevcQImageEngine::resetVideo()
{
    total_frames_ = 0;
    av_packet_unref(&packet_);
    av_packet_unref(&packet_);
    avcodec_free_context(&vCodecCtx);
    avformat_close_input(&formatContext);
    av_frame_free(&frame_);
}
