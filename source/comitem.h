/*!
    \file comitem.h
    \brief Заголовочный файл с описанием класса COM соединения

    Содержит класс управления сотсояний устройства по COM

    \author dnovikov
    \mail novdot@mail.ru, novikov@electrooptika.ru
    \date 2019 12 08
 * */
#ifndef COMITEM_H
#define COMITEM_H

#include <QObject>
#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QTime>
#include <QPlainTextEdit>

#include "protocolinterface.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FormComItem; }
QT_END_NAMESPACE

/*!
    \defgroup ComItem Описатель COM соединения
    \ingroup Dispatcher
    \brief Основной модуль, содержащей в себе описания  управления
состояний устройства по COM.
    Также содержит описание протоколов обмена.
*/
///@{

///структура настройки COM порта
typedef struct com_settingsDef {
    QString name; ///< Название
    uint32_t baudRate;///< Скорость
    //QSerialPort::Direction
    //QSerialPort::PinoutSignal
    QSerialPort::DataBits dataBits;///< Кол-во бит
    QSerialPort::Parity parity;///<
    QSerialPort::StopBits stopBits;///<
    QSerialPort::FlowControl flowControl;///<
} com_settings;

/// Набор состояний
typedef enum com_stateDef{
    _state_unknown ///< неизвестное состояние
    ,_state_setup ///< состояние настройки соединения
    ,_state_connected ///< состояние установленного соединения
}com_state;

//индексы вкладок
#define __TAB_SETUPS_IND 0
#define __TAB_DEVICE_IND 1

//константы
#define __FIELD_MANUAL_BAUD "manual"

//парсер
#define __CONF_BAUD "BAUD"
#define __CONF_DATABITS "DATABITS"
#define __CONF_PARITY "PARITY"
#define __CONF_STOPBITS "STOPBITS"
#define __CONF_FLOWCONTROL "FLOWCONTROL"
#define __CONF_DEVICE "DEVICE"

#define __CONF_FIELD "*"
#define __CONF_VALUE "="

/// структура полей настроек
typedef struct com_settings_fieldDef {
    QString name; ///< имя
    int id; ///< идентификатор
} com_settings_field;

/*!
    \brief класс COM соединения
    \author dnovikov
    \date 2019 12 08
    \warning на стадии разработки
 * */
class ComItem : public QWidget
{
    Q_OBJECT
public:
    ComItem(QWidget *parent = 0);
    ~ComItem();
    /*!
     * \brief получить настройки
    */
    com_settings getSetups(){return m_settings;}

    /*!
     * \brief получить текущее имя соединения
    */
    QString getItemName() {return m_strComItemName;}
    /*!
     * \brief получить текущее состояние соединения
    */
    bool isConnected() {return m_port.isOpen();}

    /*!
     * \brief получить текущее устройство
    */
    void* getDevice(){return m_pDevice;}

public slots:
    /*!
     * \brief setDevice установить номер и тип устройства(COM1)
     * \param[in] a_strDevName имя устройства
     */
    void setDevice(QString a_strDevName);
    /*!
     * \brief Установить соединение с текущим портом
    */
    void doConnect();
    /*!
     * \brief Проинициализировать настройки
     * и подключиться к текущему порту
    */
    void startConnect();
    /*!
     * \brief Отключиться от текущего порта
    */
    void startDisconnect();
    /*!
     * \brief Сохранение настроек COM порта
     * (в том числе и установка текущего порта)
     * \param[in] com_settings Структура настроек
    */
    void doSetup(com_settings a_settings);
    /*!
     * \brief Отправка данных в текущий порт
     * \param[in] a_data Исходный контейнер данных
    */
    void sendData(const QByteArray &a_data);
    /*!
     * \brief Обновление полей управления
    */
    void updateSetupFields();
    /*!
     * \brief Обновление списка портов
    */
    void updateCOMLists();
    /*!
     * \brief Включение - выключение логгирования
    */
    void toggleLogger();

signals:
    /*!
     * \brief Данные отправлены
    */
    void sendedData();
    /*!
     * \brief Приход данных из порта
     * \param[in] a_data принятые данные
    */
    void readData(QByteArray a_data);
    /*!
     * \brief Ошибка обработчика порта
     * \param[in] a_strerror строка с описанием ошибки
    */
    void error(QString a_strerror);
    /*!
     * \brief connected - сигнал установленного соединения.
     * \param[in] ptr указатель на источник
     * \param[in] name строка с именем устройства
     */
    void connected(void* ptr,QString name);
    /*!
     * \brief disconnected - сигнал разорванного соединения.
     * \param[in] ptr указатель на источник
     */
    void disconnected(void* ptr);

private:
    Ui::FormComItem *m_pui;
    com_settings m_settings;
    com_state m_state;
    bool m_bIsLogger;
    void* m_pDevice;
    QSerialPort m_port;
    QPlainTextEdit m_log;

