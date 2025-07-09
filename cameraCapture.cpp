// cameraCapture.cpp
#include "cameraCapture.h"
#include <QDebug>

CameraCapture::CameraCapture(ImageProvider *provider, Backend *backend, QObject *parent)
    : QObject(parent), imageProvider(provider), backend(backend) {}

CameraCapture::~CameraCapture() {
    stop();
}

void CameraCapture::start() {
    if (!cap.open(0)) {
        qWarning() << "Kamera açılamadı!";
        return;
    }
    connect(&timer, &QTimer::timeout, this, &CameraCapture::captureFrame);
    timer.start(30);
    running=true;
    emit cameraRunningChanged();
}

void CameraCapture::stop() {
    if (!running) return;
    timer.stop();
    if (cap.isOpened()) cap.release();
    running = false;

    QImage black(640, 480, QImage::Format_RGB888);
    black.fill(Qt::black);
    imageProvider->setImage(black);


    emit cameraRunningChanged();
}

void CameraCapture::toggleCamera() {
    running ? stop() : start();
}

void CameraCapture::captureFrame() {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;

    backend->setInputFrame(frame);
    backend->runDetectionOnly();
    QImage processed = backend->getProcessedFrame();

    imageProvider->setImage(processed);
}
