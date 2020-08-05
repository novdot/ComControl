#include "comcontrol.h"
#include "ui_formcomcontrol.h"

#include "comdevicebase.h"

#include <QFileDialog>

ComControl::ComControl(QWidget *parent)
    : QWidget(parent)
    , m_pui(new Ui::FormComControl)
{
    m_pui->setupUi(this);
    this->setWindowIcon(QIcon(":icon/settings.png"));
    this->setWindowTitle(tr("Setting","Window title"));

    m_lFormat = COMDeviceBase::getFormatList();
    QList< QPair<int,QString> >::ConstIterator end;
    end = m_lFormat.constEnd();
    for(QList< QPair<int,QString> >::ConstIterator i = m_lFormat.constBegin(); i != end; ++i){
        m_pui->comboBox_master_format->addItem((*i).second,(*i).second);
        m_pui->comboBox_slave_format->addItem((*i).second,(*i).second);
    }

    connect(m_pui->pushButton_deny, SIGNAL(clicked())
            , this, SLOT(deny()) );
    connect(m_pui->pushButton_accept, SIGNAL(clicked())
            , this, SLOT(accept()) );

    connect(m_pui->pushButton_link_add, SIGNAL(clicked())
            , this, SLOT(addLink()) );
    connect(m_pui->pushButton_link_change, SIGNAL(clicked())
            , this, SLOT(changeLink()) );
    connect(m_pui->pushButton_link_remove, SIGNAL(clicked())
            , this, SLOT(removeLink()) );

    connect(m_pui->pushButton_save, SIGNAL(clicked())
            , this, SLOT(save()) );
    connect(m_pui->pushButton_open, SIGNAL(clicked())
            , this, SLOT(open()) );

    connect(m_pui->listWidget_robot_list ,SIGNAL(itemClicked(QListWidgetItem *))
                , this, SLOT(chooseRobotLine(QListWidgetItem *)));

}
/*****************************************************************************/
ComControl::~ComControl()
{
    delete m_pui;
}

/*****************************************************************************/
void ComControl::chooseRobotLine(QListWidgetItem * a_pItem)
{
    QString choosenText = a_pItem->text();
    //снимем подсветку со списка
    for(int i=0;i<m_pui->listWidget_robot_list->count();i++) {
        (m_pui->listWidget_robot_list->item(i))->setBackgroundColor(Qt::white);
    }
    //подсветим выбранную комманду
    a_pItem->setBackgroundColor(Qt::yellow);

    //найдем выбранную строку в списке
    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobot.constBegin();
    end = m_lRobot.constEnd();
    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        if((*i).name == choosenText) {
            //заполнем поля из  структуры
            m_pui->lineEdit_link_info_name->setText((*i).name);
            //MASTER
            if((*i).master.device==nullptr) {
                m_pui->comboBox_master_comitem->setCurrentIndex(
                            m_pui->comboBox_master_comitem->findData(NULL_COMITEM_PTR));
            } else {
                QList<ComItem*>::ConstIterator begin;
                QList<ComItem*>::ConstIterator end;
                begin = m_lItems.constBegin();
                end = m_lItems.constEnd();
                for(QList<ComItem*>::ConstIterator j = begin; j != end; ++j){
                    if((*i).master.device==(*j)->getDevice()){
                        m_pui->comboBox_master_comitem->setCurrentIndex(
                                    m_pui->comboBox_master_comitem->findData(
                                        (*j)->getItemName()
                                        ));
                    }
                }
            }
            m_pui->comboBox_master_format->setCurrentIndex((*i).master.format);
            if((*i).master.value==COM_ROBOT_VALUE_DEFAULT) {
                m_pui->lineEdit_master_value->setText("");
            } else {
                m_pui->lineEdit_master_value->setText((*i).master.value);
            }
            //SLAVE
            if((*i).slave.device==nullptr) {
                m_pui->comboBox_slave_comitem->setCurrentIndex(
                            m_pui->comboBox_slave_comitem->findData(NULL_COMITEM_PTR));
            } else {
                QList<ComItem*>::ConstIterator begin;
                QList<ComItem*>::ConstIterator end;
                begin = m_lItems.constBegin();
                end = m_lItems.constEnd();
                for(QList<ComItem*>::ConstIterator j = begin; j != end; ++j){
                    if((*i).slave.device==(*j)->getDevice()){
                        m_pui->comboBox_slave_comitem->setCurrentIndex(
                                    m_pui->comboBox_slave_comitem->findData(
                                        (*j)->getItemName()
                                        ));
                    }
                }
            }
            m_pui->comboBox_slave_format->setCurrentIndex((*i).slave.format);
            if((*i).slave.value==COM_ROBOT_VALUE_DEFAULT) {
                m_pui->lineEdit_slave_value->setText("");
            } else {
                m_pui->lineEdit_slave_value->setText((*i).slave.value);
            }
            return;
        }
    }
}