    QString m_strDeviceName;

    //имя объекта
    //нужно для однозначного определения , что данный объект подключен к порту
    QString m_strComItemName;

    //сервисная группа полей
    //нужны для динамического отображения подключенных устройств
    QTimer m_timService;
    QStringList m_listCOMDev_prev;
    QString m_strCOMDev_prev;

    //список настроек для порта
    QList<com_settings_field> m_lBaud;
    QList<com_settings_field> m_lDatabits;
    QList<com_settings_field> m_lParity;
    QList<com_settings_field> m_lStopbits;
    QList<com_settings_field> m_lFlowcontrol;
    QList<com_settings_field> m_lDevice;
    /*!
     * \brief заполнение списков настроек порта
    */
    void initLists();
    /*!
     * \brief инициализация соединений сигнал-слот
    */
    void initSignalSlotConn();
    /*!
     * \brief собирает структуру настроек. данные тянем из полей формы
     * \return возвращает структуру настроек
    */
    com_settings readSetups();

    /*!
     * \brief заполнение полей управления
    */
    void fillSetupFields();

    /*!
     * \brief
    */
    void initStatus();

private slots:
    /*!
     * \brief загружаем конфигурацию настроек порта. обновляем поля формы загруженными настройками
    */
    void loadConfigFile();
    /*!
     * \brief сохраняем файл конфигурации настроек порта. данные тянем из полей формы
    */
    void saveConfigFile();
    /*!
     * \brief выбор файла из файловой системы
    */
    void chooseConfigFile();
    /*!
     * \brief доступ к полю ручного ввода BaudRate
    */
    void enableManualBaud(QString a_strCurrentBaudSetup);
    /*!
     * \brief Слот блокировки выбранного элемента
     * Нужен для блокировки меню выбора Серийных портов в системе
     * Позволяет при подключении-отключении порта в системе обновлять список лоступных портов
     * и не производить переключения текущего порта(если он существует)
     * \param[in] flag
     * 0 - снять блокировку;
     * 1 - установить блокировку;
    */
    void lockCOMDevice(int flag);
    /*!
     * \brief Слот обработки ошибок QSerialPort
     * \param[in] error код ошибки
    */
    void handleError(QSerialPort::SerialPortError error);
    /*!
     * \brief Слот чтения из порта по ReadyRead
    */
    void readPort();
    /*!
     * \brief setComDescription слот установки описания выбранного порта
     * \param[in] a_strDescript код ошибки
     */
    void setComDescription(QString a_strDescript);
    /*!
     * \brief openLog слот запуска лога для данного подключения
     */
    void openLog();
    /*!
     * \brief add2Log запись в лог данных
     * \param[in] data строка в лог
     */
    void add2Log(QString data);
    /*!
     * \brief add2Log запись в лог входных данных
     * \param[in] data строка в лог
     */
    void add2LogInput(QByteArray data);
    /*!
     * \brief add2Log запись в лог выходных данных
     * \param[in] data строка в лог
     */
    void add2LogOutput(QByteArray data);

    void statusErrorOn(QSerialPort::SerialPortError);
    void statusErrorOff();
    void statusTxOn();
    void statusTxOff();
    void statusRxOn();
    void statusRxOff();
    void statusConnectOn();
    void statusConnectOff();

    void setupsDeviceConnections(QString index);
};

///@}
#endif // COMITEM_H
