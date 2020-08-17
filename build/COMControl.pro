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
    $$SOURCE_PATH/comcontrol.cpp \
    $$SOURCE_PATH/comdevicebase.cpp

HEADERS += \
    $$SOURCE_PATH/comdispatcher.h \
    $$SOURCE_PATH/comitem.h \
    $$SOURCE_PATH/comcontrol.h \
    $$SOURCE_PATH/comdevicebase.h \
    $$SOURCE_PATH/comdeviceinterface.h \
    $$SOURCE_PATH/setups.h

FORMS += \
    $$SOURCE_PATH/comdispatcher.ui \
    $$SOURCE_PATH/formcomdevicebase.ui \
    $$SOURCE_PATH/formcomcontrol.ui \
    $$SOURCE_PATH/formcomitem.ui
######################################################
CONFIG(device_all_enabled): {
CONFIG += device_accel_ato
CONFIG += device_at_commands
CONFIG += device_dpb_commands
}
######################################################
CONFIG(accel_ato): {
CONFIG += device_accel_ato
DEFINES += PROFILE_ACCELEROMETR_ATO
}
CONFIG(device_accel_ato): {
DEFINES += DEVICE_ACCELEROMETR_ATO
SOURCES += $$SOURCE_PATH/device/formaccelato.cpp
HEADERS += $$SOURCE_PATH/device/formaccelato.h
FORMS += $$SOURCE_PATH/device/formaccelato.ui
}
######################################################
CONFIG(at_commands): {
DEFINES += PROFILE_AT_COMMANDS
CONFIG += device_at_commands
}
CONFIG(device_at_commands): {
DEFINES += DEVICE_AT_COMMANDS
SOURCES += $$SOURCE_PATH/device/formatcommands.cpp
HEADERS += $$SOURCE_PATH/device/formatcommands.h
FORMS += $$SOURCE_PATH/device/formatcommands.ui
}
######################################################
CONFIG(dpb_commands): {
DEFINES += PROFILE_DPB_COMMANDS
CONFIG += device_dpb_commands
}
CONFIG(device_dpb_commands): {
DEFINES += DEVICE_DPB_COMMANDS
SOURCES += $$SOURCE_PATH/device/formdpbcommands.cpp
HEADERS += $$SOURCE_PATH/device/formdpbcommands.h
FORMS += $$SOURCE_PATH/device/formdpbcommands.ui
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
