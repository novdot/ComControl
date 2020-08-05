/*!
    \file setups.h
    \brief Заголовочный файл с подключенными настройками
    \author dnovikov
    \email novdot@mail.ru, novikov@electrooptika.ru
    \date 2019 12 20
 * */
#ifndef SETUPS_H
#define SETUPS_H

#if defined(PROFILE_ACCELEROMETR_ATO) //PROFILE_ACCELEROMETR_ATO
#include "device/formaccelato.h"
#define SETUPS_DISPATCHER_CONSTRUCTOR() \
    this->setWindowTitle(\
                tr("%3 %1 %2")\
                .arg(strProjName)\
                .arg(strProjVersion)\
                .arg("Accelerometr ATO")\
                );\
    m_strAbout.appendHtml(tr("Accelerometr ATO.")); \
    this->createTab();

#define SETUPS_COMITEM_CONSTRUCTOR() \
    this->setDevice("Accelerometr ATO");\
    m_pui->comboBox_item_com_setup_baudrate->setCurrentIndex(0);\
    m_pui->lineEdit_item_com_setup_baudrate->setText("9600");\
    m_pui->comboBox_item_com_setup_databits->setCurrentText("8");\
    m_pui->comboBox_item_com_setup_parity->setCurrentText("NoParity");\
    m_pui->comboBox_item_com_setup_stopbits->setCurrentText("OneStop");\
    m_pui->comboBox_item_com_setup_flowcontrol->setCurrentText("NoFlowControl");\
    m_pui->comboBox_item_com_setup_device->setCurrentText("");

#define SETUPS_COMITEM_DEVICE() \
    m_pDevice = new FormAccelAto();

#elif defined(PROFILE_AT_COMMANDS) //PROFILE_AT_COMMANDS
#define SETUPS_DISPATCHER_CONSTRUCTOR() 0
#define SETUPS_COMITEM_CONSTRUCTOR() 0
#define SETUPS_COMITEM_DEVICE() 0

#else // PROFILE_DEFAULT
#define SETUPS_DISPATCHER_CONSTRUCTOR() \
    this->setWindowTitle(\
                tr("%3 %1 %2")\
                .arg(strProjName)\
                .arg(strProjVersion)\
                .arg("Default")\
                );\
    m_strAbout.appendHtml(tr("Default profile.")); \
    this->createTab();

#define SETUPS_COMITEM_CONSTRUCTOR() \
    this->setDevice("Default");\
    m_pui->comboBox_item_com_setup_baudrate->setCurrentIndex(0);\
    m_pui->lineEdit_item_com_setup_baudrate->setText("9600");\
    m_pui->comboBox_item_com_setup_databits->setCurrentText("8");\
    m_pui->comboBox_item_com_setup_parity->setCurrentText("NoParity");\
    m_pui->comboBox_item_com_setup_stopbits->setCurrentText("OneStop");\
    m_pui->comboBox_item_com_setup_flowcontrol->setCurrentText("NoFlowControl");\
    m_pui->comboBox_item_com_setup_device->setCurrentText("");

#define SETUPS_COMITEM_DEVICE() \
    m_pDevice = new COMDeviceBase();

#endif // PROFILE_DEFAULT

#endif // SETUPS_H
