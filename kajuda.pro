#-------------------------------------------------
#
# Project created by QtCreator 2016-11-12T14:47:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kajuda
TEMPLATE = app


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        levelmeter.cpp

HEADERS  += \
        mainwindow.h \
        levelmeter.h \
        input_handling.h



FORMS    += mainwindow.ui


LIBS += -lportaudio -lsndfile

RESOURCES += \
    res/res.qrc

HEADERS += \
    input_handling.h

QMAKE_CXXFLAGS += -std=c++11

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lportaudio
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lportaudio
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lportaudio

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/release/libportaudio.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/debug/libportaudio.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/release/portaudio.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/debug/portaudio.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/libportaudio.a
