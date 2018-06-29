#-------------------------------------------------
#
# Project created by QtCreator 2018-04-14T22:57:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = socketClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        dialog.cpp \
    dialog.cpp \
    main.cpp \
    login.cpp \
    global.cpp \
    reg.cpp \
    changepassword.cpp \
    chat.cpp

HEADERS += \
        dialog.h \
    dialog.h \
    ui_dialog.h \
    login.h \
    global.h \
    reg.h \
    changepassword.h \
    chat.h

FORMS += \
        dialog.ui \
    login.ui \
    reg.ui \
    changepassword.ui \
    chat.ui

RESOURCES += \
    icons.qrc

DISTFILES +=
#INCLUDEPATH += C:/opencv/build/include
#INCLUDEPATH += C:/opencv/build/include/opencv
#INCLUDEPATH += C:/opencv/build/include/opencv2
#LIBS += C:/opencv/build/x64/vc14/lib/opencv_world341.lib \
#        C:/opencv/build/x64/vc14/lib/opencv_world341d.lib
