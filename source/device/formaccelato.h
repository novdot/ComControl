#ifndef FORMACCELATO_H
#define FORMACCELATO_H

#include "../comdeviceinterface.h"

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class FormAccelAto; }
QT_END_NAMESPACE

//CONST
#define COMMAND_IND_MAX 128
#define MOD_NUMB 1000
#define APPEND_FDATA_SIZE 5
#define APPEND_REG_16B_SIZE 2
#define APPEND_REG_32B_SIZE 4

//SIGN
#define COMMAND_CMD_SIGN 0xcc
#define COMMAND_ANS_SIGN 0xaa

//CODE
#define COMMAND_CODE_COEF_BUF_R				0x01 //<коэффициенты термокалибровки;
#define COMMAND_CODE_COEF_BUF_WR			0x02 //<коэффициенты термокалибровки;
#define COMMAND_CODE_COEF_MEM_R				0x03 //<коэффициенты термокалибровки;
#define COMMAND_CODE_COEF_MEM_WR			0x04 //<коэффициенты термокалибровки;
#define COMMAND_CODE_COEF_FUNC_R			0x05 //<коэф являются функциями температуры термодатчика;

#define COMMAND_CODE_TEMP_R					0x06 //<температура в цельсиях;
#define COMMAND_CODE_TEMP_REG_R				0x07 //<температура из регистра;
#define COMMAND_CODE_DATA_R					0x08 //<температура,оси акселлер, оси ЦАП;
#define COMMAND_CODE_DATA_REG_R				0x09 //<температура,оси акселлер, оси ЦАП в регистрах;

#define COMMAND_CODE_COEF_TEMP_R			0x0a //<коэф температуры с датчика;
#define COMMAND_CODE_COEF_TEMP_WR			0x0b //<коэф температуры с датчика;
#define COMMAND_CODE_COEF_TEMP_MEM_R		0x0c //<коэф температуры с датчика;
#define COMMAND_CODE_COEF_TEMP_MEM_WR		0x0d //<коэф температуры с датчика;
#define COMMAND_CODE_TEMP_CALIBRATION_RUN	0x0e //<запуск калибровки;
#define COMMAND_CODE_TEMP_CALIBRATION_STOP	0x0f //<остановка калибровки;

//SIZE DATA CMD
#define COMMAND_SIZE_COEF_BUF_R		0
#define COMMAND_SIZE_COEF_BUF_WR	APPEND_REG_16B_SIZE*48
#define COMMAND_SIZE_COEF_MEM_R		0
#define COMMAND_SIZE_COEF_MEM_WR	0
#define COMMAND_SIZE_COEF_FUNC_R	0

#define COMMAND_SIZE_TEMP_R			0
#define COMMAND_SIZE_TEMP_REG_R		0
#define COMMAND_SIZE_DATA_R			0
#define COMMAND_SIZE_DATA_REG_R		0

#define COMMAND_SIZE_COEF_TEMP_R		0
#define COMMAND_SIZE_COEF_TEMP_WR		APPEND_REG_32B_SIZE+APPEND_FDATA_SIZE*2
#define COMMAND_SIZE_COEF_TEMP_MEM_R		0
#define COMMAND_SIZE_COEF_TEMP_MEM_WR		0
#define COMMAND_SIZE_TEMP_CALIBRATION_RUN	 0
#define COMMAND_SIZE_TEMP_CALIBRATION_STOP	 0

//SIZE DATA ANS
#define ANS_SIZE_COEF_BUF_R		APPEND_REG_16B_SIZE*48
#define ANS_SIZE_COEF_BUF_WR	0
#define ANS_SIZE_COEF_MEM_R		0
#define ANS_SIZE_COEF_MEM_WR	0
#define ANS_SIZE_COEF_FUNC_R	APPEND_FDATA_SIZE*9+APPEND_REG_16B_SIZE*3

#define ANS_SIZE_TEMP_R			APPEND_FDATA_SIZE
#define ANS_SIZE_TEMP_REG_R		APPEND_REG_16B_SIZE
#define ANS_SIZE_DATA_R			APPEND_FDATA_SIZE*7
#define ANS_SIZE_DATA_REG_R		APPEND_REG_16B_SIZE*7

#define ANS_SIZE_COEF_TEMP_R		APPEND_REG_32B_SIZE+APPEND_FDATA_SIZE*2
#define ANS_SIZE_COEF_TEMP_WR		0
#define ANS_SIZE_COEF_TEMP_MEM_R	0
#define ANS_SIZE_COEF_TEMP_MEM_WR		0
#define ANS_SIZE_TEMP_CALIBRATION_RUN	 0
#define ANS_SIZE_TEMP_CALIBRATION_STOP	 0

//INDEX
#define COMMAND_IND_SIGN 0
#define COMMAND_IND_CODE 1
#define COMMAND_IND_DATA_START 2

#define COMPLETE_PACKET_SIZE(data) COMMAND_IND_DATA_START+data+2

class FormAccelAto : public COMDeviceInterface
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
signals:
public slots:
    void receiveDataFromDevice(QByteArray a_data);
    void sendDataToDevice(QByteArray a_data);
    void showRobotView();
    void updateSlaveControl();
    void timStop();

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

};

#endif // FORMACCELATO_H
