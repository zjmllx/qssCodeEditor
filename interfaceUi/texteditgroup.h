#ifndef TEXTEDITGROUP_H
#define TEXTEDITGROUP_H

#include <QWidget>
#include <QPlainTextEdit>

namespace Ui {
class TextEditGroup;
}

class TextEditGroup : public QWidget
{
    Q_OBJECT

public:
    explicit TextEditGroup(QPlainTextEdit *editor = nullptr, QWidget *parent = nullptr);
    ~TextEditGroup();

public slots:
    void setStyleFromEditor();

private:
    QPlainTextEdit *editor;

private:
    Ui::TextEditGroup *ui;
};

#endif // TEXTEDITGROUP_H
