#include "session.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QtQml>
#include <qicon.h>

int main(int argc, char *argv[])
{
    QQuickWindow::setDefaultAlphaBuffer(true);

    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/ico/icon.png"));
    Session session;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("session", &session);
    engine.rootContext()->setContextProperty("CameraImage", session.camera);
    engine.addImageProvider(QLatin1String("camera"), session.camera);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    int ret = app.exec();

    return ret;
}
