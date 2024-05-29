#include "session.h"

#include <QApplication>
//#include <QCalendar>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QtQml>
#include <qicon.h>

int main(int argc, char *argv[])
{
    QQuickWindow::setDefaultAlphaBuffer(true);

    QApplication app(argc, argv);

    if (QDateTime::currentDateTime().date().dayOfWeek() == Qt::Wednesday)
        app.setWindowIcon(QIcon(":/ico/alt_icon.png"));
    else
        app.setWindowIcon(QIcon(":/ico/icon.png"));

    Session session;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("session", &session);
    engine.rootContext()->setContextProperty("CameraImage", session.camera_);
    engine.addImageProvider(QLatin1String("camera"), session.camera_);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    int ret = app.exec();

    return ret;
}
