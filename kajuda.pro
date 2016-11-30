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
        audiofile.cpp \
        audioinputfile.cpp

HEADERS  += \
        mainwindow.h \
        levelmeter.h \
        audiofile.h \
        audioinputfile.h



FORMS    += mainwindow.ui


LIBS += -lportaudio -lsndfile

RESOURCES += \
    res/res.qrc

HEADERS +=

QMAKE_CXXFLAGS += -std=c++11

