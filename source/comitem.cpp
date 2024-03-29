#include "comitem.h"
#include "ui_formcomitem.h"

#include "formprotocolbase.h"

#include "setups.h"

#include <QMessageBox>
#include <QTextCodec>
#include <QFileDialog>
#include <QThread>

/*****************************************************************************/
ComItem::ComItem(QWidget *parent)
    : QWidget(parent)
    , m_pui(new Ui::FormComItem)
{
    m_pui->setupUi(this);

    //m_settings set default

    m_state = _state_setup;
    m_bIsLogger = false;
    m_pDevice = nullptr;

    m_pui->pushButton_item_com_control_log->toggled(true);

    this->initLists();
    this->initSignalSlotConn();
    this->fillSetupFields();
    this->updateSetupFields();
    this->initStatus();

    m_timService.setInterval(1000);
    m_timService.start();

    m_log.setWindowFlags(Qt::WindowStaysOnTopHint);

    connect(m_pui->comboBox_item_com_setup_device, SIGNAL(currentIndexChanged(QString))
            , this, SLOT(setupsDeviceConnections(QString)));

    m_pui->comboBox_item_com_setup_device->addItem(SETUPS_DEFAULT_NAME);
#if defined(DEVICE_ACCELEROMETR_ATO)
    m_pui->comboBox_item_com_setup_device->addItem(SETUPS_ACCELEROMETR_ATO_NAME);
#endif
#if defined(DEVICE_AT_COMMANDS)
    m_pui->comboBox_item_com_setup_device->addItem(SETUPS_AT_COMMANDS_NAME);
#endif
#if defined(DEVICE_DPB_COMMANDS)
    m_pui->comboBox_item_com_setup_device->addItem(SETUPS_DPB_COMMANDS_NAME);
#endif
#if defined(DEVICE_BOOT)
    m_pui->comboBox_item_com_setup_device->addItem(SETUPS_BOOT_NAME);
#endif
#if defined(DEVICE_DPB)
    m_pui->comboBox_item_com_setup_device->addItem(SETUPS_DPB_NAME);
#endif
    //if compiled with profile
    //SETUPS_COMITEM_CONSTRUCTOR();
}

/*****************************************************************************/
void ComItem::console(QList< QPair<QString,QString > > commands)
{
    QPair<QString,QString > one_line;

    //get COM params
    //check command line
    for(int i=0; i<commands.count(); i++ ){
        one_line = commands.at(i);
        if(one_line.first==CONSOLE_CMD_SETUPS){
            qDebug()<<"Load COM port params from file:"<<one_line.second;
            if(!loadConfigFile(one_line.second)){
                qDebug()<<"Got some errors while load params! Check conf file! Exit.";
                goto fail;
            }
        }
    }

    //if get params try to connect
    qDebug()<<"Start connect to COM port...";
    startConnect();

    //send to device commands
    if(m_strDeviceName==SETUPS_DEFAULT_NAME) ((SETUPS_DEFAULT_CLASS*)m_pDevice)->console(commands);
#if defined(DEVICE_ACCELEROMETR_ATO)
    if(m_strDeviceName==SETUPS_ACCELEROMETR_ATO_NAME) m_pDevice = new SETUPS_ACCELEROMETR_ATO_CLASS();
#endif
#if defined(DEVICE_AT_COMMANDS)
    if(m_strDeviceName==SETUPS_NAME_AT_COMMANDS) m_pDevice = new FormProtocolBase();
#endif
#if defined(DEVICE_DPB_COMMANDS)
    if(m_strDeviceName==SETUPS_NAME_DPB_COMMANDS) m_pDevice = new FormProtocolBase();
#endif
#if defined(DEVICE_BOOT)
    if(m_strDeviceName==SETUPS_BOOT_NAME) ((SETUPS_BOOT_CLASS*)m_pDevice)->console(commands);
#endif
#if defined(DEVICE_DPB)
    if(m_strDeviceName==SETUPS_DPB_NAME) ((SETUPS_DPB_CLASS*)m_pDevice)->console(commands);
#endif


fail:;
}
/*****************************************************************************/
void ComItem::initStatus()
{
    m_pui->label_status_connected->setPixmap(QPixmap(":icon/led_off.png"));
    m_pui->label_status_rx->setPixmap(QPixmap(":icon/led_off.png"));
    m_pui->label_status_tx->setPixmap(QPixmap(":icon/led_off.png"));
    m_pui->label_status_error->setPixmap(QPixmap(":icon/led_off.png"));
}

