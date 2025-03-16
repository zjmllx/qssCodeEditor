#ifndef USEGUIDE_H
#define USEGUIDE_H

#include <QDialog>

namespace Ui {
class UseGuide;
}

class UseGuide : public QDialog
{
    Q_OBJECT

public:
    explicit UseGuide(QWidget *parent = nullptr);
    ~UseGuide();

private:
    Ui::UseGuide *ui;
};

#endif // USEGUIDE_H
