#ifndef HUESATURATIONSELECTOR_H
#define HUESATURATIONSELECTOR_H

#include <QWidget>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>

class HueSaturationSelector : public QWidget
{
    Q_OBJECT

public:
    explicit HueSaturationSelector(QWidget *parent = nullptr);
    ~HueSaturationSelector();

    void updateColorFromPosition(const QPoint &pos);
    void updatePositionFromColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    int hue;
    int saturation;
    int value;
    QPoint selectedPos;

signals:
    void colorSelected(const QColor &color);

};
#endif // HUESATURATIONSELECTOR_H

