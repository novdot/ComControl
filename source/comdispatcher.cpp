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

    m_pcontrol = new ComControl();
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
    ui->tabWidget->addTab(pwgt,QObject::trUtf8(COMITEM_NAME_DEFAULT));
    connect(pwgt,SIGNAL(connected(ComItem *,QString)),this,SLOT(itemConnected(ComItem *,QString)));
    connect(pwgt,SIGNAL(disconnected(ComItem *)),this,SLOT(itemDisconnect(ComItem *)));
}

/*****************************************************************************/
void ComDispatcher::itemConnected(ComItem * a_pitem,QString a_strName)
{
    this->renameTab(searchTabIndexByItemPtr(a_pitem),a_strName);
    m_pcontrol->setItemsList(m_lpitems);
}

/*****************************************************************************/
void ComDispatcher::itemDisconnect(ComItem * a_pitem)
{
    this->renameTab(searchTabIndexByItemPtr(a_pitem),COMITEM_NAME_DEFAULT);
    m_pcontrol->setItemsList(m_lpitems);
}

/*****************************************************************************/
int ComDispatcher::searchTabIndexByItemPtr(ComItem * a_pitem)
{
    int nTabMax = ui->tabWidget->count();
    for(int iTab=0;iTab<nTabMax;iTab++) {
        if(ui->tabWidget->widget(iTab) == (QWidget*)a_pitem )
            return iTab;
    }
    return -1;
}

/*****************************************************************************/
void ComDispatcher::closeTab(int a_index)
{
    if(a_index<0 || a_index>ui->tabWidget->count() ) return;
    ui->tabWidget->removeTab(a_index);
    delete m_lpitems.takeAt(a_index);
}

/*****************************************************************************/
void ComDispatcher::renameTab(int a_index,QString a_name)
{
    if(a_index<0 || a_index>ui->tabWidget->count() ) return;
    ui->tabWidget->setTabText(a_index,a_name);
}

/*****************************************************************************/
void ComDispatcher::on_actionAdd_new_tab_triggered()
{
    this->createTab();
}
/*****************************************************************************/
void ComDispatcher::on_actionOpen_link_editor_triggered()
{
    if(m_pcontrol)
        m_pcontrol->show();
}
/*****************************************************************************/
void ComDispatcher::on_actionQuit_triggered()
{
    exit(0);
}