/*****************************************************************************/
void ComItem::statusErrorOn(QSerialPort::SerialPortError error)
{
    if(error==QSerialPort::NoError)
        statusErrorOff();
    else
        m_pui->label_status_error->setPixmap(QPixmap(":icon/led_red.png"));
}
/*****************************************************************************/
void ComItem::statusErrorOff()
{
    m_pui->label_status_error->setPixmap(QPixmap(":icon/led_off.png"));
}
/*****************************************************************************/
void ComItem::statusTxOn()
{
    m_pui->label_status_tx->setPixmap(QPixmap(":icon/led_green.png"));
}
/*****************************************************************************/
void ComItem::statusTxOff()
{
    m_pui->label_status_tx->setPixmap(QPixmap(":icon/led_off.png"));
}
/*****************************************************************************/
void ComItem::statusRxOn()
{
    m_pui->label_status_rx->setPixmap(QPixmap(":icon/led_green.png"));
}
/*****************************************************************************/
void ComItem::statusRxOff()
{
    m_pui->label_status_rx->setPixmap(QPixmap(":icon/led_off.png"));
}
/*****************************************************************************/
void ComItem::statusConnectOn()
{
    m_pui->label_status_connected->setPixmap(QPixmap(":icon/led_green.png"));
}
/*****************************************************************************/
void ComItem::statusConnectOff()
{
    m_pui->label_status_connected->setPixmap(QPixmap(":icon/led_off.png"));
}

/*****************************************************************************/
void ComItem::setupsDeviceConnections(QString a_index)
{
    QString strBaudrate(SETUPS_DEFAULT_baudrate);
    QString strDatabits(SETUPS_DEFAULT_databits);
    QString strParity(SETUPS_DEFAULT_parity);
    QString strStopbits(SETUPS_DEFAULT_stopbits);
    QString strFlowcontrol(SETUPS_DEFAULT_flowcontrol);

    QString strCurrentDev = a_index;

    if(strCurrentDev == SETUPS_DEFAULT_NAME) goto end;
#if defined(DEVICE_ACCELEROMETR_ATO)
    if(strCurrentDev == SETUPS_ACCELEROMETR_ATO_NAME) {
        strBaudrate=(SETUPS_ACCELEROMETR_ATO_baudrate);
        strDatabits=(SETUPS_ACCELEROMETR_ATO_databits);
        strParity=(SETUPS_ACCELEROMETR_ATO_parity);
        strStopbits=(SETUPS_ACCELEROMETR_ATO_stopbits);
        strFlowcontrol=(SETUPS_ACCELEROMETR_ATO_flowcontrol);
    }
#endif
#if defined(DEVICE_AT_COMMANDS)
    if(strCurrentDev == SETUPS_DEFAULT_NAME) {

    }
#endif
#if defined(DEVICE_DPB_COMMANDS)
    if(strCurrentDev == SETUPS_DEFAULT_NAME) {

    }
#endif
#if defined(DEVICE_BOOT)
    if(strCurrentDev == SETUPS_BOOT_NAME) {
        strBaudrate=(SETUPS_BOOT_baudrate);
        strDatabits=(SETUPS_BOOT_databits);
        strParity=(SETUPS_BOOT_parity);
        strStopbits=(SETUPS_BOOT_stopbits);
        strFlowcontrol=(SETUPS_BOOT_flowcontrol);
    }
#endif
#if defined(DEVICE_DPB)
    if(strCurrentDev == SETUPS_DPB_NAME) {
        strBaudrate=(SETUPS_DPB_baudrate);
        strDatabits=(SETUPS_DPB_databits);
        strParity=(SETUPS_DPB_parity);
        strStopbits=(SETUPS_DPB_stopbits);
        strFlowcontrol=(SETUPS_DPB_flowcontrol);
    }
#endif

end:
    m_pui->lineEdit_item_com_setup_baudrate->setText(strBaudrate);
    m_pui->comboBox_item_com_setup_databits->setCurrentText(strDatabits);
    m_pui->comboBox_item_com_setup_parity->setCurrentText(strParity);
    m_pui->comboBox_item_com_setup_stopbits->setCurrentText(strStopbits);
    m_pui->comboBox_item_com_setup_flowcontrol->setCurrentText(strFlowcontrol);
}
/*****************************************************************************/
#define __ADD_FIELD_TO_LIST(ID_X,NAME_X,LIST_X) \
    field.id = ID_X; \
    field.name = NAME_X; \
    LIST_X.append(field);

