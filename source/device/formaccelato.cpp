#include "formaccelato.h"
#include "ui_formaccelato.h"

#include <math.h>
#include <float.h>

#include <QTextCodec>
#include <QDebug>

#define CONNECT_BTN(_src_,_slot_)\
    connect(_src_,SIGNAL(clicked()),this,SLOT(_slot_))

#define APPEND_REG_16B(_array_,_data_) \
    _array_->append( (_data_>>8)&0xFF ); \
    _array_->append( (_data_>>0)&0xFF );

#define APPEND_REG_32B(_array_,_data_) \
    _array_->append( (_data_>>24)&0xFF ); \
    _array_->append( (_data_>>16)&0xFF ); \
    _array_->append( (_data_>>8)&0xFF ); \
    _array_->append( (_data_>>0)&0xFF );

#define READ_REG_16B(_array_,_ind_) \
    (_array_.at(_ind_+1)<<0) + (_array_.at(_ind_+0)<<8)

#define READ_REG_32B(_ind_) \
    (_array_.at(_ind_+3)<<0) + (_array_.at(_ind_+2)<<8) + (_array_.at(_ind_+1)<<16) + (_array_.at(_ind_+0)<<24)

#define MOD_NUMB 1000
/*****************************************************************************/
void commands_append_float(QByteArray* a_Array,float a_fData)
{
    double _fModT = 0.0; //остаток;
    double _fDivT = 0.0; //целая часть;
    uint16_t _nModT = 0;
    uint16_t _nDivT = 0;
    uint8_t _nSignT = 0;

    _fModT = modf(a_fData,&_fDivT);

    if(a_fData>=0) _nSignT = 0;
    else _nSignT = 1;

    _fModT = fabs(_fModT);
    _nDivT = fabs(_nDivT);

    _fModT = _fModT*MOD_NUMB;

    _nModT = (uint16_t)_fModT;
    _nDivT = (uint16_t)_fDivT;

    a_Array->append(_nSignT);
    APPEND_REG_16B(a_Array,_nDivT);
    APPEND_REG_16B(a_Array,_nModT);
}
#define APPEND_FDATA(a_Array,a_fData) commands_append_float(a_Array,a_fData)
/*****************************************************************************/
float commands_read_float(QByteArray a_Array,uint16_t ind)
{
    float _fModT = 0.0; //остаток
    float _fDivT = 0.0; //целая часть
    float _fValue = 0.0;
    uint16_t _nModT = 0;
    uint16_t _nDivT = 0;
    uint8_t _nSignT = 0;

    _nSignT = a_Array.at(ind+0);

    _nDivT = READ_REG_16B(a_Array,ind+1);
    _nModT = READ_REG_16B(a_Array,ind+3);

    _fValue = _nDivT*1.0 + _nModT/MOD_NUMB;

    if(_nSignT==1) _fValue *= -1.0;

    return _fValue;
}
#define READ_FLOAT(a_Array,ind) commands_read_float(a_Array,ind)
/*****************************************************************************/

FormAccelAto::FormAccelAto(QWidget *parent)
    : COMDeviceInterface(parent)
    , ui(new Ui::FormAccelAto)
{
    ui->setupUi(this);

    CONNECT_BTN(ui->pushButton_stop,startRecordTlm());
    CONNECT_BTN(ui->pushButton_start,stopRecordTlm());

    CONNECT_BTN(ui->pushButton_io_r,readIo());
    CONNECT_BTN(ui->pushButton_io_regs_r,readIoRegs());

    CONNECT_BTN(ui->pushButton_coef_func_r,readCoefFunc());

    CONNECT_BTN(ui->pushButton_coef_r_buf,readCoefBuff());
    CONNECT_BTN(ui->pushButton_coef_wr_buf,writeCoefBuff());
    CONNECT_BTN(ui->pushButton_coef_r_mem,readCoefMem());
    CONNECT_BTN(ui->pushButton_coef_wr_mem,writeCoefMem());

    CONNECT_BTN(ui->pushButton_temp_r,readTemp());
    CONNECT_BTN(ui->pushButton_temp_reg_r,readTempReg());

    CONNECT_BTN(ui->pushButton_temp_calibr_r,readTempCalibrationCoef());
    CONNECT_BTN(ui->pushButton_temp_calibr_wr,writeTempCalibrationCoef());
}

FormAccelAto::~FormAccelAto()
{
    delete ui;
}

