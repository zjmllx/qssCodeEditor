#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    MyLineEdit(QWidget *parent = nullptr);

    // QWidget interface
protected:
    void wheelEvent(QWheelEvent *event) override;

signals:
    void valueChanged(QString newValue);  // 定义值变化的信号

};

#endif // MYLINEEDIT_H
