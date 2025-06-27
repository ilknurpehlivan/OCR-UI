#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "imageProvider.h"
#include "cameraCapture.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Image provider'ı oluştur ve QML'e tanıt
    auto *provider = new ImageProvider();
    engine.addImageProvider("live", provider);

    // Kamera worker'ı başlat
    CameraWorker *worker = new CameraWorker(provider);

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
