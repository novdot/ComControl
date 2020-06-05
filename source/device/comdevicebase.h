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
    \brief COMDeviceBase класс устройства
    \author dnovikov
    \date 2020 01 17
    \warning на стадии разработки
    \todo robot list view
 * */
class COMDeviceBase : public COMDeviceInterface
{
    Q_OBJECT
public:
    COMDeviceBase(QWidget *parent = nullptr);
    ~COMDeviceBase();
    /*!
     * \brief convertStr2ByteArray перевод строки в QByteArray
     * \param[in] nFormatInd формат информации
     * \param[in] data сообщение для перекодирования
     * \return выходное сообщение
     */
    static QByteArray convertStr2ByteArray(int nFormatInd,QString data);
    /*!
     * \brief convertByteArray2Str переводит QByteArray в строку
     * \param[in] nFormatInd формат информации
     * \param[in] data сообщение для перекодирования
     * \return выходное сообщение
     */
    static QString convertByteArray2Str(int nFormatInd,QByteArray data);

public slots:
    void receiveDataFromDevice(QByteArray a_data);
    void sendDataToDevice(QByteArray a_data);

    /*!
     * \brief robot обработка входного сообщения
     * проверяем список робота - если есть инструкция для текущего устройства, то
     * выполняем ее.
     * \param[in] a_msg входное сообщение
     */
    void robot(QByteArray a_msg);
    void showRobotView();
    void updateSlaveControl();
signals:
    /*!
     * \brief receive сигнал о полученном пакете
     */
    void receive(QByteArray);

private:
    Ui::COMDeviceBase *m_pui;
    QTimer *tim;
    QByteArray m_byteArray;
    int m_nSendCnt; //< оставшееся кол-во циклов отправки сообщения. если == -1 - бесонечное кол-во

    QListWidget m_lRobotView;

private slots:
    /*!
     * \brief clearRecText очистка поля принятых сообщений
     */
    void clearRecText();
    /*!
     * \brief sendText отправка сообщения из текстового поля в подключенный порт
     */
    void sendText();
    /*!
     * \brief timUpdateEvent слот обработки событий таймера
     */
    void timUpdateEvent();
    /*!
     * \brief timStop остановка таймера
     */
    void timStop();
    /*!
     * \brief doSlaveControl обработка выбора элемента из списка
     * \param[in] a_pItem элемент из списка
     */
    void doSlaveControl(QListWidgetItem * a_pItem);
};

///@}
#endif // COMDEVICEBASE_H
