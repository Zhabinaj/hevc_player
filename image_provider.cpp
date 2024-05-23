#include "image_provider.h"
#include <QFont>
#include <QPainter>
#include <QPainterPath>

#define WINDOW_FRAME_W 1920
#define WINDOW_FRAME_H 1200

ImageProvider::ImageProvider(QObject *parent)
: QObject(parent), QQuickImageProvider(QQuickImageProvider::Image)
{

    img = QImage(WINDOW_FRAME_W, WINDOW_FRAME_H, QImage::Format_RGB888);
    img.fill(0);
    hevcTV = QImage(WINDOW_FRAME_W, WINDOW_FRAME_H, QImage::Format_RGB888);
}

QImage ImageProvider::requestImage(const QString &id, QSize *, const QSize &)
{
    return hevcTV;
}

void ImageProvider::showSei(bool checked)
{
    if (checked)
        show_sei_ = true;
    else
        show_sei_ = false;
}

// в hevcTV копируется изображение из img
void ImageProvider::slotChangeQImage(int id, QImage img, Data_sei_str *pSei_)
{
    if (id == 0)
    {
        hevcTV = img.copy();
        if (pSei_ != 0 && show_sei_)
            drawRectsFromHeader(&hevcTV, pSei_, 0, 0);

        emit imageChangedHevcTV();
    }
}
//вывод метаданных
void ImageProvider::drawRectsFromHeader(QImage *dimg, Data_sei_str *sei, int dx,
                                        int dy)
{

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

        //    if (st->track == 1)
        //      int track = 0;

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

void ImageProvider::drawCorners(QPainter *p, int x, int y, int w, int h)
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
