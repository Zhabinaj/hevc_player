#include "hevc_qimage_engine.h"

#include <QPainter>
#include <iostream>

HevcQImageEngine::HevcQImageEngine(int id_str, QObject *parent) : QObject(parent)
{
    id_stream_ = id_str;
    vbuffer_   = NULL;
    vFrameRGB_ = 0;
}

HevcQImageEngine::~HevcQImageEngine()
{
    // Free the RGB image
    av_frame_free(&vFrameRGB_);
    av_free(vbuffer_);
}

bool HevcQImageEngine::readFrame()
{
    /* функция делит содержимой файла formatContext на фреймы
   * и возвроащает по одному через packet_ за каждый вызов
   * В packet_ как-то должна хранится ссылка на следующий фрейм
   */
    int ret_read_frame = av_read_frame(formatContext, &packet_);

    //=====Проверка на конец файла
    if (ret_read_frame < 0)
    {
        std::cout << "=============== End Of File ===============" << std::endl;
        return 0;
    }
    return 1;
}

int HevcQImageEngine::initialization(std::string path)
{

    av_register_all();	  // Регистрация всех доступных форматов и кодеков

    open_file_name_ = path;	   //Имя вашего HEVC файла

    // open_file_name_ = path.c_str();	   //Имя вашего HEVC файла
    std::cout << "=============== Filename: " << open_file_name_.c_str()
              << " ===============" << std::endl;

    //===================================================Обработка ошибок: возврат
    //от -1 до -9
    formatContext = avformat_alloc_context();
    if (!formatContext)
        return -1;
    std::cout << "check 1" << std::endl;
    // Открываем файл
    if (avformat_open_input(&formatContext, open_file_name_.c_str(), nullptr, nullptr) != 0)
        return -2;
    std::cout << "check 2" << std::endl;

    // Получаем информацию о потоке
    if (avformat_find_stream_info(formatContext, nullptr) < 0)	  //=================HERE
        return -3;
    std::cout << "check 3" << std::endl;

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

    int end_of_file	 = 1;
    bool find_false	 = false;
    int true_counter = 0;
    while (end_of_file)
    {
        end_of_file = readFrame();
        if (!find_false)	//??
        //if (!find_keyframe)
        {
            if (packet_.stream_index == id_stream_)
            {
                int frame_finished;
                avcodec_decode_video2(vCodecCtx, frame_, &frame_finished, &packet_);
                if (frame_->key_frame)
                {
                    std::cout << "true" << std::endl;
                    ++true_counter;	   //find_keyframe = true;
                }
                else
                {
                    std::cout << "false" << std::endl;
                    find_false = true;
                }	 //++first_keyframe_;
            }
        }
        //не работает
        av_free_packet(&packet_);
        ++total_frames_;
    }
    --total_frames_;	//из-за последнего входа в цикл, когда файл пуст
    first_keyframe_ = true_counter - 1;
    //===========================================================

    std::cout << "First kayframe is " << first_keyframe_ << std::endl;
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
    return 0;
}

