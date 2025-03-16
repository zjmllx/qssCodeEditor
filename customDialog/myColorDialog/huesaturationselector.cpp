#include "huesaturationselector.h"

#include <QPainterPath>
#include <QBrush>
#include <QGraphicsDropShadowEffect>

HueSaturationSelector::HueSaturationSelector(QWidget *parent) :
    QWidget(parent)
{
    resize(200, 200);
}

HueSaturationSelector::~HueSaturationSelector(){}

void HueSaturationSelector::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // // 创建带有圆角的路径
    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);

    // 绘制色相-饱和度渐变
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0, Qt::red);
    gradient.setColorAt(1.0 / 6, Qt::yellow);
    gradient.setColorAt(2.0 / 6, Qt::green);
    gradient.setColorAt(3.0 / 6, Qt::cyan);
    gradient.setColorAt(4.0 / 6, Qt::blue);
    gradient.setColorAt(5.0 / 6, Qt::magenta);
    gradient.setColorAt(1, Qt::red);
    painter.fillPath(path, gradient);

    // 绘制透明到白色的渐变
    QLinearGradient whiteToTransparentGradient(0, 0, 0, height());
    whiteToTransparentGradient.setColorAt(0, QColor(255, 255, 255, 0));
    whiteToTransparentGradient.setColorAt(1, QColor(255, 255, 255, 255));
    painter.fillPath(path, whiteToTransparentGradient);

    // 设置路径的边框颜色和线宽
    QPen pen(QColor(38,38,38), 1);  // 设置边框颜色为黑色，线宽为 2 像素
    painter.setPen(pen);
    painter.drawPath(path);  // 使用 drawPath 绘制带边框的路径

    // 绘制当前选择的小圈标识
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(selectedPos, 5, 5);
}

void HueSaturationSelector::mousePressEvent(QMouseEvent *event)
{
    updateColorFromPosition(event->pos());
}

void HueSaturationSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        updateColorFromPosition(event->pos());
    }
}

void HueSaturationSelector::resizeEvent(QResizeEvent *event)
{
    selectedPos = QPoint(this->width()/2, this->height()/2);
    updateColorFromPosition(selectedPos);
    return QWidget::resizeEvent(event);
}

void HueSaturationSelector::updateColorFromPosition(const QPoint &pos)
{
    // 限制位置在区域内
    int x = qBound(0, pos.x(), width()-1);
    int y = qBound(0, pos.y(), height()-1);
    selectedPos = QPoint(x, y);

    hue = static_cast<int>(360.0 * x / width()+0.5) % 360;
    saturation = static_cast<int>(255.0 * (height() - y) / height()) % 256;

    QColor color;
    color.setHsv(hue, saturation, 255);

    emit colorSelected(color);
    update();  // 更新显示
}

void HueSaturationSelector::updatePositionFromColor(const QColor &color)
{
    // 获取 hue 和 saturation 值
    int hue = color.hue();
    int saturation = color.saturation();

    // 计算小圈的位置
    selectedPos.setX(static_cast<int>((hue / 360.0) * width()));
    selectedPos.setY(static_cast<int>((1 - (saturation / 255.0)) * height()));

    update();  // 更新显示
}
