#ifndef IMAGE_PROVIDER_H
#define IMAGE_PROVIDER_H

#include <QObject>
#include <QQuickImageProvider>

/**
 * @brief Used to provide image loading features in QML
 */
class ImageProvider : public QObject
, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit ImageProvider(QObject *parent = 0);
    QImage requestImage(const QString &, QSize *, const QSize &);

    QImage img_, hevc_tv_;

signals:
    /**
     * @brief Signal to GUI that a new image has came and is ready to be displayed
     */
    void imageChangedHevcTV();

public slots:
    /**
     * @brief slot waiting for signal signalQImageReady() from HevcQImageEngine::play()
     *
     * @param[in] id stream id
     * @param[in] img finished Qimage
     */
    void slotChangeQImage(int id, QImage img);
};

#endif
