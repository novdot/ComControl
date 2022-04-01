######################################################
# path
ROOT_PATH = ..

SOURCE_PATH = $$ROOT_PATH/source
FORMS_PATH = $$ROOT_PATH/source
INCLUDE_PATH = $$ROOT_PATH/source
LIB_PATH = $$ROOT_PATH/libs
######################################################
#libs
LIBS += -L$$LIB_PATH/ -lDeviceControl

INCLUDEPATH += $$LIB_PATH/include
DEPENDPATH += $$LIB_PATH/include
######################################################
#version by git
#git tag v1.0
#always run qmake
DEFINES += GIT_VERSION=\\\"$$system($$quote(git describe --tags))\\\"
DEFINES += GIT_TIMESTAMP=\\\"$$system($$quote(git log -n 1 --format=format:\\\"%ai\\\"))\\\"

######################################################
#qt variables
#on ubuntu install libqt5serialport5-dev package
QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console

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
    $$SOURCE_PATH/comcontrol.cpp \
    #../../libDeviceControl/source/protocolinterface.cpp

HEADERS += \
    $$INCLUDE_PATH/comdispatcher.h \
    $$INCLUDE_PATH/comitem.h \
    $$INCLUDE_PATH/comcontrol.h \
    $$INCLUDE_PATH/setups.h

FORMS += \
    $$FORMS_PATH/comdispatcher.ui \
    $$FORMS_PATH/formcomcontrol.ui \
    $$FORMS_PATH/formcomitem.ui

######################################################
# Подключаемые модули.
# Поумолчанию всегда подключен модуль DeviceControl
#
######################################################
CONFIG += \
    device_boot
    #device_accel_ato

CONFIG(device_all_enabled): {
CONFIG += device_accel_ato
CONFIG += device_at_commands
CONFIG += device_dpb
CONFIG += device_gld
CONFIG += device_boot
}
######################################################
CONFIG(device_accel_ato): {
LIBS += -L$$LIB_PATH/ -lDeviceAto
DEFINES += DEVICE_ACCELEROMETR_ATO
}
######################################################
CONFIG(device_at_commands): {
LIBS += -L$$LIB_PATH/ -lDeviceAtCom
DEFINES += DEVICE_AT_COMMANDS
}
######################################################
CONFIG(device_dpb): {
LIBS += -L$$LIB_PATH/ -lDeviceDpb
DEFINES += DEVICE_DPB
}
######################################################
CONFIG(device_gld): {
LIBS += -L$$LIB_PATH/ -lDeviceGld
DEFINES += DEVICE_GLD
}
######################################################
CONFIG(device_boot): {
LIBS += -L$$LIB_PATH/ -lDeviceBoot
DEFINES += DEVICE_BOOT
}
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
