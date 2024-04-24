#DEFINES += USE_CONTROL_MSG_UDP_PROTOCOL

TEMPLATE = app
CONFIG += c++17
QT += qml quick widgets network gui

LIB_PATH_WIN = "$$PWD/../kpp_libs/win"
LIB_PATH_LINUX = "$$PWD/../kpp_libs/linux"

win32 {
    LIB_PATH = $$LIB_PATH_WIN

    DEFINES += WINDOWS

    INCLUDEPATH += $$LIB_PATH

    INCLUDEPATH += $$LIB_PATH/SDL2-2.0.9/x86_64-w64-mingw32/include
    QMAKE_LIBDIR += $$LIB_PATH/SDL2-2.0.9/x86_64-w64-mingw32/bin

    #INCLUDEPATH += $$LIB_PATH/SDL2-2.0.9/i686-w64-mingw32/include
    #QMAKE_LIBDIR += $$LIB_PATH/SDL2-2.0.9/i686-w64-mingw32/bin

    INCLUDEPATH += $$LIB_PATH/QMapControl/include
    QMAKE_LIBDIR += $$LIB_PATH/QMapControl/bin

    #INCLUDEPATH += $$LIB_PATH/QMapControl_32/include
    #QMAKE_LIBDIR += $$LIB_PATH/QMapControl_32/bin

    INCLUDEPATH += $$LIB_PATH/ftpclient/include
    QMAKE_LIBDIR += $$LIB_PATH/ftpclient/lib

    INCLUDEPATH += $$LIB_PATH/ffmpeg-n4.4/include
    QMAKE_LIBDIR += $$LIB_PATH/ffmpeg-n4.4/bin

    INCLUDEPATH += $$LIB_PATH/zlib/include
    QMAKE_LIBDIR += $$LIB_PATH/zlib/lib

    INCLUDEPATH += $$LIB_PATH/libzip/include
    QMAKE_LIBDIR += $$LIB_PATH/libzip/lib

    INCLUDEPATH += $$LIB_PATH/LibJpeg
    QMAKE_LIBDIR += $$LIB_PATH/LibJpeg/libjpeg/lib64

    QMAKE_LIBS += -lavformat-58 -lavcodec-58 -lswscale-5 -lavutil-56 -lqmapcontrol0 -lFTP -lftpclient -lzlib -lzip -lcurl

}
unix {
    LIB_PATH = $$LIB_PATH_LINUX

    INCLUDEPATH += $$LIB_PATH/ffmpeg-440/include
    QMAKE_LIBDIR += $$LIB_PATH/ffmpeg-440/bin

    LIBS += -L$$LIB_PATH/QMapControl
    INCLUDEPATH += $$LIB_PATH/QMapControl

    INCLUDEPATH += $$LIB_PATH/ftpclient/include
    QMAKE_LIBDIR += $$LIB_PATH/ftpclient/lib
    #LIBS+= $$LIB_PATH/ftpclient/lib/libftpclient.a
    #LIBS+= $$LIB_PATH/ftpclient/lib/libFTP.a


    INCLUDEPATH += $$LIB_PATH/zlib/include
    QMAKE_LIBDIR += $$LIB_PATH/zlib/lib

    INCLUDEPATH += $$LIB_PATH/libzip/include
    QMAKE_LIBDIR += $$LIB_PATH/libzip/lib

#    LIBS += -lcurl
#    LIBS += -L/usr/lib -ljpeg

    # это не работает, в системе надо установить libzip-dev
    #INCLUDEPATH += $$LIB_PATH/libzip/include
    #LIBS += $$LIB_PATH/libzip/lib/libzip.so
    QMAKE_LIBS += -lavformat -lavcodec -lswscale -lavutil -lqmapcontrol
}

QMAKE_LIBS += -lgomp -lSDL2

IIPCT_include = $$LIB_PATH/IIPCT/include
IIPCT_libs = $$LIB_PATH/IIPCT/lib

INCLUDEPATH += $$IIPCT_include/
LIBS += $$IIPCT_libs/libTracking.a
LIBS += $$IIPCT_libs/libPrimitives.a
LIBS += $$IIPCT_libs/libAlgorithms.a
LIBS += $$IIPCT_libs/libKeyPoints.a
LIBS += $$IIPCT_libs/libMachineLearning.a
LIBS += $$IIPCT_libs/libImageProcessing.a
LIBS += $$IIPCT_libs/libUtility.a

SOURCES += main.cpp \
    imageProvider.cpp \
    session.cpp \
    ffmpeg.cpp \

RESOURCES += \
    rc.qrc

HEADERS += \
    imageProvider.h \
    session.h \
    ffmpeg.h \

DISTFILES += \
    qml/noSignal.jpg
