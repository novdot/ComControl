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
    /*!
        \brief Обработчик создания новой вкладки и объекта.
    */
    void createTab();
    /*!
        \brief Обработчик закрытия вкладки и уничтожение объекта.
        \param[in] a_index индекс владки
    */
    void closeTab(int a_index);
    /*!
        \brief Обработчик изменения названия вкладки по ее номеру
        \param[in] a_index индекс вкладки
        \param[in] a_strName имя вкладки
    */
    void renameTab(int a_index, QString a_strName);

    /*!
        \brief Обработчик подключения объекта к серийному порту.
        \param[in] a_pitem указатель на объект
        \param[in] a_strName имя объекта
    */
    void itemConnected(void * a_pitem,QString a_strName);
    /*!
        \brief Обработчик отключения объекта от серийного порта.
        \param[in] a_pitem указатель на объект
    */
    void itemDisconnect(void * a_pitem);
private slots:
    /*!
        \brief Обработчик создания новой вкладки и объекта.
    */
    void on_actionAdd_new_tab_triggered();
    /*!
        \brief Обработчик открытия настройщика робота.
    */
    void on_actionOpen_link_editor_triggered();
    /*!
        \brief Обработчик открытия страницы о проекте
    */
    void on_actionAbout_triggered();
    /*!
        \brief Обработчик закрытие приложения.
    */
    void on_actionQuit_triggered();

private:
    Ui::ComDispatcher *ui;
    QList<ComItem*> m_lpitems;
    ComControl* m_pcontrol;
    QPlainTextEdit m_strAbout;

    /*!
        \brief Вернет индекс вкладки основываясь на указателе виджета на вкладке
        \param[in] a_pitem указатель на объект
        \return индекс вкладки
    */
    int searchTabIndexByItemPtr(ComItem * a_pitem);
};

///@}
#endif // MAINWINDOW_H
