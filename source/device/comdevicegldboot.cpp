#include "comdevicegldboot.h"

#include <QDebug>
#include <QTextCodec>

COMDeviceGldBoot::COMDeviceGldBoot(QWidget*parent)
    : COMDeviceBase(parent)
{
    connect(this,SIGNAL(receive(QByteArray)),this,SLOT(receiveHandler(QByteArray)));
}

void COMDeviceGldBoot::receiveHandler(QByteArray data)
{
    QByteArray byteArray;
    qDebug()<<Q_FUNC_INFO<<"data:"<<data.toHex();
    //cc 1f d2 05 00 00 04 00 fa
    if(data.toHex()=="cc1fd20500000400fa"){
        byteArray = QByteArray::fromHex(QString("dd00d20500d7").toUtf8());
        emit sendDataToDevice(byteArray);
    }
    if(data.toHex()=="cc1fd30500f7"){
        byteArray = QByteArray::fromHex(QString("dd00d300000400d7").toUtf8());
        emit sendDataToDevice(byteArray);
    }
    if(data.toHex()=="cc1fd201390000012b"){
        byteArray = QByteArray::fromHex(QString("dd00d20100d3").toUtf8());
        emit sendDataToDevice(byteArray);
    }
    if(data.toHex()=="cc1fd30100f3"){
        byteArray = QByteArray::fromHex(QString("dd00d3390000010c").toUtf8());
        emit sendDataToDevice(byteArray);
    }
    if(data.toHex()=="cc1fd70500fb"){
        byteArray = QByteArray::fromHex(QString("dd00d70500dc").toUtf8());
        emit sendDataToDevice(byteArray);
    }

}
