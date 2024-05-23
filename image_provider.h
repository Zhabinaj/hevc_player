#ifndef IMAGE_PROVIDER_H
#define IMAGE_PROVIDER_H

#include "sei_data.h"

#include <QObject>
#include <QQuickImageProvider>

class ImageProvider : public QObject
, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit ImageProvider(QObject *parent = 0);
    QImage requestImage(const QString &, QSize *, const QSize &);
    QImage img, hevcTV;
    bool show_sei_ = false;

signals:
    void imageChangedHevcTV();

public slots:
    void slotChangeQImage(int id, QImage img, Data_sei_str *pSei_);
    void drawCorners(QPainter *p, int x, int y, int w, int h);
    void drawRectsFromHeader(QImage *dimg, Data_sei_str *sei, int dx, int dy);
    void showSei(bool);
};

#endif	  // IMAGE_PROVIDER_H