void ComItem::initLists()
{
    com_settings_field field;
    __ADD_FIELD_TO_LIST(0,__FIELD_MANUAL_BAUD,m_lBaud);
    __ADD_FIELD_TO_LIST(1200,"1200",m_lBaud);
    __ADD_FIELD_TO_LIST(2400,"2400",m_lBaud);
    __ADD_FIELD_TO_LIST(4800,"4800",m_lBaud);
    __ADD_FIELD_TO_LIST(9600,"9600",m_lBaud);
    __ADD_FIELD_TO_LIST(19200,"19200",m_lBaud);
    __ADD_FIELD_TO_LIST(38400,"38400",m_lBaud);
    __ADD_FIELD_TO_LIST(57600,"57600",m_lBaud);
    __ADD_FIELD_TO_LIST(115200,"115200",m_lBaud);
    __ADD_FIELD_TO_LIST(128000,"128000",m_lBaud);
    __ADD_FIELD_TO_LIST(230400,"230400",m_lBaud);
    __ADD_FIELD_TO_LIST(256000,"256000",m_lBaud);
    __ADD_FIELD_TO_LIST(460800,"460800",m_lBaud);
    __ADD_FIELD_TO_LIST(921600,"921600",m_lBaud);

    __ADD_FIELD_TO_LIST(QSerialPort::Data5,"5",m_lDatabits);
    __ADD_FIELD_TO_LIST(QSerialPort::Data6,"6",m_lDatabits);
    __ADD_FIELD_TO_LIST(QSerialPort::Data7,"7",m_lDatabits);
    __ADD_FIELD_TO_LIST(QSerialPort::Data8,"8",m_lDatabits);

    __ADD_FIELD_TO_LIST(QSerialPort::NoParity,"NoParity",m_lParity);
    __ADD_FIELD_TO_LIST(QSerialPort::EvenParity,"EvenParity",m_lParity);
    __ADD_FIELD_TO_LIST(QSerialPort::OddParity,"OddParity",m_lParity);
    __ADD_FIELD_TO_LIST(QSerialPort::SpaceParity,"SpaceParity",m_lParity);
    __ADD_FIELD_TO_LIST(QSerialPort::MarkParity,"MarkParity",m_lParity);

    __ADD_FIELD_TO_LIST(QSerialPort::OneStop,"OneStop",m_lStopbits);
    __ADD_FIELD_TO_LIST(QSerialPort::OneAndHalfStop,"OneAndHalfStop",m_lStopbits);
    __ADD_FIELD_TO_LIST(QSerialPort::TwoStop,"TwoStop",m_lStopbits);

    __ADD_FIELD_TO_LIST(QSerialPort::NoFlowControl,"NoFlowControl",m_lFlowcontrol);
    __ADD_FIELD_TO_LIST(QSerialPort::HardwareControl,"HardwareControl",m_lFlowcontrol);
    __ADD_FIELD_TO_LIST(QSerialPort::SoftwareControl,"SoftwareControl",m_lFlowcontrol);
}
/*****************************************************************************/
void ComItem::initSignalSlotConn()
{
    connect(m_pui->comboBox_item_com_choose,SIGNAL(currentIndexChanged(QString ))
            ,this ,SLOT(setComDescription(QString )));

    /*connect(&m_port, SIGNAL(readyRead())
            ,this ,SLOT(readPort()));*/

    connect(&m_port, SIGNAL(readyRead())
            ,this ,SLOT(statusRxOn()));
    connect(this, SIGNAL(sendedData())
            ,this ,SLOT(statusTxOn()));

    connect(&m_port, SIGNAL(errorOccurred(QSerialPort::SerialPortError))
            ,this ,SLOT(statusErrorOn(QSerialPort::SerialPortError)));

    connect( &m_port, SIGNAL( error(QSerialPort::SerialPortError) )
             , this, SLOT( handleError(QSerialPort::SerialPortError) ) );

    connect(this, SIGNAL(connected(void*,QString))
            ,this ,SLOT(statusConnectOn()));
    connect(this, SIGNAL(disconnected(void*))
            ,this ,SLOT(statusConnectOff()));
    connect(this, SIGNAL(disconnected(void*))
            ,this ,SLOT(statusErrorOff()));

    //connect(&m_port, &QSerialPort::readyRead, this, &ComItem::readPort);

    connect(m_pui->pushButton_item_com_control_connect, SIGNAL(clicked())
            ,this ,SLOT(startConnect()));
    connect(m_pui->pushButton_item_com_control_disconnect, SIGNAL(clicked())
            ,this ,SLOT(startDisconnect()));
    connect(m_pui->pushButton_item_com_control_log, SIGNAL(clicked())
            ,this ,SLOT(toggleLogger()));

    connect(m_pui->comboBox_item_com_setup_device,SIGNAL(currentIndexChanged(QString ))
            , this, SLOT(setDevice(QString)));

    connect(m_pui->checkBox_item_com_lock, SIGNAL(stateChanged(int))
            , this, SLOT(lockCOMDevice(int)));

    connect(&m_timService, SIGNAL(timeout())
            , this, SLOT(updateSetupFields()));

    connect(m_pui->comboBox_item_com_setup_baudrate, SIGNAL(currentIndexChanged(QString ))
            , this, SLOT(enableManualBaud(QString)));

    connect(m_pui->pushButton_item_com_setup_choose_config, SIGNAL(clicked())
            , this, SLOT(chooseConfigFile()));
    connect(m_pui->pushButton_item_com_setup_load_file, SIGNAL(clicked())
            , this, SLOT(openConfigFile()));
    connect(m_pui->pushButton_item_com_setup_save_file, SIGNAL(clicked())
            , this, SLOT(saveConfigFile()));

    connect(m_pui->pushButton_item_com_control_log, SIGNAL(clicked())
            , this, SLOT(openLog()) );

}
/*****************************************************************************/
ComItem::~ComItem()
{
    startDisconnect();
    delete (FormProtocolBase*)m_pDevice;
    delete m_pui;
}

