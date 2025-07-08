// imageProvider.cpp
#include "imageProvider.h"

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);
    QMutexLocker locker(&m_mutex);
    if (size) *size = m_image.size();
    return m_image.copy();
}

void ImageProvider::setImage(const QImage &img) {
    QMutexLocker locker(&m_mutex);
    m_image = img.copy();
}

QImage ImageProvider::getCurrentImage() const {
    QMutexLocker locker(&m_mutex);
    return m_image;
}
