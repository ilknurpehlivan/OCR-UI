#ifndef BACKEND_H
#define BACKEND_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <tesseract/baseapi.h>
#include <QString>
#include <QObject>


class Backend: public QObject {
    Q_OBJECT

public:
    Backend(QObject *parent = nullptr);
    ~Backend();

    void setInputFrame(const cv::Mat &frame);
    QImage getProcessedFrame();
    Q_INVOKABLE void runOCRonLastFrame();
    void runDetectionOnly();

    Q_INVOKABLE void clearLog();
    Q_INVOKABLE void exportLog(const QString &logText);



private:
    cv::Mat currentFrame;
    cv::dnn::Net net;
    tesseract::TessBaseAPI tess;

    std::vector<cv::Rect> runDetection(cv::Mat &frame);
    std::string recognizeTextFromROI(const cv::Mat &roi);
    void logText(const std::string &text);
    cv::Mat drawBoxes(cv::Mat &frame, const std::vector<cv::Rect> &boxes, std::vector<std::string> &texts);



signals:
    void logUpdated(const QString &text);
    void threatStatusChanged(const QString &msg, bool isThreat);
    // void clearLogRequested();

};

#endif


