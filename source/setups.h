/*!
    \file setups.h
    \brief Заголовочный файл с подключенными настройками
    \author dnovikov
    \email novdot@mail.ru, novikov@electrooptika.ru
    \date 2019 12 20
 * */
#ifndef SETUPS_H
#define SETUPS_H

#define SETUPS_DEFAULT_NAME         ("DEFAULT")
#define SETUPS_DEFAULT_baudrate     ("9600")
#define SETUPS_DEFAULT_databits     ("8")
#define SETUPS_DEFAULT_parity       ("NoParity")
#define SETUPS_DEFAULT_stopbits     ("OneStop")
#define SETUPS_DEFAULT_flowcontrol  ("NoFlowControl")
#define SETUPS_DEFAULT_CLASS        FormProtocolBase
/////////////////////////////////////////////////////////////////////////////////
//Include available devices
#if defined(DEVICE_ACCELEROMETR_ATO)
    #include <formaccelato.h>
#endif
#if defined(DEVICE_AT_COMMANDS)
    #include <formatcommands.h>
#endif
#if defined(DEVICE_DPB_COMMANDS)
    #include <formdpbcommands.h>
#endif

/////////////////////////////////////////////////////////////////////////////////
//Current profile on user. Only 1 profile available on time
#if defined(PROFILE_ACCELEROMETR_ATO) //PROFILE_ACCELEROMETR_ATO
#define SETUPS_NAME         SETUPS_ACCELEROMETR_ATO_NAME
#define SETUPS_baudrate     SETUPS_ACCELEROMETR_ATO_baudrate
#define SETUPS_databits     SETUPS_ACCELEROMETR_ATO_databits
#define SETUPS_parity       SETUPS_ACCELEROMETR_ATO_parity
#define SETUPS_stopbits     SETUPS_ACCELEROMETR_ATO_stopbits
#define SETUPS_flowcontrol  SETUPS_ACCELEROMETR_ATO_flowcontrol
#define SETUPS_CLASS        SETUPS_ACCELEROMETR_ATO_CLASS
#elif defined(PROFILE_AT_COMMANDS) //PROFILE_AT_COMMANDS
#define SETUPS_NAME         ("DEFAULT")
#define SETUPS_baudrate     ("9600")
#define SETUPS_databits     ("8")
#define SETUPS_parity       ("NoParity")
#define SETUPS_stopbits     ("OneStop")
#define SETUPS_flowcontrol  ("NoFlowControl")
#define SETUPS_CLASS        FormProtocolBase
#elif defined(PROFILE_DPB_COMMANDS) //PROFILE_DPB_COMMANDS
#define SETUPS_NAME         ("DEFAULT")
#define SETUPS_baudrate     ("9600")
#define SETUPS_databits     ("8")
#define SETUPS_parity       ("NoParity")
#define SETUPS_stopbits     ("OneStop")
#define SETUPS_flowcontrol  ("NoFlowControl")
#define SETUPS_CLASS        FormProtocolBase
#else // PROFILE_DEFAULT
#define SETUPS_NAME         SETUPS_DEFAULT_NAME
#define SETUPS_baudrate     SETUPS_DEFAULT_baudrate
#define SETUPS_databits     SETUPS_DEFAULT_databits
#define SETUPS_parity       SETUPS_DEFAULT_parity
#define SETUPS_stopbits     SETUPS_DEFAULT_stopbits
#define SETUPS_flowcontrol  SETUPS_DEFAULT_flowcontrol
#define SETUPS_CLASS        SETUPS_DEFAULT_CLASS
#endif // PROFILE_DEFAULT

//this->setWindowIcon(QIcon(":icon/serial_port_icon.png"));
#define SETUPS_DISPATCHER_CONSTRUCTOR() \
    this->setWindowTitle(\
                tr("%3 %1 %2")\
                .arg(strProjName)\
                .arg(strProjVersion)\
                .arg(tr(SETUPS_NAME))\
                );\
    m_strAbout.appendHtml(tr(SETUPS_NAME)); \
    this->createTab();

#define SETUPS_COMITEM_CONSTRUCTOR() \
    m_pui->comboBox_item_com_setup_baudrate->setCurrentIndex(0);\
    m_pui->lineEdit_item_com_setup_baudrate->setText(SETUPS_baudrate);\
    m_pui->comboBox_item_com_setup_databits->setCurrentText(SETUPS_databits);\
    m_pui->comboBox_item_com_setup_parity->setCurrentText(SETUPS_parity);\
    m_pui->comboBox_item_com_setup_stopbits->setCurrentText(SETUPS_stopbits);\
    m_pui->comboBox_item_com_setup_flowcontrol->setCurrentText(SETUPS_flowcontrol);\
    m_pui->comboBox_item_com_setup_device->setCurrentText("");

#define SETUPS_COMITEM_DEVICE() \
    m_pDevice = new SETUPS_CLASS();

/////////////////////////////////////////////////////////////////////////////////
#endif // SETUPS_H
