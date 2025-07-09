#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "imageProvider.h"
#include "cameraCapture.h"
#include "backend.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // ImageProvider oluştur
    auto *provider = new ImageProvider();
    engine.addImageProvider("live", provider);

    Backend *backend = new Backend(provider);
    engine.rootContext()->setContextProperty("backend", backend);

    CameraCapture *cameraCapture = new CameraCapture(provider, backend);
    engine.rootContext()->setContextProperty("cameraCapture", cameraCapture);
    cameraCapture->start();  // kamera başlatılıyor

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