/*****************************************************************************/
void ComItem::setDevice(QString a_strDeviceName)
{
    //delete old parser tab
    if(m_pDevice!=nullptr) {
        delete (FormProtocolBase*)m_pDevice;
        m_pDevice = nullptr;
    }

    m_strDeviceName = a_strDeviceName;

    //create device
    //SETUPS_COMITEM_DEVICE();

#define DEVICE_OBJ  QObject*

    if(m_strDeviceName==SETUPS_DEFAULT_NAME) m_pDevice = new SETUPS_DEFAULT_CLASS();
#if defined(DEVICE_ACCELEROMETR_ATO)
    if(m_strDeviceName==SETUPS_ACCELEROMETR_ATO_NAME) m_pDevice = new SETUPS_ACCELEROMETR_ATO_CLASS();
#endif
#if defined(DEVICE_AT_COMMANDS)
    if(m_strDeviceName==SETUPS_NAME_AT_COMMANDS) m_pDevice = new FormProtocolBase();
#endif
#if defined(DEVICE_DPB_COMMANDS)
    if(m_strDeviceName==SETUPS_NAME_DPB_COMMANDS) m_pDevice = new FormProtocolBase();
#endif
#if defined(DEVICE_BOOT)
    if(m_strDeviceName==SETUPS_BOOT_NAME) m_pDevice = new SETUPS_BOOT_CLASS();
#endif
#if defined(DEVICE_DPB)
    if(m_strDeviceName==SETUPS_DPB_NAME) m_pDevice = new SETUPS_DPB_CLASS();
#endif

    ((DeviceInterface*)m_pDevice)->m_pHardware = (void*)(&m_port);

    connect( this, SIGNAL( readData(QByteArray) )
             , (QObject*)m_pDevice, SLOT( receiveDataFromDevice(QByteArray) ) );
    connect( (QObject*)m_pDevice, SIGNAL( send(QByteArray) )
             , this, SLOT( sendData(QByteArray) ) );
    connect( (QObject*)m_pDevice, SIGNAL( setRcMode(device_rcMode) )
             , this, SLOT( setRcMode(device_rcMode) ) );

    connect( (QObject*)m_pDevice, SIGNAL( send(QByteArray) )
             , this, SLOT( add2LogOutput(QByteArray) ) );
    connect( (QObject*)m_pDevice, SIGNAL( receiveDone(QByteArray) )
             , this, SLOT( add2LogInput(QByteArray) ) );
    connect( (QObject*)m_pDevice, SIGNAL( add2Log(QString) )
             , this, SLOT( add2Log(QString) ) );

    connect( this, SIGNAL( connected(void*,QString) )
             , (QObject*)m_pDevice, SLOT( itemConnected(void*,QString) ) );
    connect( this, SIGNAL( disconnected(void*) )
             , (QObject*)m_pDevice, SLOT( itemDisconnect(void*) ) );

    connect( m_pui->pushButton_RobotView, SIGNAL( clicked() )
             , (QObject*)m_pDevice, SLOT( showRobotView() ) );

    connect(this, SIGNAL(disconnected(void*))
            , (QObject*)m_pDevice, SLOT(timStop())    );

    //create new tab
    m_pui->tabWidget->addTab((QWidget*)m_pDevice,m_strDeviceName);

    ((DeviceInterface*)m_pDevice)->runSetups();
}

/*****************************************************************************/
void ComItem::updateCOMLists()
{
    QStringList listCOMDev_curr;

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        if(!info.isBusy()) {
            listCOMDev_curr.append(info.portName());
            //m_pui->comboBox_item_com_choose->addItem((info.portName()));
        }
    }
    QStringList::ConstIterator begin;
    QStringList::ConstIterator end;
    int nSetIndex = 0;
    int nIndex = 0;
    bool bFinded = false;
    QMessageBox msgBoxError;
    begin = listCOMDev_curr.constBegin();
    end = listCOMDev_curr.constEnd();

    if (m_listCOMDev_prev.count() != listCOMDev_curr.count()) goto update;

    for(QStringList::ConstIterator i = begin; i != end; ++i){
        if(!m_listCOMDev_prev.contains((*i))) goto update;
    }
    return;
update:
    m_pui->comboBox_item_com_choose->clear();
    for(QStringList::ConstIterator i = begin; i != end; ++i){
        m_pui->comboBox_item_com_choose->addItem((*i));
        if(m_strCOMDev_prev==(*i)) {
            nSetIndex = nIndex;
            bFinded = true;
        }
        nIndex++;
    }
    m_listCOMDev_prev = listCOMDev_curr;
    m_pui->comboBox_item_com_choose->setCurrentIndex(nSetIndex);
    //if COM name for hold position cant find in current devices
    //clear remember item , lock state and show message
    if(!bFinded&&m_pui->checkBox_item_com_lock->isChecked()) {
        msgBoxError.setText(QString("COM Device (%1) removed!").arg(m_strCOMDev_prev));
        m_pui->checkBox_item_com_lock->setCheckState(Qt::Unchecked);
        msgBoxError.exec();
    }
}

