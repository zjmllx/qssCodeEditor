#include "mypushbutton.h"

CustomButton::CustomButton(QWidget *parent, QString name)
    : QPushButton(parent),
    isHovered(false)
{
    resize(40, 40); // 设置按钮大小
    color = QColor(255,255,255);
    setMouseTracking(true); // 启用鼠标跟踪
    index = 0;
    this->setText(name);
}

void CustomButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 绘制圆形边框
    QPen pen(color.darker(150));
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);

    // 设置虚线的自定义模式
    pen.setDashPattern({3,2}); // 虚线长度为6像素，间隙为5像素
    painter.setPen(pen);

    QRect rect = this->rect().adjusted(1, 1, -1, -1);
    painter.setBrush(QBrush(color));

    if (isHovered||isChecked()) {
        if(color == QColor(0,0,0)){
            pen.setColor(QColor(255,255,255));
        }else{
            pen.setColor(color.lighter(150));
        }
        painter.setPen(pen);
    }

    painter.drawEllipse(rect);
}


void CustomButton::enterEvent(QEnterEvent *event)
{
    if (isMouseInsideCircle(event->position().toPoint())) {
        isHovered = true;
        update(); // 更新样式
    }
    QPushButton::enterEvent(event);
}

void CustomButton::leaveEvent(QEvent *event)
{
    isHovered = false;
    update(); // 更新样式
    QPushButton::leaveEvent(event);
}

void CustomButton::mouseMoveEvent(QMouseEvent *event)
{
    bool inside = isMouseInsideCircle(event->pos());
    if (inside != isHovered) {
        isHovered = inside;
        update(); // 更新样式
    }
    QPushButton::mouseMoveEvent(event);
}

bool CustomButton::isMouseInsideCircle(const QPoint &pos) const {
    QPoint center(width() / 2, height() / 2);
    int radius = width() / 2; // 假设是正方形按钮，半径为宽度的一半
    return (pos - center).manhattanLength() <= radius;
}
