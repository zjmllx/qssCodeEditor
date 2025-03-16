#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QMenu>
#include <QPushButton>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QCursor>

class CustomButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CustomButton(QWidget *parent = nullptr, QString name = "");
    QColor color;
    int index;

protected:
    bool isHovered; // 标记鼠标是否在圆内

    // 重写鼠标进入和离开事件
    void enterEvent(QEnterEvent *event) override;

    void leaveEvent(QEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
private:
    // 判断鼠标是否在圆形区域内
    bool isMouseInsideCircle(const QPoint &pos) const;

};

#endif // MYPUSHBUTTON_H
