#ifndef COMDEVICEINTERFACE_H
#define COMDEVICEINTERFACE_H

#include <QObject>
#include <QWidget>
#include <QPair>
#include <QList>
#include <QByteArray>

/*!
    \defgroup COMDeviceInterface интерфейс устройства
    \ingroup COMDevice
    \brief
*/
///@{

typedef enum device_formatDef{
    _device_format_hex
    , _device_format_utf8
    , _device_format_ascii
}device_format;

/*!
    \brief
    \author dnovikov
    \date 2020 01 17
    \warning на стадии разработки
 * */
class COMDeviceInterface : public QWidget
{
    Q_OBJECT
public:
    COMDeviceInterface(QWidget *parent = nullptr): QWidget(parent)
    {
        QPair<int,QString> format;
        format.first = _device_format_hex;
        format.second = trUtf8("Hex");
        m_lFormat.append(format);
        format.first = _device_format_utf8;
        format.second = trUtf8("Utf8");
        m_lFormat.append(format);
        format.first = _device_format_ascii;
        format.second = trUtf8("ASCII");
        m_lFormat.append(format);
    }
    //QList< QPair<int,QString> > getEventsList(){return m_lEvents;}
    //QList< QPair<int,QString> > getCommandsList(){return m_lCommands;}
public slots:
    /*!
     * \brief слот для обработки входящего пакета
    */
    virtual void receiveDataFromDevice(QByteArray a_data){Q_UNUSED(a_data)}
signals:
    /*!
     * \brief сигнал пакета управления устройством
    */
    void sendDataToDevice(QByteArray);
protected:
    //списки возможных значений
    //QList< QPair<int,QString> > m_lEvents; //< События
    //QList< QPair<int,QString> > m_lCommands; //< команды (УВ)
    QList< QPair<int,QString> > m_lFormat; //< формат входных/выходных данных
};

///@}
#endif // COMDEVICEINTERFACE_H
