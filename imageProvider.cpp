#include "imageProvider.h"
#include <QPainter>

// From networkhelper.h
#define WINDOW_FRAME_W 1920
#define WINDOW_FRAME_H 1200

ImageProvider::ImageProvider(QObject *parent)
    : QObject(parent), QQuickImageProvider(QQuickImageProvider::Image) {

  img = QImage(WINDOW_FRAME_W, WINDOW_FRAME_H, QImage::Format_RGB888);
  img.fill(0);
  hevcTV = QImage(WINDOW_FRAME_W, WINDOW_FRAME_H, QImage::Format_RGB888);
}

QImage ImageProvider::requestImage(const QString &id, QSize *, const QSize &) {
  return hevcTV;
}

// в hevcTV копируется изображение из img
void ImageProvider::slotChangeQImage(int id, QImage img) {
  if (id == 0) {
    hevcTV = img.copy();
    emit imageChangedHevcTV();
  }
}
