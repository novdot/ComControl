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

class FormAccelAto : public COMDeviceInterface
{
    Q_OBJECT

public:
    explicit FormAccelAto(QWidget *parent = nullptr);
    ~FormAccelAto();

private:
    Ui::FormAccelAto *ui;
signals:
public slots:
    void receiveDataFromDevice(QByteArray a_data);
    void sendDataToDevice(QByteArray a_data);
    void showRobotView();
    void updateSlaveControl();
    void timStop();

private slots:
};

#endif // FORMACCELATO_H
