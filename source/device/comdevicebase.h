#ifndef COMDEVICEBASE_H
#define COMDEVICEBASE_H

#include "comdeviceinterface.h"

#include <QObject>
#include <QWidget>

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
    \brief класс устройства
    \author dnovikov
    \date 2020 01 17
    \warning на стадии разработки
 * */
class COMDeviceBase : public COMDeviceInterface
{
    Q_OBJECT
public:
    COMDeviceBase(QWidget *parent = nullptr);
    ~COMDeviceBase();

public slots:
    virtual void receiveDataFromDevice(QByteArray a_data);

private:
    Ui::COMDeviceBase *m_pui;

private slots:
    void clearRecText();
    void sendText();
};

///@}
#endif // COMDEVICEBASE_H
