#ifndef FORMACCELATO_H
#define FORMACCELATO_H

#include "protocolinterface.h"

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class FormAccelAto; }
QT_END_NAMESPACE

#define SETUPS_ACCELEROMETR_ATO_NAME        ("ACCELEROMETR ATO")
#define SETUPS_ACCELEROMETR_ATO_baudrate    ("57600")
#define SETUPS_ACCELEROMETR_ATO_databits    ("8")
#define SETUPS_ACCELEROMETR_ATO_parity      ("NoParity")
#define SETUPS_ACCELEROMETR_ATO_stopbits    ("OneStop")
#define SETUPS_ACCELEROMETR_ATO_flowcontrol ("NoFlowControl")
#define SETUPS_ACCELEROMETR_ATO_CLASS       FormAccelAto

typedef enum{
    _status_ready = 0
    , _status_warning
    , _status_error
}ato_status_code;

class FormAccelAto : public ProtocolInterface
{
    Q_OBJECT

public:
    explicit FormAccelAto(QWidget *parent = nullptr);
    ~FormAccelAto();

private:
    Ui::FormAccelAto *ui;

    bool m_bWrite2File;
    QFile m_File;

    int m_nCoefTableWr;

    void setStatus(QString msg, ato_status_code code );

    uint16_t calcCrc(QByteArray a_data);
    bool checkCrc(QByteArray a_data);
    /*!
        \brief Обработка входного буффера для выделения буффера команды
        \param[in] a_pdata указатель на входной буффер
        \return вернем команду
    */
    QByteArray parseCmd(QByteArray*a_pdata);
    /*!
        \brief Размер(байт) команды по ее коду
        \param[in] a_nCmdCode код команды
        \return вернем размер команды
    */
    int getCmdLenght(uint8_t a_nCmdCode);

    /*!
        \brief Отображение команды
    */
    void visualCmd(QByteArray a_dataCmd);

    void commands_append_float(QByteArray* a_Array,float a_fData);
    float commands_read_float(QByteArray a_Array,uint16_t ind);
signals:
public slots:
    void receiveDataFromDevice(QByteArray a_data);
    void sendDataToDevice(QByteArray a_data);
    void showRobotView();
    void updateSlaveControl();
    void timStop();
    void timUpdateEvent();

private slots:
    /*!
        \brief Старт запроса, получения и сохранения телеметрии с прибора
    */
    void startRecordTlm();
    /*!
        \brief Остановка запроса, получения и сохранения телеметрии с прибора
    */
    void stopRecordTlm();
    /*!
        \brief Чтение пересчитанных значений ввода-вывода прибора
    */
    void readIo();
    /*!
        \brief Чтение регистров значений ввода-вывода прибора
    */
    void readIoRegs();
    /*!
        \brief Чтение коэф (являются функциями температуры термодатчика);
    */
    void readCoefFunc();
    /*!
        \brief Чтение коэффициенты термокалибровки из буффера
    */
    void readCoefBuff();
    /*!
        \brief Запись коэффициенты термокалибровки в буффера
    */
    void writeCoefBuff();
    /*!
        \brief Чтение коэффициенты термокалибровки из file
    */
    void readCoefFile();
    /*!
        \brief Запись коэффициенты термокалибровки в file
    */
    void writeCoefFile();
    /*!
        \brief
    */
    void testCoef();
    /*!
        \brief Чтение температуры пересчитанной
    */
    void readTemp();
    /*!
        \brief Чтение температуры из регистра
    */
    void readTempReg();
    /*!
        \brief Чтение коэф калибровки температуры
    */
    void readTempCalibrationCoef();
    /*!
        \brief Запись коэф калибровки температуры
    */
    void writeTempCalibrationCoef();
    /*!
        \brief Обработка изменения ячейки в таблице
    */
    void tableChangeCoef(QTableWidgetItem *item);
    /*!
        \brief Создать файл логов
    */
    void createTlmFile();
    /*!
        \brief Добавить в файл строку
    */
    void appendLine2File(float t,float x,float y, float z, float u_x, float u_y, float u_z);

};

#endif // FORMACCELATO_H
