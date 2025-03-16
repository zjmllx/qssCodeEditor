#ifndef MYVSLIDER_H
#define MYVSLIDER_H

#include <QWidget>

class MyVSlider : public QWidget
{
    Q_OBJECT
public:
    MyVSlider(QWidget *parent = nullptr);

    int getValue();

    void setValueFromExternal(int value);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

private:
    int minimum, maximum, currentValue;
    QColor startColor;

private:
    void setRange(int min = 0, int max = 100);
    void setValue(int value);

signals:
    void valueChanged(int value);

public slots:
    void changeStartColor(const QColor color);
};

#endif // MYVSLIDER_H
