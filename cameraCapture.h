#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include <QObject>
#include <QTimer>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "imageProvider.h"
#include "backend.h"

class CameraCapture : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool cameraRunning READ isCameraRunning NOTIFY cameraRunningChanged)

public:
    CameraCapture(ImageProvider *provider, Backend *backend, QObject *parent = nullptr);
    ~CameraCapture();
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void toggleCamera();
    bool isCameraRunning() const {return running;}

signals:
    void cameraRunningChanged();

private slots:
    void captureFrame();

private:
    QTimer timer;
    cv::VideoCapture cap;
    ImageProvider *imageProvider;
    Backend *backend;
    bool running = false;
};

#endif
