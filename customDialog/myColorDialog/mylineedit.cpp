#include "mylineedit.h"

#include <QWheelEvent>
#include <QValidator>

MyLineEdit::MyLineEdit(QWidget *parent):
    QLineEdit(parent)
{
    this->setValidator(new QIntValidator(0, 255, this));
    this->setStyleSheet("background-color: rgb(47, 47, 47);"
                        "border-radius: 5px;"
                        "color: rgb(255, 255, 255);");
}

void MyLineEdit::wheelEvent(QWheelEvent *event)
{
    int currentValue = text().toInt();  // 获取当前值
    if (event->angleDelta().y() > 0) {
        // 向上滚动，增加值
        currentValue = qMin(currentValue + 1, 255);
    } else {
        // 向下滚动，减少值
        currentValue = qMax(currentValue - 1, 0);
    }

    this->setText(QString::number(currentValue));  // 更新显示的值

    // 发送一个信号通知值的变化
    emit valueChanged(QString::number(currentValue));

    event->accept();
}
