#include "mytolbtn.h"
#include <QPixmap>

MyTolBtn::MyTolBtn(QWidget *parent) :
    QPushButton{parent}
{
    this->resize(83, 30);

    QFont font;
    font.setFamily("微软雅黑");
    font.setBold(true);
    font.setPixelSize(14);

    btn = new QPushButton("按钮", this);
    btn->setIconSize(QSize(16,16));
    // 设置按钮的样式表，以增加图标和文字之间的间距
    btn->setFont(font);

    btn->setStyleSheet(
        "QPushButton {"
        "   border: none;" // 边框样式
        "   padding: 5px;"        // 控制图标和文字的间距
        "   background-color: #dcdcdc;" // 背景色
        "   color: black;"              // 文字颜色
        "}"
        "QPushButton:hover {"
        "   background-color: #c0c0c0;" // 鼠标悬浮时背景色
        "}"
        "QPushButton:pressed {"
        "   background-color: red;" // 按下时背景色#a0a0a0
        "}"
    );
}
