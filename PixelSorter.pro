#-------------------------------------------------
#
# Project created by QtCreator 2018-08-15T12:20:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PixelSorter
TEMPLATE = app
CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    sortingthread.cpp \
    optionsdialog.cpp \
    sorter.cpp \
    openthread.cpp \
    savethread.cpp \
    avaliablefunclistwidget.cpp \
    orderfunclistwidget.cpp \
    imagewidget.cpp

HEADERS += \
        mainwindow.h \
    sortingthread.h \
    optionsdialog.h \
    sorter.h \
    openthread.h \
    savethread.h \
    avaliablefunclistwidget.h \
    orderfunclistwidget.h \
    imagewidget.h

FORMS += \
        mainwindow.ui \
    optionsdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/x64/vc15/lib/ -lopencv_world342
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/x64/vc15/lib/ -lopencv_world342d
else:unix: LIBS += -L$$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/x64/vc15/lib/ -lopencv_world342

INCLUDEPATH += $$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/include
DEPENDPATH += $$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/x64/vc15/lib/libopencv_world342.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/x64/vc15/lib/libopencv_world342d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/x64/vc15/lib/opencv_world342.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/x64/vc15/lib/opencv_world342d.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../Libraries/opencv/opencv_vc15_x64/opencv/build/x64/vc15/lib/libopencv_world342.a


QMAKE_CXXFLAGS += /openmp

RESOURCES += \
    res/images.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/opencv_vc14_x32/install/x86/vc15/lib/ -lopencv_core342
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/opencv_vc14_x32/install/x86/vc15/lib/ -lopencv_core342d

INCLUDEPATH += $$PWD/../../Libraries/opencv/opencv_vc14_x32/install/include
DEPENDPATH += $$PWD/../../Libraries/opencv/opencv_vc14_x32/install/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/opencv_vc14_x32/install/x86/vc15/lib/ -lopencv_imgproc342
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Libraries/opencv/opencv_vc14_x32/install/x86/vc15/lib/ -lopencv_imgproc342d

INCLUDEPATH += $$PWD/../../Libraries/opencv/opencv_vc14_x32/install/include
DEPENDPATH += $$PWD/../../Libraries/opencv/opencv_vc14_x32/install/include

DISTFILES += \
    todo.md
