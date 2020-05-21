#include "comdispatcher.h"
#include "ui_comdispatcher.h"

#include <QThread>

/*****************************************************************************/
ComDispatcher::ComDispatcher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ComDispatcher)
{
    ui->setupUi(this);

    QString strProjName = "COMControl";
    ///TODO versions
    QString strProjVersion = "1.0.0";

    this->setWindowTitle(
                QObject::trUtf8("%1 v.%2")
                .arg(strProjName)
                .arg(strProjVersion)
                );

    ui->tabWidget->setTabsClosable(true);

    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int))
            ,this,SLOT(closeTab(int))
            );
}

ComDispatcher::~ComDispatcher()
{
    while(m_lpitems.count()!=0) this->closeTab(0);
    delete ui;
}

/*****************************************************************************/
void ComDispatcher::createTab()
{
    ComItem * pwgt = new ComItem();
    m_lpitems.append(pwgt);
    ui->tabWidget->addTab(pwgt,QObject::trUtf8("new COM"));
}

/*****************************************************************************/
void ComDispatcher::closeTab(int a_index)
{
    ui->tabWidget->removeTab(a_index);
    delete m_lpitems.takeAt(a_index);
}

/*****************************************************************************/
void ComDispatcher::renameTab(int a_index,QString a_name)
{
    ui->tabWidget->setTabText(a_index,a_name);
}

/*****************************************************************************/
void ComDispatcher::on_actionAdd_new_tab_triggered()
{
    this->createTab();
}
