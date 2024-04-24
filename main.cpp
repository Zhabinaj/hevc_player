#include "session.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Session session;

  QQmlApplicationEngine engine;

  engine.rootContext()->setContextProperty("session", &session);
  engine.rootContext()->setContextProperty("CameraImage", session.camera);
  engine.addImageProvider(QLatin1String("camera"), session.camera);
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  int ret = app.exec();

  return ret;
}
