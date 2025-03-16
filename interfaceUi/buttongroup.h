#ifndef BUTTONGROUP
#define BUTTONGROUP

#include <QWidget>
#include <QPlainTextEdit>
#include <QStyle>
QT_BEGIN_NAMESPACE
namespace Ui { class ButtonGroup; }
QT_END_NAMESPACE

// 按钮组界面
class ButtonGroup : public QWidget
{
    Q_OBJECT

public:
    ButtonGroup(QPlainTextEdit *editor = nullptr, QWidget *parent = nullptr);
    ~ButtonGroup();

private:
    QPlainTextEdit *editor;

public slots:
    void setStyleFromEditor();

public:
    Ui::ButtonGroup *ui_buttonGroup;
};

#endif
