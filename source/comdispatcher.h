#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QThread>

#include "comitem.h"
#include "comcontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ComDispatcher; }
QT_END_NAMESPACE

/*!
    \defgroup Dispatcher Описатель диспетчера соединений
    \brief Основной модуль, содержащий управление объектами соединений по раличным интерфейсам
*/
///@{

/*!
    \brief класс диспетчера COM соединений
    \author dnovikov
    \version
    \date 2019 12 08
    \warning на стадии разработки
 * */
class ComDispatcher : public QMainWindow
{
    Q_OBJECT

public:
    ComDispatcher(QWidget *parent = nullptr);
    ~ComDispatcher();

public slots:
    void createTab();
    void closeTab(int a_index);
    void renameTab(int a_index, QString a_strname);

private slots:
    void on_actionAdd_new_tab_triggered();

private:
    Ui::ComDispatcher *ui;
    QList<ComItem*> m_lpitems;
    ComControl* m_pcontrol;
};

///@}
#endif // MAINWINDOW_H
