#include "comdevicebase.h"
#include "ui_formcomdevicebase.h"

#include <QTextCodec>
#include <QMessageBox>
#include <QDebug>

COMDeviceBase::COMDeviceBase(QWidget *parent)
    : COMDeviceInterface(parent)
    , m_pui(new Ui::COMDeviceBase)
{
    m_pui->setupUi(this);

    m_pErrMsg = new QMessageBox();

    //gui format lists
    QList< QPair<int,QString> >::ConstIterator end;
    end = m_lFormat.constEnd();
    for(QList< QPair<int,QString> >::ConstIterator i = m_lFormat.constBegin(); i != end; ++i){
        m_pui->comboBox_device_rec_format->addItem((*i).second);
        m_pui->comboBox_device_send_format->addItem((*i).second);
    }

    //recieve
    connect(m_pui->pushButton_device_rec_clear,SIGNAL(clicked())
            , this, SLOT(clearRecText()));

    //send
    connect(m_pui->pushButton_device_send,SIGNAL(clicked())
            , this, SLOT(sendText()));
    connect(m_pui->pushButton_device_send_all,SIGNAL(clicked())
            , this, SLOT(sendListAll()));

    connect(m_pui->pushButton_device_send_update,SIGNAL(clicked())
            , this, SLOT(updateCurrentCmd2List()));
    connect(m_pui->pushButton_device_send_add2list,SIGNAL(clicked())
            , this, SLOT(addCurrentCmd2List()));
    connect(m_pui->pushButton_device_send_open_robot_list,SIGNAL(clicked())
            , this, SLOT(openRobotList()));

    m_pui->lineEdit_device_send_count->setText("1");

    m_pui->lineEdit_device_send_position->setText("1");

    tim = new QTimer();
    connect(tim,SIGNAL(timeout()),SLOT(timUpdateEvent()));

    //robot
    connect(this,SIGNAL(slaveListUpdated()),this,SLOT(updateSlaveControl()));

    connect(&m_lSlaveView ,SIGNAL(itemClicked(QListWidgetItem *))
                , this, SLOT(doSlaveControl(QListWidgetItem *)));

}

COMDeviceBase::~COMDeviceBase()
{
    delete m_pui;
}

/*****************************************************************************/
void COMDeviceBase::receiveDataFromDevice(QByteArray a_data)
{
    int nFormatInd = m_pui->comboBox_device_rec_format->currentIndex();
    bool bIsNewLine = false;
    QString strData = "";
    QString strCurrData = "";
    static bool m_bIgnoreNewLine = false;

    //получим строку в зависимости от выбранного формата
    strData = convertByteArray2Str(nFormatInd,a_data);

    //запишем строку
    strCurrData = m_pui->textBrowser_device_rec->toPlainText();
    m_pui->textBrowser_device_rec->setPlainText(QString("%1%2").arg(strCurrData).arg(strData));

    emit receiveDone(a_data);
    this->robot(a_data);
}
/*****************************************************************************/
QByteArray COMDeviceBase::convertStr2ByteArray(int nFormatInd, QString strText)
{
    QByteArray byteArray;
    switch (nFormatInd) {
    case _device_format_utf8:
        byteArray = strText.toLocal8Bit();
        break;
    case _device_format_ascii:
        break;
    case _device_format_hex:
    default:
        strText.remove(QChar(' '));
        byteArray = QByteArray::fromHex(strText.toUtf8());
        break;
    }
    return byteArray;
}
/*****************************************************************************/
QString COMDeviceBase::convertByteArray2Str(int nFormatInd, QByteArray a_data)
{
    QString strData;
    //получим строку в зависимости от выбранного формата
    switch (nFormatInd) {
    case _device_format_utf8:
        strData = QTextCodec::codecForName("UTF-8")->toUnicode(a_data);
        break;
    case _device_format_ascii:
        strData = QTextCodec::codecForName("KOI8-R")->toUnicode(a_data);
        break;
    case _device_format_hex:
    default:
        strData = a_data.toHex();
        break;
    }
    return strData;
}
/*****************************************************************************/
void COMDeviceBase::robot(QByteArray a_msg)
{
    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobot.constBegin();
    end = m_lRobot.constEnd();

    QString rec = "";

    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        //проверим мастер - устройство
        //если - текущее, то поробуем выполнить
        if( ((*i).master.device == this) ){
            //если значение - по умолчанию, то просто транслируем
            if((*i).master.value == COM_ROBOT_VALUE_DEFAULT) {
                ((COMDeviceBase*)(*i).slave.device)->sendDataToDevice(a_msg);
                continue;
            }
            //перегоним полученное сообщение в формат мастера
            rec = convertStr2ByteArray((*i).master.format,(*i).master.value);
            //сравним значение мастера с принятым сообщением
            if((*i).master.value == rec) {
                //если успех - запишем в слейв перекодированное сообщение слейва
                ((COMDeviceBase*)(*i).slave.device)->sendDataToDevice(
                            convertStr2ByteArray((*i).master.format,(*i).master.value)
                            );
                continue;
            }
        }
    }
}

/*****************************************************************************/
void COMDeviceBase::clearRecText()
{
    m_pui->textBrowser_device_rec->clear();
}

