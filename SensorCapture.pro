QT += quick
QT += widgets
QT += bluetooth
QT += charts
CONFIG += c++11

FORMS += forms/mainwindow.ui \
    forms/devicepanel.ui \
    forms/devicediscoverywizardpage.ui \
    forms/deviceselectwizardpage.ui

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

include(3rdparty/mbientlab.pro)

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    forms/mainwindow.cpp \
    forms/devicewizard.cpp \
    forms/devicediscoverywizardpage.cpp \
    common/devicehandler.cpp \
    common/metawearhandler.cpp \
    forms/deviceselectwizardpage.cpp

RESOURCES += qml.qrc \
    icons.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH = gui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    forms/mainwindow.h \
    common/devices/graillabpackets.h \
    forms/devicewizard.h \
    forms/devicediscoverywizardpage.h \
    common/devicehandler.h \
    common/metawearhandler.h \
    forms/deviceselectwizardpage.h
