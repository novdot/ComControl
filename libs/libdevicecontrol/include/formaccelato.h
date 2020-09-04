#ifndef FORMACCELATO_H
#define FORMACCELATO_H

#include "protocolinterface.h"

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class FormAccelAto; }
QT_END_NAMESPACE

class FormAccelAto : public ProtocolInterface
{
    Q_OBJECT

public:
    explicit FormAccelAto(QWidget *parent = nullptr);
    ~FormAccelAto();

private:
    Ui::FormAccelAto *ui;

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
        \brief Чтение коэффициенты термокалибровки из память
    */
    void readCoefMem();
    /*!
        \brief Запись коэффициенты термокалибровки в память
    */
    void writeCoefMem();
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

};

#endif // FORMACCELATO_H
