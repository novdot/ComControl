#ifndef COMDEVICEBASE_H
#define COMDEVICEBASE_H

#include "comdeviceinterface.h"

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <QMessageBox>

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
    void showRobotView();
    void updateSlaveControl();
    void timStop();

signals:

private:
    Ui::COMDeviceBase *m_pui;
    QTimer *tim; //< таймер для периодической отправки
    QByteArray m_byteArray; //< контейнер для отправки
    int m_nSendCnt; //< оставшееся кол-во циклов отправки сообщения. если == -1 - бесконечное кол-во

    QListWidget m_lRobotView; //< список событий для робота
    QListWidget m_lSlaveView; //< список событий для робота - список команд

    QMessageBox* m_pErrMsg;
private slots:
    /*!
     * \brief robot обработка входного сообщения
     * проверяем список робота - если есть инструкция для текущего устройства, то
     * выполняем ее.
     * \param[in] a_msg входное сообщение
     */
    void robot(QByteArray a_msg);
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
     * \brief doSlaveControl обработка выбора элемента из списка
     * \param[in] a_pItem элемент из списка
     */
    void doSlaveControl(QListWidgetItem * a_pItem);

    void sendListAll();

    void updateCurrentCmd2List();

    void addCurrentCmd2List();

    void openRobotList();
};

///@}
#endif // COMDEVICEBASE_H
