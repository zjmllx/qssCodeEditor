#ifndef INPUTGROUP_H
#define INPUTGROUP_H

#include <QWidget>
#include <QPlainTextEdit>

namespace Ui {
class InputGroup;
}

class InputGroup : public QWidget
{
    Q_OBJECT

public:
    explicit InputGroup(QPlainTextEdit *editor = nullptr, QWidget *parent = nullptr);
    ~InputGroup();

public slots:
    void setStyleFromEditor();

private:
    QPlainTextEdit *editor;

private:
    Ui::InputGroup *ui;
};

#endif // INPUTGROUP_H
