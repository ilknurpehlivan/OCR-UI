#ifndef CAMERA_WORKER_H
#define CAMERA_WORKER_H

#include <QObject>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "imageProvider.h"

class CameraWorker : public QObject {
    Q_OBJECT

public:
    CameraWorker(ImageProvider* provider, QObject *parent = nullptr);

public slots:
    void captureFrame();

private:
    cv::VideoCapture cap;
    ImageProvider* imageProvider;
    QTimer timer;
};

#endif // CAMERA_WORKER_H
