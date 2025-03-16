#ifndef MYTOLBTN_H
#define MYTOLBTN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QImage>

class MyTolBtn : public QPushButton
{
    Q_OBJECT
public:
    explicit MyTolBtn(QWidget *parent = nullptr);

public:
    void setBtnText(QString text);
    void setBtnIcon(QString icon);

private:
    QPushButton* btn;

signals:
};

#endif // MYTOLBTN_H