/*****************************************************************************/
uint16_t FormAccelAto::calcCrc(QByteArray a_data)
{
    uint16_t crc = 0;
    for(int idata = 1; idata<a_data.count() ; idata++) {
        crc += a_data.at(idata);
    }
    return crc;
}
/*****************************************************************************/
bool FormAccelAto::checkCrc(QByteArray a_data)
{
    uint16_t nCrcCalc = 0;
    uint16_t nCrcReal = 0;
    QByteArray arrDataNoCrc = a_data.left(a_data.count() - 2);
    nCrcReal = calcCrc(arrDataNoCrc);
    nCrcCalc = READ_REG_16B(a_data,a_data.count() - 2);

    if(nCrcReal!=nCrcCalc) return false;
    else return true;
}
/*****************************************************************************/
int FormAccelAto::getCmdLenght(uint8_t a_nCmdCode)
{
    switch(a_nCmdCode){
    case COMMAND_CODE_COEF_BUF_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_BUF_R);
    case COMMAND_CODE_COEF_BUF_WR: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_BUF_WR);
    case COMMAND_CODE_COEF_MEM_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_MEM_R);
    case COMMAND_CODE_COEF_MEM_WR: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_MEM_WR);
    case COMMAND_CODE_COEF_FUNC_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_FUNC_R);

    case COMMAND_CODE_TEMP_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_TEMP_R);
    case COMMAND_CODE_TEMP_REG_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_TEMP_REG_R);
    case COMMAND_CODE_DATA_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_DATA_R);
    case COMMAND_CODE_DATA_REG_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_DATA_REG_R);

    case COMMAND_CODE_COEF_TEMP_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_TEMP_R);
    case COMMAND_CODE_COEF_TEMP_WR: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_TEMP_WR);
    case COMMAND_CODE_COEF_TEMP_MEM_R: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_TEMP_MEM_R);
    case COMMAND_CODE_COEF_TEMP_MEM_WR: return COMPLETE_PACKET_SIZE(ANS_SIZE_COEF_TEMP_MEM_WR);
    case COMMAND_CODE_TEMP_CALIBRATION_RUN: return COMPLETE_PACKET_SIZE(ANS_SIZE_TEMP_CALIBRATION_RUN);
    case COMMAND_CODE_TEMP_CALIBRATION_STOP: return COMPLETE_PACKET_SIZE(ANS_SIZE_TEMP_CALIBRATION_STOP);
    default:
        return 0;
        break;
    }
}

/*****************************************************************************/
void FormAccelAto::visualCmd(QByteArray a_dataCmd)
{
    uint8_t nCmdCode = 0;
    if((uint8_t)a_dataCmd.at(COMMAND_IND_SIGN)!=COMMAND_ANS_SIGN) return;

    nCmdCode = (uint8_t)a_dataCmd.at(COMMAND_IND_CODE);

    switch(nCmdCode){
    case COMMAND_CODE_COEF_BUF_R:
        break;
    case COMMAND_CODE_COEF_BUF_WR:
        break;
    case COMMAND_CODE_COEF_MEM_R:
        break;
    case COMMAND_CODE_COEF_MEM_WR:
        break;
    case COMMAND_CODE_COEF_FUNC_R:
        break;

    case COMMAND_CODE_TEMP_R:
        break;
    case COMMAND_CODE_TEMP_REG_R:
        break;
    case COMMAND_CODE_DATA_R:
        break;
    case COMMAND_CODE_DATA_REG_R:
        break;

    case COMMAND_CODE_COEF_TEMP_R:
        break;
    case COMMAND_CODE_COEF_TEMP_WR:
        break;
    case COMMAND_CODE_COEF_TEMP_MEM_R:
        break;
    case COMMAND_CODE_COEF_TEMP_MEM_WR:
        break;
    case COMMAND_CODE_TEMP_CALIBRATION_RUN:
        break;
    case COMMAND_CODE_TEMP_CALIBRATION_STOP:
        break;
    default:
        break;
    }
}

