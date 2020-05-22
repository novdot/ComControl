#include "comdevicebase.h"
#include "ui_formcomdevicebase.h"

#include <QTextCodec>

COMDeviceBase::COMDeviceBase(QWidget *parent)
    : COMDeviceInterface(parent)
    , m_pui(new Ui::COMDeviceBase)
{
    m_pui->setupUi(this);
    QList< QPair<int,QString> >::ConstIterator end;
    end = m_lFormat.constEnd();
    for(QList< QPair<int,QString> >::ConstIterator i = m_lFormat.constBegin(); i != end; ++i){
        m_pui->comboBox_device_rec_format->addItem((*i).second);
        m_pui->comboBox_device_send_format->addItem((*i).second);
    }

    connect(m_pui->pushButton_device_rec_clear,SIGNAL(clicked())
            , this, SLOT(clearRecText()));

    connect(m_pui->pushButton_device_send,SIGNAL(clicked())
            , this, SLOT(sendText()));

    m_pui->lineEdit_device_send_count->setText("1");

    tim = new QTimer();
    connect(tim,SIGNAL(timeout()),SLOT(timUpdateEvent()));
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
    static bool m_bIgnoreNewLine = false;

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
    //запишем строку
    m_pui->plainTextEdit_device_rec->appendHtml(strData);

    emit receive(a_data);

    /*
    m_pui->plainTextEdit_device_rec->textCursor().insertHtml(strData);
    //определим последний символ с целью перевода строки
    if(  strData.at(strData.size()-1)=='\n' || strData.at(strData.size()-1)=='\r') {
        bIsNewLine = true;
    }
    //если пришел всего один символ перевода строки и предыдущий символ также переводил строку - проигнорируем текущую итерацию
    if(m_bIgnoreNewLine&&(strData.size()==1)&&bIsNewLine)
        return;
    //если есть последний символ на перевод строки и
    //предыдущая итерация не переводила строку - перейдем на новую строчку
    if( (!m_bIgnoreNewLine) && bIsNewLine){
        m_pui->plainTextEdit_device_rec->appendHtml("");
        m_bIgnoreNewLine = true;
    } else {
        m_pui->plainTextEdit_device_rec->moveCursor(QTextCursor::End);
        m_bIgnoreNewLine = false;
    }*/
}

/*****************************************************************************/
void COMDeviceBase::clearRecText()
{
    m_pui->plainTextEdit_device_rec->clear();
}

/*****************************************************************************/
void COMDeviceBase::sendText()
{
    QByteArray byteArray;
    QString strText = "";
    int nFormatInd = -1;

    //если процесс отправки сообщений запущен - повторное нажатие остановит цикл
    if(m_nSendCnt!=0 && tim->isActive()) {
        timStop();
        return;
    }

    //check combobox state
    nFormatInd = m_pui->comboBox_device_send_format->currentIndex();

    //read text from text field and convert to selected format
    strText = m_pui->plainTextEdit_device_send->toPlainText();

    //получим строку в зависимости от выбранного формата
    switch (nFormatInd) {
    case _device_format_utf8:
        //strText.fromUtf8( &byteArray );
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
    m_byteArray = byteArray;

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
        emit sendDataToDevice(m_byteArray);
    }else{
        if(m_nSendCnt==0) m_nSendCnt = -1;
        m_pui->pushButton_device_send->setText("stop");
        m_pui->lineEdit_device_send_count->setEnabled(false);
        m_pui->lineEdit_device_send_period->setEnabled(false);

        tim->start(period);
    }
}
/*****************************************************************************/
void COMDeviceBase::timStop()
{
    tim->stop();
    m_nSendCnt = 0;
    m_pui->pushButton_device_send->setText("send");
    m_pui->lineEdit_device_send_count->setEnabled(true);
    m_pui->lineEdit_device_send_period->setEnabled(true);
    m_pui->lineEdit_device_send_count->setText(QString("%1").arg(m_nSendCnt));
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

    emit sendDataToDevice(m_byteArray);
}
