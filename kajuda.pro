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
        levelmeter.cpp \
        audiofile.cpp

HEADERS  += \
        mainwindow.h \
        levelmeter.h \
        input_handling.h \
        audiofile.h



FORMS    += mainwindow.ui


LIBS += -lportaudio -lsndfile

RESOURCES += \
    res/res.qrc

HEADERS += \
    input_handling.h

QMAKE_CXXFLAGS += -std=c++11