//вернет 1 если создан QImage, 0 если не удалось
bool HevcQImageEngine::processingFrame(QImage &img)
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

            img = QImage((uchar *)vFrameRGB_->data[0], vCodecCtx->width,
                         vCodecCtx->height, QImage::Format_RGB888);
            av_free_packet(&packet_);

            return 1;
        }
        else
            return 0;
    }
}
//получаем sei из фрейма и записываем в ту структуру, которая передается через параметры
bool HevcQImageEngine::getSei(Data_sei_str *str)
{

    if (frame_->nb_side_data > 0)
    {
        AVFrameSideData *sd = frame_->side_data[0];
        if (sd->type == AV_FRAME_DATA_SEI_UNREGISTERED)
        {
            memcpy(str, sd->data, sizeof(Data_sei_str));
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

bool HevcQImageEngine::play(bool show_sei, Data_sei_str *str, QImage &img)
{
    if (readFrame())
        processingFrame(img);
    else
        return 0;

    bool get_sei_flag;
    if (show_sei)
    {
        get_sei_flag = getSei(str);	   //вернет 1 если всё ок
        if (get_sei_flag)
            drawDataOnFrame(str, &img);	   //отправляем в рисовашку, у плеера должен быть свой QImage?
        else
            std::cout << "Error get sei" << std::endl;
    }

    emit signalQImageReady(id_stream_, img);
    return 1;
}

void HevcQImageEngine::drawDataOnFrame(Data_sei_str *sei, QImage *dimg)
{
    int dx = 0;
    int dy = 0;
    QPainter paint(dimg);
    strob_struct *st;

    for (int i = 0; i < 5; i++)
    {
        st = sei->strob + i;
        if (st->type == 0)
            paint.setPen(QPen(QBrush(Qt::blue), 2));
        else if (st->type == 1)
            paint.setPen(QPen(QBrush(QColor(55, 0xff, 55)), 2));
        else if (st->type == 2)
            paint.setPen(QPen(QBrush(Qt::yellow), 2));
        else if (st->type == 3)
            paint.setPen(QPen(QBrush(Qt::cyan), 2));
        else if (st->type == 4)
            paint.setPen(QPen(QBrush(Qt::darkCyan), 2));
        else
            paint.setPen(QPen(QBrush(Qt::gray), 2));

        if (dimg->width() == 1920 && dimg->height() == 1080)
        {
            dx = (dimg->width() - 1024) / 2 + (47.8112 - 90.3752 / sei->camera.fov_h);
            dy = (dimg->height() - 768) / 2 +
                 (0.0001882264 * sei->camera.fov_h * sei->camera.fov_h *
                      sei->camera.fov_h -
                  0.0256920432 * sei->camera.fov_h * sei->camera.fov_h +
                  1.1316176497 * sei->camera.fov_h - 2.1749032649);
        }

        float scale_x = 1.0f;
        float scale_y = 1.0f;

        if (dimg->width() == 640 && dimg->height() == 512)
        {
            scale_x = (dimg->width() / 1024.0f);
            scale_y = (dimg->height() / 768.0f);
        }

        paint.drawRect(scale_x * (st->x + dx), scale_y * (st->y + dy),
                       scale_x * (st->width), scale_x * (st->height));

        if (st->track == 1)
            drawCorners(&paint, scale_x * (st->x + dx), scale_y * (st->y + dy),
                        scale_x * (st->width), scale_y * (st->height));
        QString modeStr, timeStr, latitude, longitude, altitude, distance, yaw_ops,
            pitch_ops, yaw_bla, pitch_bla, roll_bla, fov, dist;

        timeStr = QString("Таймштамп   : %1 ").arg(sei->sys_time);
        latitude =
            QString("Широта      : ") + QString::number(sei->latitude_bla, 'f', 8);
        longitude = QString("Долгота     : ") +
                    QString::number(sei->longitude_bla, 'f',
                                    8);	   //   arg(h->longitude*180./M_PI);
        altitude = QString("Высота      : %1 ").arg(sei->altitude_bla);
        yaw_bla	 = QString("Курс БЛА    : ") + QString::number(sei->yaw_bla, 'f', 2);
        yaw_ops	 = QString("Курс OPS    : ") + QString::number(sei->yaw, 'f', 2);
        pitch_bla =
            QString("Тангаж БЛА  : ") + QString::number(sei->pitch_bla, 'f', 2);
        pitch_ops = QString("Тангаж OPS  : ") + QString::number(sei->pitch, 'f', 2);
        roll_bla =
            QString("Крен БЛА    : ") + QString::number(sei->roll_bla, 'f', 2);
        fov =
            QString("Поле зрения : ") + QString::number(sei->camera.fov_h, 'f', 2);
        dist = QString("Дальность   : %1 ").arg(sei->ld_distance);

        QBrush background;
        background.setColor(Qt::gray);
        background.setStyle(Qt::Dense4Pattern);

        paint.setBrush(background);		 //задаем текущему qpainter нашу кисть
        paint.setPen(QPen(Qt::gray));	 //устанавливем текущую ручку серой
        paint.drawRect(10, 10, 320,
                       400);	//функция рисует прямоугольник текущей кистью
        //в начальных координатах 10,10
        //шириной 300 высотой 400
        // границы прямоугольника рисуются текущей ручкой

        paint.setPen(QPen(Qt::black));	  //переключаем цвет текущей ручки на черный

        paint.setFont(QFont("Courier", 15, QFont::Normal));
        paint.drawText(15, 30, timeStr);
        paint.drawText(15, 60, latitude);
        paint.drawText(15, 90, longitude);
        paint.drawText(15, 120, altitude);
        paint.drawText(15, 150, yaw_bla);
        paint.drawText(15, 180, pitch_bla);
        paint.drawText(15, 210, roll_bla);
        paint.drawText(15, 240, yaw_ops);
        paint.drawText(15, 270, pitch_ops);
        paint.drawText(15, 300, dist);
        paint.drawText(15, 330, fov);
    }
}

void HevcQImageEngine::drawCorners(QPainter *p, int x, int y, int w, int h)
{
    int b  = 2;
    int x1 = x - b - 1;
    int y1 = y - b - 1;
    int x2 = x + w + b + 1;
    int y2 = y + h + b + 1;
    int dw = w * 0.15;
    int dh = h * 0.15;
    p->setPen(QPen(QBrush(Qt::red), 2));
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
    av_free_packet(&packet_);
    av_packet_unref(&packet_);
    avcodec_free_context(&vCodecCtx);
    avformat_close_input(&formatContext);
    av_frame_free(&frame_);
}
