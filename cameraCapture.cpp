#include "cameraCapture.h"
#include <QImage>

CameraWorker::CameraWorker(ImageProvider* provider, QObject *parent)
    : QObject(parent), imageProvider(provider) {
    cap.open(0);  // 0 = default camera
    timer.setInterval(30);  // 30ms ≈ 33 fps
    connect(&timer, &QTimer::timeout, this, &CameraWorker::captureFrame);
    timer.start();
}

void CameraWorker::captureFrame() {
    cv::Mat frame;
    cap >> frame;
    if (!frame.empty()) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        imageProvider->setImage(img);
    }
}