/*****************************************************************************/
void ComItem::fillSetupFields()
{
    updateCOMLists();

    QList<com_settings_field>::ConstIterator end;
    end = m_lBaud.constEnd();
    for(QList<com_settings_field>::ConstIterator i = m_lBaud.constBegin(); i != end; ++i){
        m_pui->comboBox_item_com_setup_baudrate->addItem((*i).name);
    }

    end = m_lDatabits.constEnd();
    for(QList<com_settings_field>::ConstIterator i = m_lDatabits.constBegin(); i != end; ++i){
        m_pui->comboBox_item_com_setup_databits->addItem((*i).name);
    }

    end = m_lParity.constEnd();
    for(QList<com_settings_field>::ConstIterator i = m_lParity.constBegin(); i != end; ++i){
        m_pui->comboBox_item_com_setup_parity->addItem((*i).name);
    }

    end = m_lStopbits.constEnd();
    for(QList<com_settings_field>::ConstIterator i = m_lStopbits.constBegin(); i != end; ++i){
        m_pui->comboBox_item_com_setup_stopbits->addItem((*i).name);
    }

    end = m_lFlowcontrol.constEnd();
    for(QList<com_settings_field>::ConstIterator i = m_lFlowcontrol.constBegin(); i != end; ++i){
        m_pui->comboBox_item_com_setup_flowcontrol->addItem((*i).name);
    }

    end = m_lDevice.constEnd();
    for(QList<com_settings_field>::ConstIterator i = m_lDevice.constBegin(); i != end; ++i){
        m_pui->comboBox_item_com_setup_device->addItem((*i).name);
    }
}
/*****************************************************************************/
void ComItem::startConnect()
{
    QByteArray array;

    //check if already open
    if (m_port.isOpen()) {
        array = m_port.portName().toLocal8Bit();
        printf("App already connect to port %s!\n",(const char*)array.data());
        return;
    }

    //read current setups
    doSetup(readSetups());

    //try connect to port with setups
    doConnect();

    //update gui fields enable for user
    updateSetupFields();
    if (m_port.isOpen()) {
        array = m_port.portName().toLocal8Bit();
        printf("Port %s open succsess!\n",(const char*)array.data());
    }

    //switch setting tab to device tab
    if( (m_pui->tabWidget->count()>=__TAB_MAX)&&(m_pui->tabWidget->currentIndex()==__TAB_SETUPS_IND) )
        m_pui->tabWidget->setCurrentIndex(__TAB_DEVICE_IND);
}
/*****************************************************************************/
void ComItem::doConnect()
{
    //check struct m_settings fields
    ///TODO

    //setups
    m_port.setPortName(m_settings.name);
    m_port.setBaudRate(m_settings.baudRate);
    m_port.setDataBits(m_settings.dataBits);
    m_port.setParity(m_settings.parity);
    m_port.setStopBits(m_settings.stopBits);
    m_port.setFlowControl(m_settings.flowControl);

    //if connect succsess - disable setups
    if (m_port.open(QIODevice::ReadWrite)) {
        m_state = _state_connected;
        m_strComItemName = QString("%1 %2").arg(m_strDeviceName).arg(m_settings.name);
        //отправим сигнал родителю
        emit connected(this,m_strComItemName);
        this->add2Log(tr("Connected to %1 : %2, %3, %4, %5, %6")
                      .arg(m_settings.name)
                      .arg(m_settings.baudRate)
                      .arg(m_settings.dataBits)
                      .arg(m_settings.parity)
                      .arg(m_settings.stopBits)
                      .arg(m_settings.flowControl)
                      );
    } else {
        QMessageBox::critical(this, tr("Error"), m_port.errorString());
        m_state = _state_setup;
    }
}
/*****************************************************************************/
void ComItem::startDisconnect()
{
    //close port
    if (m_port.isOpen()) {
        m_port.flush();
        m_port.close();
        emit disconnected(this);
    }
    //update gui fields enable for user
    m_state = _state_setup;
    updateSetupFields();

    this->add2Log(tr("Disconnected from %1")
                  .arg(m_settings.name)
                  );
}
/*****************************************************************************/
#define __UI_FIELD_TO_STRUCT_INIT \
    QList<com_settings_field>::ConstIterator end;
#define __UI_FIELD_TO_STRUCT(UI_NAME_X,LIST_X,STRUCT_FIELD_X,STRUCT_FIELD_TYPE_X) \
    end = LIST_X.constEnd(); \
    for(QList<com_settings_field>::ConstIterator i = LIST_X.constBegin(); i != end; ++i){ \
        if(UI_NAME_X==(*i).name) {STRUCT_FIELD_X= (STRUCT_FIELD_TYPE_X) (*i).id;break;} \
    }

