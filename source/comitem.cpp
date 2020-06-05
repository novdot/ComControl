#include "comitem.h"
#include "ui_formcomitem.h"

#include "device/comdevicebase.h"

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

    m_timService.setInterval(1000);
    m_timService.start();

    this->setDevice("Device");
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

    connect(&m_port, SIGNAL(readyRead())
            ,this ,SLOT(readPort()));
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
            , this, SLOT(loadConfigFile()));
    connect(m_pui->pushButton_item_com_setup_save_file, SIGNAL(clicked())
            , this, SLOT(saveConfigFile()));

    connect(m_pui->pushButton_item_com_control_log, SIGNAL(clicked())
            , this, SLOT(openLog()) );

}
/*****************************************************************************/
ComItem::~ComItem()
{
    startDisconnect();
    delete (COMDeviceBase*)m_pDevice;
    delete m_pui;
}

/*****************************************************************************/
void ComItem::setDevice(QString a_strDeviceName)
{
    //delete old parser tab
    if(m_pDevice!=nullptr) {
        delete (COMDeviceBase*)m_pDevice;
        m_pDevice = nullptr;
    }

    //create device
    m_pDevice = new COMDeviceBase();

    connect( this, SIGNAL( readData(QByteArray) )
             , (QObject*)m_pDevice, SLOT( receiveDataFromDevice(QByteArray) ) );
    connect( (QObject*)m_pDevice, SIGNAL( send(QByteArray) )
             , this, SLOT( sendData(QByteArray) ) );

    connect( (QObject*)m_pDevice, SIGNAL( send(QByteArray) )
             , this, SLOT( add2LogOutput(QByteArray) ) );
    connect( (QObject*)m_pDevice, SIGNAL( receive(QByteArray) )
             , this, SLOT( add2LogInput(QByteArray) ) );

    connect( &m_port, SIGNAL( error(QSerialPort::SerialPortError) )
             , this, SLOT( handleError(QSerialPort::SerialPortError) ) );


    connect( this, SIGNAL( connected(void*,QString) )
             , (QObject*)m_pDevice, SLOT( itemConnected(void*,QString) ) );
    connect( this, SIGNAL( disconnected(void*) )
             , (QObject*)m_pDevice, SLOT( itemDisconnect(void*) ) );


    connect( m_pui->pushButton_RobotView, SIGNAL( clicked() )
             , (QObject*)m_pDevice, SLOT( showRobotView() ) );

    //create new tab
    m_pui->tabWidget->addTab((QWidget*)m_pDevice,a_strDeviceName);
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
    //check if already open
    if (m_port.isOpen()) return;

    //read current setups
    doSetup(readSetups());

    //try connect to port with setups
    doConnect();

    //update gui fields enable for user
    updateSetupFields();
    if (m_port.isOpen()) {

    }
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
        m_strComItemName = m_settings.name;
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
    m_port.write(a_data);
}
/*****************************************************************************/
void ComItem::handleError(QSerialPort::SerialPortError error)
{
    this->add2Log(tr("For port:%1 catch QSerialPort error code:%2")
            .arg(m_settings.name)
            .arg(error)
            );
}
/*****************************************************************************/
void ComItem::readPort()
{
    const QByteArray data = m_port.readAll();
    emit readData(data);
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
                        .arg(info.serialNumber())
                        .arg(info.systemLocation())
                        .arg(info.vendorIdentifier())
                        .arg(info.productIdentifier())
                        );
        }
    }

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

    file.write(strData.toUtf8());
}
/*****************************************************************************/
void ComItem::loadConfigFile()
{
    QStringList lstField;
    QStringList lstVal;
    QString strFile = m_pui->lineEdit_item_com_setup_current_config->text();
    QFile file(strFile);
    QMessageBox msgBoxError;
    if(!file.open(QIODevice::ReadOnly)) {
        msgBoxError.setText(QString("Cant open file (%1)!").arg(strFile));
        msgBoxError.exec();
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
            m_pui->comboBox_item_com_setup_databits->setCurrentText(lstVal[1]);
            goto end_value;
        }
        if(lstVal[0]==__CONF_PARITY) {
            m_pui->comboBox_item_com_setup_parity->setCurrentText(lstVal[1]);
            goto end_value;
        }
        if(lstVal[0]==__CONF_STOPBITS) {
            m_pui->comboBox_item_com_setup_stopbits->setCurrentText(lstVal[1]);
            goto end_value;
        }
        if(lstVal[0]==__CONF_FLOWCONTROL) {
            m_pui->comboBox_item_com_setup_flowcontrol->setCurrentText(lstVal[1]);
            goto end_value;
        }
        if(lstVal[0]==__CONF_DEVICE) {
            m_pui->comboBox_item_com_setup_device->setCurrentText(lstVal[1]);
            goto end_value;
        }
end_value:;
    }

}
/*****************************************************************************/
void ComItem::openLog()
{
    m_log.show();
}
/*****************************************************************************/
void ComItem::add2Log(QString data)
{
    m_log.appendHtml(data);
}
/*****************************************************************************/
void ComItem::add2LogInput(QByteArray data)
{
    QString strData = data.toHex();
    add2Log(QString("Input:0x%1").arg(strData));
}
/*****************************************************************************/
void ComItem::add2LogOutput(QByteArray data)
{
    QString strData = data.toHex();
    add2Log(QString("Output:0x%1").arg(strData));
}
