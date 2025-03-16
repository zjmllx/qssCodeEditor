#include "myvslider.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

MyVSlider::MyVSlider(QWidget *parent):
    QWidget(parent)
{
    this->setRange(0, 255);
    this->currentValue = maximum / 2 + 1;
    this->startColor = Qt::white; // 初始化渐变的起始颜色
}

void MyVSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int y = event->pos().y();
        int value = this->maximum - (y * this->maximum / this->height());
        this->setValue(value);
    }
    QWidget::mousePressEvent(event);  // 继续处理其他事件
}

void MyVSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int y = event->pos().y();
        int value = this->maximum - (y * this->maximum / this->height());

        if (value < minimum)
            value = minimum;
        else if (value > maximum)
            value = maximum;

        this->setValue(value);
    }
    QWidget::mouseMoveEvent(event);  // 继续处理其他事件
}

void MyVSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor endColor = Qt::black;

    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);

    // 设置从上到下的线性渐变
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, startColor);
    gradient.setColorAt(1, endColor);
    painter.fillPath(path, gradient);

    // 计算当前值的 y 坐标位置
    int yPos = height() - (currentValue * height() / maximum);

    // 设置圆环的大小
    int ringRadius = 8;
    int ringThickness = 4;

    // 绘制圆环
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::white, ringThickness));
    painter.drawEllipse(QPoint(width() / 2, yPos), ringRadius, ringRadius);

    QWidget::paintEvent(event); // 调用基类绘制方法
}

void MyVSlider::setRange(int min, int max)
{
    this->minimum = min;
    this->maximum = max;
    if (currentValue < min)
        currentValue = min;
    if (currentValue > max)
        currentValue = max;
    update();
}

void MyVSlider::setValue(int value)
{
    if (value < minimum)
        value = minimum;
    else if (value > maximum)
        value = maximum;

    if (currentValue != value) {
        this->currentValue = value;
        update();
        emit valueChanged(value);
    }
}

int MyVSlider::getValue()
{
    return currentValue;
}

void MyVSlider::changeStartColor(const QColor color)
{
    startColor = color;
    startColor.setHsv(color.hue(), color.saturation(), 255);
    update();
}

// 新增：根据外部传入值调整滑块位置
void MyVSlider::setValueFromExternal(int value)
{
    if (value < minimum || value > maximum) {
        qWarning("Value out of range!");
        return;
    }
    setValue(value);
}
