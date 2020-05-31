#ifndef COMDEVICEINTERFACE_H
#define COMDEVICEINTERFACE_H

#include <QObject>
#include <QWidget>
#include <QPair>
#include <QList>
#include <QByteArray>
#include <QDebug>

/*!
    \defgroup COMDeviceInterface интерфейс устройства
    \ingroup COMDevice
    \brief
*/
///@{

typedef enum device_formatDef{
    _device_format_hex = 0
    , _device_format_utf8 = 1
    , _device_format_ascii = 2
}device_format;

///структура
#define COM_ROBOT_VALUE_DEFAULT "as is"
typedef struct com_robot_srcDef{
    void* device;
    device_format format;
    QString value;
}com_robot_src;

///структура условия событие -> действие
typedef struct com_robotDef {
    QString name;
    com_robot_src slave;
    com_robot_src master;
}com_robot;

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
    COMDeviceInterface(QWidget *parent = nullptr): QWidget(parent) {
        m_lFormat = COMDeviceInterface::getFormatList();
    }
    QList<com_robot> getRobotList(){return m_lRobot;}
    void setRobotList(QList<com_robot> list){
        m_lRobot = list;

        //проверка на элементы , где есть только слейв. занесем их в отдельный список
        QList<com_robot>::ConstIterator begin;
        QList<com_robot>::ConstIterator end;
        begin = m_lRobot.constBegin();
        end = m_lRobot.constEnd();

        m_lRobotSlave.clear();

        for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
            if( ((*i).master.device != nullptr) ) continue;
            m_lRobotSlave.append((*i));
        }

        updateSlaveControl();
    }

    static QList< QPair<int,QString> > getFormatList(){
        QList< QPair<int,QString> > lFormat;
        QPair<int,QString> format;
        format.first = _device_format_hex;
        format.second = trUtf8("Hex");
        lFormat.append(format);
        format.first = _device_format_utf8;
        format.second = trUtf8("Utf8");
        lFormat.append(format);
        format.first = _device_format_ascii;
        format.second = trUtf8("ASCII");
        lFormat.append(format);

        return lFormat;
    }

public slots:
    /*!
     * \brief слот для обработки входящего пакета
    */
    virtual void receiveDataFromDevice(QByteArray a_data){Q_UNUSED(a_data)}
    virtual void sendDataToDevice(QByteArray a_data){Q_UNUSED(a_data)}
    /*!
     * \brief обновление списка возможных команд
     */
    virtual void updateSlaveControl(){}
signals:
    /*!
     * \brief сигнал пакета управления устройством
    */
    void send(QByteArray);

    /*!
     * \brief сигнал обновления списка слейва, для обновления возможных команд от COMdevice
    */
    void slaveListUpdated();
private:
protected:
    //список событий
    QList<com_robot> m_lRobot;
    //подсписоки событий. Содержатся только события для слейва - быстрые команды на выдачу
    QList<com_robot> m_lRobotSlave;
    //списки возможных значений
    QList< QPair<int,QString> > m_lFormat; //< формат входных/выходных данных
};

///@}
#endif // COMDEVICEINTERFACE_H
