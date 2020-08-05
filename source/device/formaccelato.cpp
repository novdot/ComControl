#include "formaccelato.h"
#include "ui_formaccelato.h"

FormAccelAto::FormAccelAto(QWidget *parent)
    : COMDeviceInterface(parent)
    , ui(new Ui::FormAccelAto)
{
    ui->setupUi(this);
}

FormAccelAto::~FormAccelAto()
{
    delete ui;
}

/*****************************************************************************/
void FormAccelAto::receiveDataFromDevice(QByteArray a_data)
{

}
/*****************************************************************************/
void FormAccelAto::sendDataToDevice(QByteArray a_data)
{

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