com_settings ComItem::readSetups()
{
    com_settings _settings;
    int nData = 0;
    bool ok;
    _settings.parity = QSerialPort::UnknownParity;
    _settings.baudRate = QSerialPort::UnknownBaud;
    _settings.dataBits = QSerialPort::UnknownDataBits;
    _settings.stopBits = QSerialPort::UnknownStopBits;
    _settings.flowControl = QSerialPort::UnknownFlowControl;
    //take name from field
    //search name in list
    //complete struct field
    _settings.name = m_pui->comboBox_item_com_choose->currentText();
    __UI_FIELD_TO_STRUCT_INIT;
    //user can put manually baud
    if(m_pui->comboBox_item_com_setup_baudrate->currentIndex()==0){
        _settings.baudRate = m_pui->lineEdit_item_com_setup_baudrate->text().toInt(&ok);
    }else{
        __UI_FIELD_TO_STRUCT(m_pui->comboBox_item_com_setup_baudrate->currentText(),m_lBaud,_settings.baudRate,uint32_t);
    }

    __UI_FIELD_TO_STRUCT(m_pui->comboBox_item_com_setup_databits->currentText(),m_lDatabits,_settings.dataBits,QSerialPort::DataBits);
    __UI_FIELD_TO_STRUCT(m_pui->comboBox_item_com_setup_parity->currentText(),m_lParity,_settings.parity,QSerialPort::Parity);
    __UI_FIELD_TO_STRUCT(m_pui->comboBox_item_com_setup_stopbits->currentText(),m_lStopbits,_settings.stopBits,QSerialPort::StopBits);
    __UI_FIELD_TO_STRUCT(m_pui->comboBox_item_com_setup_flowcontrol->currentText(),m_lFlowcontrol,_settings.flowControl,QSerialPort::FlowControl);

    return _settings;
}
/*****************************************************************************/
void ComItem::doSetup(com_settings a_settings)
{
    m_settings = a_settings;
}
/*****************************************************************************/
void ComItem::sendData(const QByteArray &a_data)
{
    //проверим есть ли вообще подключение. если нет - выведем предупреждение
    if(!isConnected()) {
        QMessageBox::critical(this, tr("Error"), "Connect port before send message!");
        ((FormProtocolBase*)m_pDevice)->timStop();
        return;
    }
    m_port.write(a_data);
    emit sendedData();
}
/*****************************************************************************/
void ComItem::handleError(QSerialPort::SerialPortError error)
{
    this->add2Log(tr("For port:%1 catch QSerialPort error code:%2")
            .arg(m_settings.name)
            .arg(error)
            );
    switch(error){
    case QSerialPort::DeviceNotFoundError      :break;
    case QSerialPort::PermissionError          :break;
    case QSerialPort::OpenError                :break;
    case QSerialPort::ParityError              :break;
    case QSerialPort::FramingError             :break;
    case QSerialPort::BreakConditionError      :break;
    case QSerialPort::WriteError               :break;
    case QSerialPort::ReadError                :break;
    case QSerialPort::ResourceError            :
        /*if(m_port.isOpen()){
            m_port.flush();
            m_port.close();
            doConnect();
        }*/
        break;
    case QSerialPort::UnsupportedOperationError:break;
    case QSerialPort::UnknownError             :break;
    }
    statusErrorOn(error);
}
/*****************************************************************************/
void ComItem::readPort()
{
    const QByteArray data = m_port.readAll();
    emit readData(data);
}
/*****************************************************************************/
QByteArray ComItem::readPortManual()
{
    return m_port.readAll();
}
/*****************************************************************************/
void ComItem::setRcMode(device_rcMode a_mode)
{
    switch(a_mode){
    case _interface_manual_rc:
        this->add2Log(tr("Manual read hardware reg mode!")
                );
        disconnect(&m_port, SIGNAL(readyRead())
                ,this ,SLOT(readPort()));
        break;

    case _interface_signal_rc:
        this->add2Log(tr("Signal read hardware reg mode!")
                );
        connect(&m_port, SIGNAL(readyRead())
                ,this ,SLOT(readPort()));
        break;

    default:
        this->add2Log(tr("Unknown read hardware reg mode!")
                );
        break;

    }
}
/*****************************************************************************/
void ComItem::setComDescription(QString  a_text)
{
    m_pui->plainTextEdit_item_com_info->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        if(info.portName()==a_text) {
            m_pui->plainTextEdit_item_com_info->appendPlainText(
                        QString(""
                                "portName %1\n"
                                "description %2\n"
                                "manufacturer %3\n"
                                "serialNumber %4\n"
                                "systemLocation %5\n"
                                "vendorIdentifier %6\n"
                                "productIdentifier %7\n"
                                )
                        .arg(info.portName())
                        .arg(info.description())
                        .arg(info.manufacturer())
            #if(QT_MAJOR_VERSION>4 && QT_MINOR_VERSION>2)
                        .arg(info.serialNumber())
            #else
                        .arg("---")
            #endif
                        .arg(info.systemLocation())
                        .arg(info.vendorIdentifier())
                        .arg(info.productIdentifier())
                        );
        }
    }

    m_pui->groupBox_item_com_control->setTitle(trUtf8("Control %1").arg(a_text));

}

