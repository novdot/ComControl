#include "formatcommands.h"
#include "ui_formatcommands.h"

FormAtCommands::FormAtCommands(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAtCommands)
{
    ui->setupUi(this);
}

FormAtCommands::~FormAtCommands()
{
    delete ui;
}
