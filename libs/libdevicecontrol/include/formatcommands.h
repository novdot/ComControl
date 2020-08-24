#ifndef FORMATCOMMANDS_H
#define FORMATCOMMANDS_H

#include <QWidget>

namespace Ui {
class FormAtCommands;
}

class FormAtCommands : public QWidget
{
    Q_OBJECT

public:
    explicit FormAtCommands(QWidget *parent = nullptr);
    ~FormAtCommands();

private:
    Ui::FormAtCommands *ui;
};

#endif // FORMATCOMMANDS_H
