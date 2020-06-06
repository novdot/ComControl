#include "comdispatcher.h"
#include "ui_comdispatcher.h"

#include <QThread>

/*****************************************************************************/
ComDispatcher::ComDispatcher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ComDispatcher)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":icon/serial_port_icon.png"));

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

    m_strAbout.setReadOnly(true);
    m_strAbout.appendHtml(tr("<h4>О проекте</h4>"));
    m_strAbout.appendHtml(tr("COMControl ver.:%1 date:%2").arg(GIT_VERSION).arg(GIT_TIMESTAMP));
    m_strAbout.appendHtml(tr("<p>Проект разработан на С++ с использованием библиотеки Qt</p><p>Проект на GitHub:<a href=\"https://github.com/novdot/comcontrol.git\">https://github.com/novdot/comcontrol.git</a></p>"));
    m_strAbout.appendHtml(tr("Сайт с описанием проекта: <a href=\"https://http://idea2art.ru/content_item#78\">https://idea2art.ru/content_item#78</a>"));
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
    connect(pwgt,SIGNAL(connected(void*,QString)),this,SLOT(itemConnected(void*,QString)));
    connect(pwgt,SIGNAL(disconnected(void *)),this,SLOT(itemDisconnect(void*)));
}

/*****************************************************************************/
void ComDispatcher::itemConnected(void* a_pitem,QString a_strName)
{
    this->renameTab(searchTabIndexByItemPtr((ComItem*)a_pitem),a_strName);
    m_pcontrol->setItemsList(m_lpitems);
}

/*****************************************************************************/
void ComDispatcher::itemDisconnect(void * a_pitem)
{
    this->renameTab(searchTabIndexByItemPtr((ComItem*)a_pitem),COMITEM_NAME_DEFAULT);
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
void ComDispatcher::on_actionAbout_triggered()
{
    m_strAbout.show();
}
/*****************************************************************************/
void ComDispatcher::on_actionQuit_triggered()
{
    exit(0);
}


