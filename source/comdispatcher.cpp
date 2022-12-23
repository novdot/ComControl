#include "comdispatcher.h"
#include "ui_comdispatcher.h"

#include "setups.h"

#include <QThread>
#include <QLibrary>

#define PROJECT_VER "2.0"

/*****************************************************************************/
ComDispatcher::ComDispatcher(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ComDispatcher)
{
    ui->setupUi(this);

    QString strProjName = "COMControl";
    QString strProjVersion = tr("%1 %2")
                                .arg(PROJECT_VER)
                                .arg(__TIMESTAMP__);
    QString strCompilerVer = "";
    QString gitFileName = "../.git/FETCH_HEAD";
    QString git_ver;

#if defined(__GNUC__)
    strCompilerVer = tr("GNU %1.%2.%3")
            .arg(__GNUC__ )
            .arg(__GNUC_MINOR__ )
            .arg(__GNUC_PATCHLEVEL__ );

#elif defined(_MSC_VER)
    strCompilerVer = tr("MSC %1")
            .arg(_MSC_VER )
#endif

    QFile file(gitFileName);

    if (!file.open(QIODevice::ReadOnly)){
        strProjVersion = QString("%1 %2")
                .arg(strProjVersion)
                .arg("<cant find git>")
                ;
    }else{
        git_ver = file.readLine();
        strProjVersion = QString("%1 %2")
                .arg(strProjVersion)
                .arg(git_ver)
                ;
    }

    this->setWindowTitle(
                tr("%1 %2")
                .arg(strProjName)
                .arg(strProjVersion)
                );
    qDebug()<<
               tr("%1 %2")
               .arg(strProjName)
               .arg(strProjVersion);

    ui->tabWidget->setTabsClosable(true);

    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int))
            ,this,SLOT(closeTab(int))
            );

    m_pcontrol = new ComControl();

    m_strAbout.setReadOnly(true);
    m_strAbout.appendHtml(tr("<h4>О проекте</h4>"));
    m_strAbout.appendHtml(tr("%1 %2")
                          .arg(strProjName)
                          .arg(strProjVersion));
    m_strAbout.appendHtml(tr(""
                             "<p>Проект разработан на С++ с использованием библиотеки Qt</p>"
                             "<p>Версия Qt: %1</p>"
                             "<p>Версия Compiler: %2</p>"
                             "<p>Дата сборки: %3</p>"
                             "<p>Версия проекта: %4</p>"
                             "<p>Проект на GitHub:<a href=\"https://github.com/novdot/comcontrol.git\">https://github.com/novdot/comcontrol.git</a></p>")
                          .arg(QT_VERSION_STR)
                          .arg(strCompilerVer )
                          .arg(__TIMESTAMP__ )
                          .arg(PROJECT_VER )
                          );
    m_strAbout.appendHtml(tr("Сайт с описанием проекта: <a href=\"https://http://idea2art.ru/content_item#78\">https://idea2art.ru/content_item#78</a>"));

    //SETUPS_DISPATCHER_CONSTRUCTOR();
    this->createTab();

    //read configuration file

    //load DLL libraries
    //DeviceControl load always
    QLibrary lib( "DeviceControl" );
    if( !lib.load() ) {
        qDebug() << "Loading failed!";
    }

}

ComDispatcher::~ComDispatcher()
{
    while(m_lpitems.count()!=0) this->closeTab(0);
    delete ui;
}

/*****************************************************************************/
void ComDispatcher::console(QList< QPair<QString,QString > > commands)
{
    m_lpitems.at(0)->console(commands);
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


