// imageProvider.h
#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <QMutex>

class ImageProvider : public QQuickImageProvider {
public:
    ImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    void setImage(const QImage &img);
    QImage getCurrentImage() const;

private:
    QImage m_image;
    mutable QMutex m_mutex;
};

#endif // IMAGEPROVIDER_H
