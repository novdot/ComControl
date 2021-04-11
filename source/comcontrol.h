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
#include <QList>
#include <QListWidgetItem>
#include <QMessageBox>

#include "comitem.h"
#include "protocolinterface.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FormComControl; }
QT_END_NAMESPACE

#define __ROBOT_PARSER_FIELD_LINKNAME "LINKNAME"
#define __ROBOT_PARSER_FIELD_MASTER_FORM "MASTER_FORM"
#define __ROBOT_PARSER_FIELD_MASTER_VAL "MASTER_VAL"
#define __ROBOT_PARSER_FIELD_SLAVE_FORM "SLAVE_FORM"
#define __ROBOT_PARSER_FIELD_SLAVE_VAL "SLAVE_VAL"
#define __ROBOT_PARSER_FIELD_END "#"

#define __ROBOT_PARSER_FIELD "*"
#define __ROBOT_PARSER_VALUE "="

#define NULL_COMITEM_PTR "null"

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
    ~ComControl();

public slots:
    /*!
        \brief Загрузить список всех созданных объектов
    */
    void setItemsList(QList<ComItem*> a_items);
signals:
    void setupRobot(QList<device_robot>);
private:
    Ui::FormComControl *m_pui;
    QList<device_robot> m_lRobot;
    QList<ComItem*> m_lItems;
    QMessageBox m_box;

    QList< QPair<int,QString> > m_lFormat; //< формат входных/выходных данных
    /*!
        \brief заполняем структуру из полей GUI
    */
    device_robot completeLinkFromFields();

private slots:
    /*!
        \brief Обработчик выбора записи из списка
    */
    void chooseRobotLine(QListWidgetItem *);
    /*!
        \brief Просто закрываем все
    */
    void updateRobotList();
    /*!
        \brief Просто закрываем все
    */
    void deny();
    /*!
        \brief Закрываем и список линков рассылаем по всем устройствам
    */
    void accept();

    /*!
        \brief Работа с текущими полями и списком всех линков
    */
    void addLink();
    void changeLink();
    void removeLink();

    /*!
        \brief Работа со списком линков
    */
    void save();
    void open();

};

///@}
#endif // COMCONTROL_H