/*****************************************************************************/
void ComItem::lockCOMDevice(int state)
{
    //remember COM name for hold position when update device list
    if(state==Qt::Checked){
        m_strCOMDev_prev = m_pui->comboBox_item_com_choose->currentText();
        m_pui->comboBox_item_com_choose->setEnabled(false);
    }else{
        m_strCOMDev_prev ="";
        m_pui->comboBox_item_com_choose->setEnabled(true);
    }
}
/*****************************************************************************/
void ComItem::updateSetupFields()
{
    switch(m_state)  {
    case _state_setup:
        m_pui->pushButton_item_com_control_connect->setEnabled(true);
        m_pui->pushButton_item_com_control_disconnect->setEnabled(false);
        m_pui->groupBox_item_com->setEnabled(true);
        m_pui->groupBox_item_com_setup->setEnabled(true);
        updateCOMLists();
        break;
    case _state_connected:
        m_pui->pushButton_item_com_control_connect->setEnabled(false);
        m_pui->pushButton_item_com_control_disconnect->setEnabled(true);
        m_pui->groupBox_item_com->setEnabled(false);
        m_pui->groupBox_item_com_setup->setEnabled(false);
        break;
    default:
        break;
    }
    statusTxOff();
    statusRxOff();
}

/*****************************************************************************/
void ComItem::toggleLogger()
{
    m_bIsLogger = ~m_bIsLogger;
    m_pui->pushButton_item_com_control_log->toggled(m_bIsLogger);
}

/*****************************************************************************/
void ComItem::enableManualBaud(QString strBaud)
{
    //TODO switch to equals
    if(strBaud==__FIELD_MANUAL_BAUD){
        m_pui->lineEdit_item_com_setup_baudrate->setEnabled(true);
    }else{
        m_pui->lineEdit_item_com_setup_baudrate->clear();
        m_pui->lineEdit_item_com_setup_baudrate->setEnabled(false);
    }
}

/*****************************************************************************/
void ComItem::chooseConfigFile()
{
    QString strFile = QFileDialog::getOpenFileName(
                        this,
                        tr("Выбрать файл конфигурации"),
                        QDir::current().absolutePath(),
                        "Файлы конигурации (*.conf)"
                        );
    m_pui->lineEdit_item_com_setup_current_config->setText(strFile);
}

