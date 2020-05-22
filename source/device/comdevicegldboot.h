#ifndef COMDEVICEGLDBOOT_H
#define COMDEVICEGLDBOOT_H

#include <QObject>
#include <QWidget>

#include "comdevicebase.h"

class COMDeviceGldBoot : public COMDeviceBase
{
    Q_OBJECT
public:
    COMDeviceGldBoot(QWidget*parent=nullptr);
private slots:
    void receiveHandler(QByteArray);
};

#endif // COMDEVICEGLDBOOT_H
