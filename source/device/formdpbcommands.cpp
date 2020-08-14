#include "formdpbcommands.h"
#include "ui_formdpbcommands.h"

FormDpbCommands::FormDpbCommands(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDpbCommands)
{
    ui->setupUi(this);
}

FormDpbCommands::~FormDpbCommands()
{
    delete ui;
}
