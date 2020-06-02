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

#define COMITEM_NAME_DEFAULT "new COM"

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
    void renameTab(int a_index, QString a_strName);

    void itemConnected(void * a_pitem,QString a_strName);
    void itemDisconnect(void * a_pitem);
private slots:
    void on_actionAdd_new_tab_triggered();
    void on_actionOpen_link_editor_triggered();
    void on_actionQuit_triggered();

private:
    Ui::ComDispatcher *ui;
    QList<ComItem*> m_lpitems;
    ComControl* m_pcontrol;

    /*!
        \brief Вернет индекс вкладки основываясь на указателе виджета на вкладке
    */
    int searchTabIndexByItemPtr(ComItem * a_pitem);
};

///@}
#endif // MAINWINDOW_H