/*****************************************************************************/
void COMDeviceBase::sendText()
{
    QByteArray byteArray;
    QString strText = "";
    QString strKeyEmul = "";
    int nFormatInd = -1;

    //если процесс отправки сообщений запущен - повторное нажатие остановит цикл
    if(m_nSendCnt!=0 && tim->isActive()) {
        timStop();
        return;
    }

    //check combobox state
    nFormatInd = m_pui->comboBox_device_send_format->currentIndex();

    //read text from text field and convert to selected format
    strText = m_pui->lineEdit_device_send->text();

    //получим строку в зависимости от выбранного формата
    m_byteArray = convertStr2ByteArray(nFormatInd,strText);

    //проверяем поля ввода для отправки сообщений
    //если количество отправок > 1, то запускаем таймер с установленным периодом.
    m_nSendCnt = m_pui->lineEdit_device_send_count->text().toInt();
    int period;
    if(m_pui->lineEdit_device_send_period->text()=="")
        period = 1;
    else
        period = m_pui->lineEdit_device_send_period->text().toInt();

    if(m_nSendCnt == 1) {
        m_nSendCnt = 0;
        sendDataToDevice(m_byteArray);
    }else{
        if(m_nSendCnt==0) m_nSendCnt = -1;
        m_pui->pushButton_device_send->setText("stop");
        m_pui->lineEdit_device_send_count->setEnabled(false);
        m_pui->lineEdit_device_send_period->setEnabled(false);

        tim->start(period);
    }
}

/*****************************************************************************/
void COMDeviceBase::sendDataToDevice(QByteArray a_data)
{
    emit send(a_data);
}
/*****************************************************************************/
void COMDeviceBase::timStop()
{
    tim->stop();
    m_pui->pushButton_device_send->setText("send");
    m_pui->lineEdit_device_send_count->setEnabled(true);
    m_pui->lineEdit_device_send_period->setEnabled(true);
    //m_nSendCnt = 0;
    //m_pui->lineEdit_device_send_count->setText(QString("%1").arg(m_nSendCnt));
    return;
}
/*****************************************************************************/
void COMDeviceBase::timUpdateEvent()
{
    //если счетчик отправки сообщений установлен в 0 - остановим таймер
    if(m_nSendCnt==0) {
        timStop();
        return;
    }
    //если цикл отправки не бесконечный, то уменьшим счетчик.
    if(m_nSendCnt!=-1) {
        m_nSendCnt--;
        m_pui->lineEdit_device_send_count->setText(QString("%1").arg(m_nSendCnt));
    }

    sendDataToDevice(m_byteArray);
}
/*****************************************************************************/
void COMDeviceBase::showRobotView()
{
    QString line;
    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobot.constBegin();
    end = m_lRobot.constEnd();

    m_lRobotView.clear();

    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        line.clear();
        line = QString("%1 from:%2 if EQ val:%3 to:%4 send val:%5 ")
                .arg((*i).name)
                .arg(((COMDeviceBase*)(*i).master.device)->getCOMName())
                .arg((*i).master.value)
                .arg(((COMDeviceBase*)(*i).slave.device)->getCOMName())
                .arg((*i).slave.value)
                ;

        m_lRobotView.addItem(line);
    }
    m_lRobotView.show();
}
/*****************************************************************************/
void COMDeviceBase::updateSlaveControl()
{
    qDebug()<<Q_FUNC_INFO;
    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobotSlave.constBegin();
    end = m_lRobotSlave.constEnd();

    QString rec = "";

    m_lSlaveView.clear();

    //заполним slave list
    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        if( ((*i).slave.device != this) ) continue;
        m_lSlaveView.addItem((*i).name);
    }
}
/*****************************************************************************/
void COMDeviceBase::doSlaveControl(QListWidgetItem * item)
{
    QString choosenText = item->text();
    //снимем подсветку со списка
    /*for(int i=0;i<m_pui->tableWidget_device_send_list->rowcount();i++) {
        (m_lSlaveView.item(i))->setBackgroundColor(Qt::white);
    }
    //подсветим выбранную комманду
    //item->setBackgroundColor(Qt::yellow);
    */
    m_lSlaveView.hide();

    //очистим поле ввода комманды
    m_pui->lineEdit_device_send->clear();

    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobotSlave.constBegin();
    end = m_lRobotSlave.constEnd();
    //ищем выбранный элемнт в списке
    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        if((*i).name!=choosenText) continue;
        if( ((*i).slave.device != this) ) continue;
        if( ((*i).slave.value == COM_ROBOT_VALUE_DEFAULT) ) continue;
        //все нашли , элемент подходит данному ComDevice
        //установим нашу команду
        m_pui->lineEdit_device_send->setText((*i).slave.value);
        m_pui->comboBox_device_send_format->setCurrentIndex((*i).slave.format);
        return;
    }
}
/*****************************************************************************/
void COMDeviceBase::sendListAll()
{
    if(m_pui->tableWidget_device_send_list->rowCount() <= 0){
        m_pErrMsg->setText(tr("Table commands was emptry!"));
        m_pErrMsg->show();
        return;
    }
}
/*****************************************************************************/
void COMDeviceBase::updateCurrentCmd2List()
{
    //search by position
}
/*****************************************************************************/
void COMDeviceBase::addCurrentCmd2List()
{
    //new position

    //read fields from table
}
/*****************************************************************************/
void COMDeviceBase::openRobotList()
{

}
