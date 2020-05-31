#ifndef COMDEVICEBASE_H
#define COMDEVICEBASE_H

#include "comdeviceinterface.h"

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class COMDeviceBase; }
QT_END_NAMESPACE

/*!
    \defgroup COMDevice устройства
    \ingroup Dispatcher
    \brief содержит описание протоколов обмена.
*/
///@{

/*!
    \brief класс устройства
    \author dnovikov
    \date 2020 01 17
    \warning на стадии разработки
 * */
class COMDeviceBase : public COMDeviceInterface
{
    Q_OBJECT
public:
    COMDeviceBase(QWidget *parent = nullptr);
    ~COMDeviceBase();

    static QByteArray convertStr2ByteArray(int,QString);
    static QString convertByteArray2Str(int,QByteArray);

public slots:
    virtual void receiveDataFromDevice(QByteArray a_data);
    virtual void sendDataToDevice(QByteArray a_data);

    /*!
     * \brief robot обработка входного сообщения
     * проверяем список робота - если есть инструкция для текущего устройства, то
     * выполняем ее.
     */
    void robot(QByteArray a_msg);

    /*!
     * \brief обновление списка возможных команд
     */
    virtual void updateSlaveControl();
signals:
    void receive(QByteArray);

private:
    Ui::COMDeviceBase *m_pui;
    QTimer *tim;
    QByteArray m_byteArray;
    int m_nSendCnt; //< оставшееся кол-во циклов отправки сообщения. если == -1 - бесонечное кол-во

private slots:
    /*!
     * \brief очистка поля принятых сообщений
     */
    void clearRecText();
    /*!
     * \brief отправка сообщения из текстового поля в подключенный порт
     */
    void sendText();
    /*!
     * \brief слот обработки событий таймера
     */
    void timUpdateEvent();
    /*!
     * \brief остановка таймера
     */
    void timStop();
    /*!
     * \brief обработка выбора элемента из списка
     */
    void doSlaveControl(QListWidgetItem *);
};

///@}
#endif // COMDEVICEBASE_H
