#ifndef IMAGE_PROVIDER_H
#define IMAGE_PROVIDER_H

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

signals:
    void imageChangedHevcTV();

public slots:
    void slotChangeQImage(int id, QImage img);
};

#endif	  // IMAGE_PROVIDER_H
