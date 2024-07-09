#include "session.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QtQml>
#include <qicon.h>

/*! @mainpage Hevc Player Documentation

 @section intro_sec Introduction

  <br> You can use Hevc Player to opens, plays and saves .hevc videofiles with or without SEI data.
  <br>
  <br> The main functionality is based on working with the ffmpeg library.
  <br> GUI is QML-based.
  <br>
  <br> Release version for Linux tested on Ubuntu 20.04.6
  <br> Release version for Windows tested on Windows 10 x64

  @section install_sec Installation

  <br> 1: For using app on linux download and run hevc_payer.AppImage
  <br> 2: For using app on windows download hevc_player_win.zip and run hevc_payer.exe
  <br> 3: For using source code download/clone hevc_player folder and change the path to kpp_libs in CMakeLists.txt
 */

int main(int argc, char *argv[])
{
    QQuickWindow::setDefaultAlphaBuffer(true);

    QApplication app(argc, argv);

    app.setOrganizationName("AME");
    app.setOrganizationDomain("339");

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