/*****************************************************************************/
void ComItem::saveConfigFile()
{
    QString strFile = QFileDialog::getSaveFileName(
                        this,
                        tr("Сохранить файл конигурации"),
                        QDir::current().absolutePath(),
                        "Файлы конигурации (*.conf)"
                        );

    QFile file(strFile);
    QMessageBox msgBoxError;
    if(!file.open(QIODevice::WriteOnly)) {
        msgBoxError.setText(QString("Cant open file (%1)!").arg(strFile));
        msgBoxError.exec();
    }
    QString strData;
    if(m_pui->comboBox_item_com_setup_baudrate->currentText()==__FIELD_MANUAL_BAUD){
        strData.append(QString("%1%2%3").arg(__CONF_BAUD).arg(__CONF_VALUE).arg(m_pui->lineEdit_item_com_setup_baudrate->text()));
    }else {
        strData.append(QString("%1%2%3").arg(__CONF_BAUD).arg(__CONF_VALUE).arg(m_pui->comboBox_item_com_setup_baudrate->currentText()));
    }
    strData.append(__CONF_FIELD);
    strData.append(QString("%1%2%3").arg(__CONF_DATABITS).arg(__CONF_VALUE).arg(m_pui->comboBox_item_com_setup_databits->currentText()));
    strData.append(__CONF_FIELD);
    strData.append(QString("%1%2%3").arg(__CONF_PARITY).arg(__CONF_VALUE).arg(m_pui->comboBox_item_com_setup_parity->currentText()));
    strData.append(__CONF_FIELD);
    strData.append(QString("%1%2%3").arg(__CONF_STOPBITS).arg(__CONF_VALUE).arg(m_pui->comboBox_item_com_setup_stopbits->currentText()));
    strData.append(__CONF_FIELD);
    strData.append(QString("%1%2%3").arg(__CONF_FLOWCONTROL).arg(__CONF_VALUE).arg(m_pui->comboBox_item_com_setup_flowcontrol->currentText()));
    strData.append(__CONF_FIELD);
    strData.append(QString("%1%2%3").arg(__CONF_DEVICE).arg(__CONF_VALUE).arg(m_pui->comboBox_item_com_setup_device->currentText()));
    strData.append(__CONF_FIELD);
    strData.append(QString("%1%2%3").arg(__CONF_COM).arg(__CONF_VALUE).arg(m_pui->comboBox_item_com_choose->currentText()));

    file.write(strData.toUtf8());
}
/*****************************************************************************/
void ComItem::openConfigFile()
{
    QString strFile = m_pui->lineEdit_item_com_setup_current_config->text();
    loadConfigFile(strFile);
}
bool ComItem::loadConfigFile(QString filepath)
{
    QStringList lstField;
    QStringList lstVal;

    QFile file(filepath);
    QMessageBox msgBoxError;
    if(!file.open(QIODevice::ReadOnly)) {
        msgBoxError.setText(QString("Cant open file (%1)!").arg(filepath));
        msgBoxError.exec();
        qDebug()<<QString("Cant open file (%1)!").arg(filepath);
    }
    //read string
    QString strData;
    strData = QTextCodec::codecForMib(106)->toUnicode(file.readAll());

    //parse by field separate
    lstField = strData.split(__CONF_FIELD);

    //parse by value separate
    QStringList::ConstIterator end;
    QStringList::ConstIterator begin;
    end = lstField.constEnd();
    begin = lstField.constBegin();
    for(QStringList::ConstIterator i = begin; i != end; ++i){
        lstVal = (*i).split(__CONF_VALUE);
        if(lstVal[0]==__CONF_BAUD) {
            m_pui->comboBox_item_com_setup_baudrate->setCurrentIndex(0);
            m_pui->lineEdit_item_com_setup_baudrate->setText(lstVal[1]);
            goto end_value;
        }
        if(lstVal[0]==__CONF_DATABITS) {
            if(m_pui->comboBox_item_com_setup_databits->findText(lstVal[1])==-1){
                msgBoxError.setText(QString("Cant find databits setup (%1)!").arg(lstVal[1]));
                msgBoxError.exec();
                qDebug()<<QString("Cant find databits setup (%1)!").arg(lstVal[1]);
                goto fail;
            }else{
                m_pui->comboBox_item_com_setup_databits->setCurrentText(lstVal[1]);
            }
            goto end_value;
        }
        if(lstVal[0]==__CONF_PARITY) {
            if(m_pui->comboBox_item_com_setup_parity->findText(lstVal[1])==-1){
                msgBoxError.setText(QString("Cant find parity setup (%1)!").arg(lstVal[1]));
                msgBoxError.exec();
                qDebug()<<QString("Cant find parity setup (%1)!").arg(lstVal[1]);
                goto fail;
            }else{
                m_pui->comboBox_item_com_setup_parity->setCurrentText(lstVal[1]);
            }
            goto end_value;
        }
        if(lstVal[0]==__CONF_STOPBITS) {
            if(m_pui->comboBox_item_com_setup_stopbits->findText(lstVal[1])==-1){
                msgBoxError.setText(QString("Cant find stopbits setup (%1)!").arg(lstVal[1]));
                msgBoxError.exec();
                qDebug()<<QString("Cant find stopbits setup (%1)!").arg(lstVal[1]);
                goto fail;
            }else{
                m_pui->comboBox_item_com_setup_stopbits->setCurrentText(lstVal[1]);
            }
            goto end_value;
        }
        if(lstVal[0]==__CONF_FLOWCONTROL) {
            if(m_pui->comboBox_item_com_setup_flowcontrol->findText(lstVal[1])==-1){
                msgBoxError.setText(QString("Cant find flowcontrol setup (%1)!").arg(lstVal[1]));
                msgBoxError.exec();
                qDebug()<<QString("Cant find flowcontrol setup (%1)!").arg(lstVal[1]);
                goto fail;
            }else{
                m_pui->comboBox_item_com_setup_flowcontrol->setCurrentText(lstVal[1]);
            }
            goto end_value;
        }
        if(lstVal[0]==__CONF_DEVICE) {
            //check if COM not exist
            if(m_pui->comboBox_item_com_setup_device->findText(lstVal[1])==-1){
                msgBoxError.setText(QString("Cant find device (%1)! Check libraries DLLs!").arg(lstVal[1]));
                msgBoxError.exec();
                qDebug()<<QString("Cant find device (%1)! Check libraries DLLs!").arg(lstVal[1]);
                goto fail;
            }else{
                m_pui->comboBox_item_com_setup_device->setCurrentText(lstVal[1]);
            }
            goto end_value;
        }
        if(lstVal[0]==__CONF_COM) {
            //check if COM not exist
            if(m_pui->comboBox_item_com_choose->findText(lstVal[1])==-1){
                msgBoxError.setText(QString("Cant find COM port (%1)!").arg(lstVal[1]));
                msgBoxError.exec();
                qDebug()<<QString("Cant find COM port (%1)!").arg(lstVal[1]);
                goto fail;
            }else{
                m_pui->comboBox_item_com_choose->setCurrentText(lstVal[1]);
            }
            goto end_value;
        }
end_value:;
    }

    return true;
fail:;
    return false;
}
/*****************************************************************************/
void ComItem::openLog()
{
    //m_log.hide(); //stay on top
    m_log.show();
}
/*****************************************************************************/
void ComItem::add2Log(QString data)
{
    QDateTime time;
    if(m_log.blockCount()>1000) m_log.clear();
    m_log.appendHtml(
                QString("%3 <font color=\"blue\">%1</font> %2")
                .arg(time.currentDateTime().toString())
                .arg(data)
                .arg(m_log.blockCount())
                );
    qDebug()<<data;
}
/*****************************************************************************/
void ComItem::add2LogInput(QByteArray data)
{
    QString strData = data.toHex();
    //add2Log(QString("<font color=\"green\">Input</font>:0x%1").arg(strData));
}
/*****************************************************************************/
void ComItem::add2LogOutput(QByteArray data)
{
    QString strData = data.toHex();
    //add2Log(QString("<font color=\"green\">Output</font>:0x%1").arg(strData));
}