/*****************************************************************************/
void ComControl::setItemsList(QList<ComItem*> a_items)
{
    //сохраним список итемов
    m_lItems = a_items;

    //пройдемся по списку - заполним поля настроек
    QList<ComItem*>::ConstIterator begin;
    QList<ComItem*>::ConstIterator end;
    begin = a_items.constBegin();
    end = a_items.constEnd();

    m_pui->comboBox_master_comitem->clear();
    m_pui->comboBox_slave_comitem->clear();

    m_pui->comboBox_master_comitem->addItem(NULL_COMITEM_PTR,NULL_COMITEM_PTR);
    m_pui->comboBox_slave_comitem->addItem(NULL_COMITEM_PTR,NULL_COMITEM_PTR);

    for(QList<ComItem*>::ConstIterator i = begin; i != end; ++i){
        if( !(*i)->isConnected() ) continue;
        m_pui->comboBox_master_comitem->addItem((*i)->getItemName(),(*i)->getItemName());
        m_pui->comboBox_slave_comitem->addItem((*i)->getItemName(),(*i)->getItemName());
    }
}
/*****************************************************************************/
void ComControl::deny()
{
    this->hide();
}
/*****************************************************************************/
void ComControl::accept()
{
    //сообщим внешнему миру о новых настройках
    emit setupRobot(m_lRobot);

    //пройдемся по подключенным устройствам и закинем им новые настройки
    QList<ComItem*>::ConstIterator begin;
    QList<ComItem*>::ConstIterator end;
    begin = m_lItems.constBegin();
    end = m_lItems.constEnd();
    for(QList<ComItem*>::ConstIterator i = begin; i != end; ++i){
        ((COMDeviceBase*)(*i)->getDevice())->setRobotList(m_lRobot);
    }

    this->hide();
}
/*****************************************************************************/
com_robot ComControl::completeLinkFromFields()
{
    com_robot robotLine;
    QString temp = "";
    robotLine.name = m_pui->lineEdit_link_info_name->text();
    //MASTER
    temp = m_pui->comboBox_master_comitem->currentText();
    if(temp == "null")
        robotLine.master.device = nullptr;
    else{
        QList<ComItem*>::ConstIterator begin;
        QList<ComItem*>::ConstIterator end;
        begin = m_lItems.constBegin();
        end = m_lItems.constEnd();
        for(QList<ComItem*>::ConstIterator i = begin; i != end; ++i){
            if((*i)->getItemName()==temp)
                robotLine.master.device = (*i)->getDevice();
        }
    }
    robotLine.master.format = (device_format)m_pui->comboBox_master_format->currentIndex();
    temp = m_pui->lineEdit_master_value->text();
    if(temp=="")
        robotLine.master.value = COM_ROBOT_VALUE_DEFAULT;
    else
        robotLine.master.value = temp;

    //SLAVE
    temp = m_pui->comboBox_slave_comitem->currentText();
    if(temp == "null")
        robotLine.slave.device = nullptr;
    else{
        QList<ComItem*>::ConstIterator begin;
        QList<ComItem*>::ConstIterator end;
        begin = m_lItems.constBegin();
        end = m_lItems.constEnd();
        for(QList<ComItem*>::ConstIterator i = begin; i != end; ++i){
            if((*i)->getItemName()==temp)
                robotLine.slave.device = (*i)->getDevice();
        }
    }
    robotLine.slave.format = (device_format)m_pui->comboBox_slave_format->currentIndex();
    temp = m_pui->lineEdit_slave_value->text();
    if(temp=="")
        robotLine.slave.value = COM_ROBOT_VALUE_DEFAULT;
    else
        robotLine.slave.value = temp;

    return robotLine;
}
/*****************************************************************************/
void ComControl::updateRobotList()
{
    m_pui->listWidget_robot_list->clear();
    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobot.constBegin();
    end = m_lRobot.constEnd();
    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        m_pui->listWidget_robot_list->addItem((*i).name);
    }
}
/*****************************************************************************/
void ComControl::addLink()
{
    //прочитаем поля
    com_robot robotLine = completeLinkFromFields();

    //проверим что название не дублируется
    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobot.constBegin();
    end = m_lRobot.constEnd();
    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        if((*i).name == robotLine.name){
            m_box.setText("В списке уже есть настройка с таким названием! Измените название на уникальное.");
            m_box.show();
            return;
        }
    }

    //добавим поле
    m_lRobot.append(robotLine);
    this->updateRobotList();

}
/*****************************************************************************/
void ComControl::changeLink()
{
    QString strChoosedLine;
    int nInd = 0;
    //проверим что поле выбрано
    bool isFinded = false;
    for(int i=0;i<m_pui->listWidget_robot_list->count();i++) {
        if( (m_pui->listWidget_robot_list->item(i))->backgroundColor() == Qt::yellow) {
            strChoosedLine = m_pui->listWidget_robot_list->item(i)->text();
            isFinded = true;
            nInd = i;
            break;
        }
    }
    if(!isFinded) {
        m_box.setText("Нет выбранного элемента из списка, либо список пуст! Сначала добавьте элемнты в список и выбирите тот, что необходимо поменять.");
        m_box.show();
        return;
    }

    //проверим что название не дублируется (исключая выбранное поле)
    com_robot robotLine = completeLinkFromFields();
    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobot.constBegin();
    end = m_lRobot.constEnd();
    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        if( ((*i).name == robotLine.name) && (strChoosedLine != robotLine.name) ){
            m_box.setText("В списке уже есть настройка с таким названием и данное поле не выбрано! Измените название на уникальное.");
            m_box.exec();
            return;
        }
    }

    //перезапишем текущее поле
    m_lRobot.removeAt(nInd);
    m_lRobot.insert(nInd,robotLine);
    this->updateRobotList();
}
/*****************************************************************************/
void ComControl::removeLink()
{
    //проверим что поле выбрано
    int nInd = 0;
    bool isFinded = false;
    for(int i=0;i<m_pui->listWidget_robot_list->count();i++) {
        if( (m_pui->listWidget_robot_list->item(i))->backgroundColor() == Qt::yellow) {
            isFinded = true;
            nInd = i;
            break;
        }
    }
    if(!isFinded) {
        m_box.setText("Нет выбранного элемента из списка, либо список пуст! Сначала добавьте элемнты в список и выбирите тот, что необходимо поменять.");
        m_box.exec();
        return;
    }

    //удалим поле
    m_lRobot.removeAt(nInd);
    this->updateRobotList();
}
/*****************************************************************************/
void ComControl::save()
{
    //сохраним в файл
    QString strFile = QFileDialog::getSaveFileName(
                        this,
                        tr("Сохранить файл робота"),
                        QDir::current().absolutePath(),
                        "Файлы робота (*.robot)"
                        );
    QFile file(strFile);
    if(!file.open(QIODevice::WriteOnly)) {
        m_box.setText(QString("Cant open file (%1)!").arg(strFile));
        m_box.exec();
        return;
    }
    //соберем все строки робота в строку
    QString strData;

    QList<com_robot>::ConstIterator begin;
    QList<com_robot>::ConstIterator end;
    begin = m_lRobot.constBegin();
    end = m_lRobot.constEnd();
    for(QList<com_robot>::ConstIterator i = begin; i != end; ++i){
        strData.append(QString("%1%2%3").arg(__ROBOT_PARSER_FIELD_LINKNAME).arg(__ROBOT_PARSER_VALUE).arg((*i).name));
        strData.append(__ROBOT_PARSER_FIELD);
        strData.append(QString("%1%2%3").arg(__ROBOT_PARSER_FIELD_MASTER_FORM).arg(__ROBOT_PARSER_VALUE).arg((*i).master.format));
        strData.append(__ROBOT_PARSER_FIELD);
        strData.append(QString("%1%2%3").arg(__ROBOT_PARSER_FIELD_MASTER_VAL).arg(__ROBOT_PARSER_VALUE).arg((*i).master.value));
        strData.append(__ROBOT_PARSER_FIELD);
        strData.append(QString("%1%2%3").arg(__ROBOT_PARSER_FIELD_SLAVE_FORM).arg(__ROBOT_PARSER_VALUE).arg((*i).slave.format));
        strData.append(__ROBOT_PARSER_FIELD);
        strData.append(QString("%1%2%3").arg(__ROBOT_PARSER_FIELD_SLAVE_VAL).arg(__ROBOT_PARSER_VALUE).arg((*i).slave.value));
        strData.append(__ROBOT_PARSER_FIELD_END);
    }

    //запишем строку в файл
    file.write(strData.toUtf8());
}
/*****************************************************************************/
void ComControl::open()
{
    com_robot lineRobot;
    QStringList lstLine;
    QStringList lstField;
    QStringList lstVal;
    //откроем файл
    QString strFile = QFileDialog::getOpenFileName(
                        this,
                        tr("Выбрать файл робота"),
                        QDir::current().absolutePath(),
                        "Файлы робота (*.robot)"
                        );
    QFile file(strFile);
    if(!file.open(QIODevice::ReadOnly)) {
        m_box.setText(QString("Cant open file (%1)!").arg(strFile));
        m_box.exec();
        return;
    }

    m_lRobot.clear();

    //прочтем все в сттроку
    QString strData = file.readAll();

    //распарсим строку и соберем список линков робота
    //определим линии
    lstLine = strData.split(__ROBOT_PARSER_FIELD_END);
    QStringList::ConstIterator end_line;
    QStringList::ConstIterator begin_line;
    end_line = lstLine.constEnd();
    begin_line = lstLine.constBegin();
    for(QStringList::ConstIterator i_line = begin_line; i_line != end_line; ++i_line){
        //разобьем на поля
        lstField = (*i_line).split(__ROBOT_PARSER_FIELD);

        //для каждой линии будем чистить структуру
        lineRobot.name = "";
        lineRobot.master.format = (device_format)0;
        lineRobot.master.value = "";
        lineRobot.slave.format = (device_format)0;
        lineRobot.slave.value = "";

        //в полях будем вычленять пары маркер-значение
        QStringList::ConstIterator end;
        QStringList::ConstIterator begin;
        end = lstField.constEnd();
        begin = lstField.constBegin();
        for(QStringList::ConstIterator i = begin; i != end; ++i){
            lstVal = (*i).split(__ROBOT_PARSER_VALUE);
            if(lstVal[0]==__ROBOT_PARSER_FIELD_LINKNAME) lineRobot.name = lstVal[1];
            if(lstVal[0]==__ROBOT_PARSER_FIELD_MASTER_FORM) lineRobot.master.format = (device_format)(lstVal[1]).toInt();
            if(lstVal[0]==__ROBOT_PARSER_FIELD_MASTER_VAL) lineRobot.master.value = lstVal[1];
            if(lstVal[0]==__ROBOT_PARSER_FIELD_SLAVE_FORM) lineRobot.slave.format = (device_format)(lstVal[1]).toInt();
            if(lstVal[0]==__ROBOT_PARSER_FIELD_SLAVE_VAL){
                lineRobot.slave.value = lstVal[1];
                m_lRobot.append(lineRobot);
            }
        }
    }
    this->updateRobotList();
}
/*****************************************************************************/
