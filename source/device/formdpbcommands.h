#ifndef FORMDPBCOMMANDS_H
#define FORMDPBCOMMANDS_H

#include <QWidget>

namespace Ui {
class FormDpbCommands;
}

class FormDpbCommands : public QWidget
{
    Q_OBJECT

public:
    explicit FormDpbCommands(QWidget *parent = nullptr);
    ~FormDpbCommands();

private:
    Ui::FormDpbCommands *ui;
};

#endif // FORMDPBCOMMANDS_H
