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

}

COMDeviceBase::~COMDeviceBase()
{

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

    //send data
    emit sendDataToDevice(byteArray);
}
