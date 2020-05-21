/*!
    \file comcontrol.h
    \brief Заголовочный файл с описанием класса управления элеиентами COM

    Содержит класс управления сотсояний и взаимосвязей между
    подключенными устройствами по COM

    \author dnovikov
    \mail novdot@mail.ru, novikov@electrooptika.ru
    \date 2019 12 20
 * */
#ifndef COMCONTROL_H
#define COMCONTROL_H

#include <QObject>
#include <QWidget>

/*!
    \defgroup Control Описатель диспетчера соединений
    \ingroup Dispatcher
    \brief  модуль, содержащий взаимосвязи
*/
///@{

/*!
    \brief класс управления COM соединений
    \author dnovikov
    \version
    \date 2019 12 20
    \warning на стадии разработки
 * */
class ComControl : public QWidget
{
    Q_OBJECT
public:
    explicit ComControl(QWidget *parent = nullptr);

signals:

};

///@}
#endif // COMCONTROL_H
