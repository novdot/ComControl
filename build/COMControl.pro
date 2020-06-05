######################################################
# path
ROOT_PATH = ..

SOURCE_PATH = $$ROOT_PATH/source
FORMS_PATH = $$ROOT_PATH/source
INCLUDE_PATH = $$ROOT_PATH/source
######################################################
#libs
LIBS +=
INCLUDEPATH +=
DEPENDPATH +=
######################################################
#qt variables
QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = comcontrol

DESTDIR = $$ROOT_PATH/bin
MOC_DIR = $$ROOT_PATH/temp/moc
OBJECTS_DIR = $$ROOT_PATH/temp/obj
RCC_DIR = $$ROOT_PATH/temp/rcc
UI_DIR = $$ROOT_PATH/temp/uic

######################################################
#defines

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

######################################################
SOURCES += \
    $$SOURCE_PATH/comdispatcher.cpp \
    $$SOURCE_PATH/comitem.cpp \
    $$SOURCE_PATH/main.cpp \
    ../source/comcontrol.cpp \
    ../source/device/comdevicebase.cpp

HEADERS += \
    $$SOURCE_PATH/comdispatcher.h \
    $$SOURCE_PATH/comitem.h \
    ../source/comcontrol.h \
    ../source/device/comdevicebase.h \
    ../source/device/comdeviceinterface.h

FORMS += \
    $$SOURCE_PATH/comdispatcher.ui \
    ../source/device/formcomdevicebase.ui \
    ../source/formcomcontrol.ui \
    ../source/formcomitem.ui

######################################################
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: {
        RC_ICONS = ../res/serial_port_icon.ico
        RC_FILE += ../res/res.rc
}

RESOURCES += \
    ../res/resurce.qrc
