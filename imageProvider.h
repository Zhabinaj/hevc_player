#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QQuickImageProvider>

class ImageProvider : public QObject, public QQuickImageProvider {
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

#endif // IMAGEPROVIDER_H