/*****************************************************************************/
QByteArray FormAccelAto::parseCmd(QByteArray*a_pdata)
{
    bool isInCmd = false; // флаг того, что нашли начало команды
    QByteArray cmd;
    int size;

    for(int idata = 0; idata<a_pdata->count() ; idata++) {
        //проверим начало команды
        if((uint8_t)a_pdata->at(idata) != COMMAND_ANS_SIGN)
            continue;

        //если был отбит конец команды - выкинули мусор перед
        //a_pdata->remove(0,idata);
        *a_pdata = a_pdata->mid(idata);
        isInCmd = true;
        break;
    }

    //если в приемном буффере нет признака команды - почистим буффер
    //и выйдем из обработчика
    if(!isInCmd){
        a_pdata->clear();
        return cmd;
    }

    //определим длину команды по ее коду
    size = getCmdLenght((uint8_t)a_pdata->at(COMMAND_IND_CODE));

    //если не нашли команду
    if(size == 0){
        //уберем код команды
        *a_pdata = a_pdata->mid(1);
        //покинем обработчик
        return cmd;
    }

    //если размер команды превышает буффер - выйдем. необходимо дождаться след пакета
    if(size > a_pdata->count()){
        return cmd;
    }

    //выделим принятую команду из буффера
    cmd = a_pdata->left(size);
    *a_pdata = a_pdata->mid(size);

    //проверим CRC
    if(checkCrc(cmd)) return cmd;
    else {
        cmd.clear();
        return cmd;
    }
}
/*****************************************************************************/
void FormAccelAto::receiveDataFromDevice(QByteArray a_data)
{
    static QByteArray data;//остатки
    int index_start_command = 0;//
    int index_end_command = 0;//
    QList<QByteArray> packets;
    QByteArray packet;

    emit receiveDone(a_data);

    //qDebug() << a_data.toHex();
    //дописали
    data.append(a_data);

    //разобьем на пакеты входной буффер
    while(1){
        packet = parseCmd(&data);
        if(packet.count()==0 || data.count()==0)
            break;
    }

}
/*****************************************************************************/
void FormAccelAto::sendDataToDevice(QByteArray a_data)
{
    emit send(a_data);
}
/*****************************************************************************/
void FormAccelAto::showRobotView()
{

}
/*****************************************************************************/
void FormAccelAto::updateSlaveControl()
{

}
/*****************************************************************************/
void FormAccelAto::timStop()
{

}
/*****************************************************************************/
void FormAccelAto::startRecordTlm()
{
    //run timer
}
/*****************************************************************************/
void FormAccelAto::stopRecordTlm()
{
    //stop timer
}
/*****************************************************************************/
void FormAccelAto::readIo()
{

}
/*****************************************************************************/
void FormAccelAto::readIoRegs()
{

}
/*****************************************************************************/
void FormAccelAto::readCoefFunc()
{

}
/*****************************************************************************/
void FormAccelAto::readCoefBuff()
{

}
/*****************************************************************************/
void FormAccelAto::writeCoefBuff()
{

}
/*****************************************************************************/
void FormAccelAto::readCoefMem()
{

}
/*****************************************************************************/
void FormAccelAto::writeCoefMem()
{

}
/*****************************************************************************/
void FormAccelAto::readTemp()
{
    QByteArray data;
    uint16_t crc = 0;

    data.append(COMMAND_CMD_SIGN);
    data.append(COMMAND_CODE_TEMP_R);

    crc = calcCrc(data);
    data.append((char)(crc<<8)&0xFF);
    data.append((char)(crc<<0)&0xFF);

    sendDataToDevice(data);
}
/*****************************************************************************/
void FormAccelAto::readTempReg()
{
    QByteArray data;
    uint16_t crc = 0;

    data.append(COMMAND_CMD_SIGN);
    data.append(COMMAND_CODE_TEMP_REG_R);

    crc = calcCrc(data);
    data.append((char)(crc<<8)&0xFF);
    data.append((char)(crc<<0)&0xFF);

    sendDataToDevice(data);
}
/*****************************************************************************/
void FormAccelAto::readTempCalibrationCoef()
{
    QByteArray data;
    uint16_t crc = 0;

    data.append(COMMAND_CMD_SIGN);
    data.append(COMMAND_CODE_COEF_TEMP_R);

    crc = calcCrc(data);
    data.append((char)(crc<<8)&0xFF);
    data.append((char)(crc<<0)&0xFF);

    sendDataToDevice(data);
}
/*****************************************************************************/
void FormAccelAto::writeTempCalibrationCoef()
{
    QByteArray data;
    QByteArray* pdata;
    uint16_t crc = 0;
    int32_t shift = 0;
    float scale = 0;
    float norm = 0;

    pdata = &data;

    data.append(COMMAND_CMD_SIGN);
    data.append(COMMAND_CODE_COEF_TEMP_WR);

    ui->lineEdit_temp_calibr_shift->text();
    ui->lineEdit_temp_calibr_scale->text();
    ui->lineEdit_temp_calibr_normal_temp->text();

    APPEND_REG_32B(pdata,shift);
    APPEND_FDATA(&data,scale);
    APPEND_FDATA(&data,norm);

    crc = calcCrc(data);
    data.append((char)(crc<<8)&0xFF);
    data.append((char)(crc<<0)&0xFF);

    sendDataToDevice(data);
}
/*****************************************************************************/
