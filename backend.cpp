#include "backend.h"
#include <QDebug>
#include <QDateTime>
#include <QImage>
#include <QDir>
#include <fstream>

Backend::Backend(QObject *parent) : QObject(parent) {
    net = cv::dnn::readNetFromONNX("/home/mana/projectOCR/best.onnx");
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    if(tess.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY)) {
        qDebug() << "Tesseract başlatılamadı!";
    } else {
        // tess.SetPageSegMode(tesseract::PSM_AUTO);
        tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    }
}

Backend::~Backend() {
    tess.End();
}

void Backend::setInputFrame(const cv::Mat &frame) {
    frame.copyTo(currentFrame);
}

QImage Backend::getProcessedFrame() {
    if(currentFrame.empty()) {
        return QImage();
    }
    cv::Mat rgb;
    cv::cvtColor(currentFrame, rgb, cv::COLOR_BGR2RGB);
    return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
}

std::vector<cv::Rect> Backend::runDetection(cv::Mat &frame) {
    std::vector<cv::Rect> boxes;
    const float confThreshold = 0.4;
    const float nmsThreshold = 0.45;
    const int inputWidth = 416;
    const int inputHeight = 416;

    cv::Mat blob = cv::dnn::blobFromImage(frame, 1/255.0, cv::Size(inputWidth, inputHeight), cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> tempBoxes;

    const int rows = outputs[0].size[1];
    const int dimensions = outputs[0].size[2];
    float* data = (float*)outputs[0].data;

    for (int i = 0; i < rows; ++i) {
        float objectness = data[4];
        if (objectness >= confThreshold) {
            float* classScores = data + 5;
            cv::Mat scores(1, dimensions - 5, CV_32FC1, classScores);
            cv::Point classIdPoint;
            double maxClassScore;
            minMaxLoc(scores, 0, &maxClassScore, 0, &classIdPoint);

            float confidence = objectness * maxClassScore;
            if (confidence > confThreshold) {
                float cx = data[0];
                float cy = data[1];
                float w = data[2];
                float h = data[3];

                int left = int((cx - w / 2) * frame.cols / inputWidth);
                int top = int((cy - h / 2) * frame.rows / inputHeight);
                int width = int(w * frame.cols / inputWidth);
                int height = int(h * frame.rows / inputHeight);

                classIds.push_back(classIdPoint.x);
                confidences.push_back(confidence);
                tempBoxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        data += dimensions;
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(tempBoxes, confidences, confThreshold, nmsThreshold, indices);

    for (int i : indices) {
        boxes.push_back(tempBoxes[i]);
    }

    return boxes;
}


std::string Backend::recognizeTextFromROI(const cv::Mat &roiOriginal) {

    if (roiOriginal.empty() || roiOriginal.cols < 30 || roiOriginal.rows < 15) {
        qDebug() << "ROI çok küçük veya boş.";
        return "";
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
    QString filename = QString("/home/mana/projectOCR/roi_images/roi_%1.jpg").arg(timestamp);
    cv::imwrite(filename.toStdString(), roiOriginal);


    // Griye çevir
    cv::Mat gray;
    cv::cvtColor(roiOriginal, gray, cv::COLOR_BGR2GRAY);

    // Kontrast artır
    cv::threshold(gray, gray, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Boyutu artır
    cv::Mat resized;
    cv::resize(gray, resized, cv::Size(), 2.0, 2.0, cv::INTER_LINEAR);

    // OCR
    tess.SetImage(resized.data, resized.cols, resized.rows, 1, resized.step);
    tess.SetSourceResolution(300);
    tess.Recognize(0);
    char* outText = tess.GetUTF8Text();
    std::string result(outText ? outText : "");
    delete[] outText;

    return result;
}

void Backend::runDetectionOnly() {
    if (currentFrame.empty()) return;

    std::vector<cv::Rect> boxes = runDetection(currentFrame);
    std::vector<std::string> dummyTexts(boxes.size(), "");

    currentFrame = drawBoxes(currentFrame, boxes, dummyTexts);
    if (boxes.empty()) {
        emit threatStatusChanged("", false);
    }
}

void Backend::runOCRonLastFrame() {
    if (currentFrame.empty()) return;

    std::vector<cv::Rect> boxes = runDetection(currentFrame);
    std::vector<std::string> recognizedTexts;

    for (const auto &box : boxes) {
        cv::Rect safeBox = box & cv::Rect(0, 0, currentFrame.cols, currentFrame.rows);
        cv::Mat roi = currentFrame(safeBox);
        std::string text = recognizeTextFromROI(roi);
        recognizedTexts.push_back(text);
        logText(text);
    }

    currentFrame = drawBoxes(currentFrame, boxes, recognizedTexts);
}

cv::Mat Backend::drawBoxes(cv::Mat &frame, const std::vector<cv::Rect> &boxes, std::vector<std::string> &texts) {
    for (size_t i = 0; i < boxes.size(); ++i) {
        const cv::Rect &box = boxes[i];
        const std::string &label = texts[i];

        cv::rectangle(frame, box, cv::Scalar(0, 0, 255), 2);

        int baseline = 0;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
        cv::rectangle(frame, cv::Point(box.x, box.y - labelSize.height - 5),
                      cv::Point(box.x + labelSize.width, box.y), cv::Scalar(0, 0, 255), cv::FILLED);

        cv::putText(frame, label, cv::Point(box.x, box.y - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }
    return frame;
}

void Backend::logText(const std::string &text) {
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString originalText = QString::fromStdString(text).trimmed();
    QString qText = QString("[%1] %2").arg(timestamp, originalText);
    emit logUpdated(qText);


    QMap<QString, QStringList> keywordMap = {
        { "danger", {"danger", "denger", "danjer", "dnager", "dangr", "danqer"} },
        { "warning", {"warning", "waring", "warniing", "warnng", "warninq"} },
        { "explosive", {"explosive", "explozive", "explosiv", "exlosive", "expl0sive"} },
        { "hazard", {"hazard", "hazerd", "hazrd", "hazzard", "h4zard"} },
        { "acid", {"acid", "acld", "asid", "aced", "4cid", "açid"} },
        { "flammable", {"flammable", "flamable", "flmmable", "flamible", "flamabl"} },
        { "toxic", {"toxic", "toxik", "toxsic", "tox1c", "toxıc"} },
        { "stop", {"stop", "st0p", "stpp", "s+op"} },
        { "alert", {"alert", "alart", "alrt", "aleert"} },
        { "fire", {"fire", "fier", "f1re", "fyre", "fiire"} },
        { "biohazard", {"biohazard", "biohazerd", "bio hzr", "biohaz", "bio hazaard"} },
        { "danger zone", {"danger zone", "danjer zone", "dnager zone", "dangr zone"} },
        { "emergency", {"emergency", "emergncy", "emergenccy", "emergeny"} },
        { "keep out", {"keep out", "kep out", "keepot", "keepout", "keepp out"} },
        { "evacuate", {"evacuate", "evakvate", "evacuet", "evacuat"} },
        { "hazmat", {"hazmat", "hazmatt", "hazmt", "hazmaat"} },
        { "chemical", {"chemical", "kemikal", "chemcal", "cheemical"} },
        { "shock", {"shock", "shok", "sh0ck", "shawk"} },
        { "electrical hazard", {"electrical hazard", "elektrikal hazard", "elec hazard", "electric hazard"} },
        { "high voltage", {"high voltage", "hi voltage", "high voltaje", "hıgh voltage"} },
        { "warning sign", {"warning sign", "warniing sign", "warnng sign", "waring sign"} },
        { "lethal", {"lethal", "lethl", "lethaal"} },
        { "alarm", {"alarm", "alrm", "alaram"} },
        { "poison", {"poison", "poıson", "poısonn", "poizon"} },
        { "gas", {"gas", "gaz", "gass"} },
        { "radiation", {"radiation", "rad1ation", "radiat1on", "radyation"} },
        { "critical", {"critical", "cr1tical", "crtical", "crıtical"} },
        { "fragile", {"fragile", "fragle", "fragel", "fr4gile"} },
        { "sharp", {"sharp", "sh4rp", "sharpp"} },
        { "hot surface", {"hot surface", "h0t surface", "hot surfce"} },
        { "restricted", {"restricted", "restrcted", "restircted"} },
        { "do not enter", {"do not enter", "dont enter", "do not entar"} },
        { "authorized personnel only", {"authorized personnel only", "authorized only", "auth personnel only"} },
        { "crush hazard", {"crush hazard", "crsh hazard", "crush hazrd"} },
        { "burn hazard", {"burn hazard", "burn hazrd", "burn hzr"} },
        { "fall hazard", {"fall hazard", "fal hazard", "f4ll hazard"} },
        { "no entry", {"no entry", "no entri", "no enter"} },
        { "no trespassing", {"no trespassing", "no trespasin", "no trespasing"} },
        { "lockdown", {"lockdown", "lokdown", "lockdwn"} },
        { "risk of death", {"risk of death", "r1sk of death", "risk death"} },
        { "be safe lock it out", {"be safe lock it out", "be safe lock-out", "lock it out", "lock-it-out"} },
        { "stay clear", {"stay clear", "stay cl3ar", "staycleer"} },
        { "safety first", {"safety first", "safety frst", "safty first"}},
        { "safety", {"sftey", "safty"}}
    };

    QString lowered = originalText.toLower();

    for (auto it = keywordMap.begin(); it != keywordMap.end(); ++it) {
        const QStringList &variants = it.value();

        for (const QString &variant : variants) {
            if (lowered.contains(variant)) {
                emit threatStatusChanged(it.key(), true);
                return;
            }
        }
    }

    emit threatStatusChanged("", false);
}


void Backend::clearLog() {
    emit logUpdated("");  // QML tarafındaki log alanını temizle
}

void Backend::exportLog(const QString &logText) {
    QString dirPath = "/home/mana/projectOCR/logs";
    QString filename = QString("log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QString fullPath = dirPath + "/" + filename;

    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << logText;
        file.close();
        qDebug() << "Log dosyaya kaydedildi:" << fullPath;
    } else {
        qWarning() << "Log dosyası oluşturulamadı!";
    }
}


