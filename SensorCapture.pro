TARGET = SensorCapture

QT += quick
QT += widgets
QT += bluetooth
QT += printsupport

macx {

    INCLUDEPATH += platform/osx

    SOURCES += \
    platform/osx/common/metawearwrapper.cpp

    HEADERS += \
    platform/osx/common/metawearwrapper.h

    CONFIG += c++11
}

unix:!macx {
    LIBS += -Llibz -lz

    INCLUDEPATH += platform/linux

    SOURCES += \
    platform/linux/common/metawearwrapper.cpp

    HEADERS += \
    platform/linux/common/metawearwrapper.h

    CONFIG += c++11
}
win32 {
    DEFINES += QUAZIP_STATIC
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib

    WINRT_MANIFEST.publisher = "GaitRehabilitation"
    WINRT_MANIFEST.description = "A utility capture for for MbientLab's MetaMotion sensors"
    WINRT_MANIFEST.publisherid = "CN=Gait Rehabilitation, O=Chapman Unversity, L=Orange, S=California, C=US"
    WINRT_MANIFEST.version = "0.0.0.1"

    INCLUDEPATH += platform/windows

    SOURCES += \
    platform/windows/common/metawearwrapper.cpp

    HEADERS += \
    platform/windows/common/metawearwrapper.h

    # configure for C++17 features when using winRT library
    CONFIG += c++17
    QMAKE_CXXFLAGS += /await
}



FORMS += forms/mainwindow.ui \
    forms/deviceselectdialog.ui \
    forms/sensorpanel.ui

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

include(3rdparty/mbientlab.pro)
include(3rdparty/quazip/quazip.pri)
include(3rdparty/qmsgpack/qmsgpack.pri)

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    qcustomplot.cpp \
    common/main.cpp \
    forms/mainwindow.cpp \
    forms/deviceselectdialog.cpp \
    forms/sensorpanel.cpp \
    common/util.cpp \
    common/metawearwrapperbase.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH = gui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    qcustomplot.h \
    forms/mainwindow.h \
    forms/deviceselectdialog.h \
    forms/sensorpanel.h \
    common/util.h \
    common/metawearwrapperbase.h

