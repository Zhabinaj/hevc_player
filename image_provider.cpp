#include "image_provider.h"
#include <QFont>
#include <QPainter>
#include <QPainterPath>

#define WINDOW_FRAME_W 1920
#define WINDOW_FRAME_H 1200

ImageProvider::ImageProvider(QObject *parent)
: QObject(parent), QQuickImageProvider(QQuickImageProvider::Image)
{
    img_ = QImage(WINDOW_FRAME_W, WINDOW_FRAME_H, QImage::Format_RGB888);
    img_.fill(0);
    hevc_tv_ = QImage(WINDOW_FRAME_W, WINDOW_FRAME_H, QImage::Format_RGB888);
}

QImage ImageProvider::requestImage(const QString &id, QSize *, const QSize &)
{
    return hevc_tv_;
}

void ImageProvider::slotChangeQImage(int id, QImage img)
{
    if (id == 0)
    {
        hevc_tv_ = img.copy();
        emit imageChangedHevcTV();
    }
}
