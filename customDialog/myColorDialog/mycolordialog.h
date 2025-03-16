#ifndef MYCOLORDIALOG_H
#define MYCOLORDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include <windows.h>

#include "huesaturationselector.h"
#include "myCustomControl/myCustomBtn/mypushbutton.h"
#include "myvslider.h"
#include "mylineedit.h"

namespace Ui {
class MyColorDialog;
}

class MyColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MyColorDialog(QWidget *parent = nullptr);
    ~MyColorDialog();

    static QColor getCurrentColor(QColor preColor = QColor(), QWidget *parent = nullptr);

private:
    HueSaturationSelector* hueSaSel;
    MyVSlider* myVSlider;
    QButtonGroup *m_buttonGroup;
    QVector<CustomButton*> m_buttons; // 按钮集合

    int colorValue;
    int colorHue;
    int colorSaturation;
    int colorAlpha;

    MyLineEdit* line_R;
    MyLineEdit* line_G;
    MyLineEdit* line_B;
    MyLineEdit* line_A;

    void setColorHue(int hue);
    void setColorSaturation(int saturation);

    void changeCurrentColor();
    void colorBlockchecked(CustomButton *btn);

    void RGBtoHSV(float r, float g, float b, float &h, float &s, float &v);
    void updataShowLabColor();
    void setShowText();

    QColor currentColor;

private slots:
    void setColorFromHSVSelector(const QColor &color);
    void updateBrightness(int brightness);

    void setRValue(QString red);
    void setGValue(QString green);
    void setBValue(QString blue);
    void setAValue(QString alpha);

signals:
    void onBaseColorChanged(QColor baseColor);
    void onCurrentColorChanged(QColor currentColor);

private:
    Ui::MyColorDialog *ui;
};

#endif // MYCOLORDIALOG_H
