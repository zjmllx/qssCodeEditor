#ifndef VIEWGROUP_H
#define VIEWGROUP_H

#include <QWidget>
#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class ViewGroup; }
QT_END_NAMESPACE

// 视图界面
class ViewGroup : public QWidget
{
    Q_OBJECT

public:
    ViewGroup(QPlainTextEdit *editor = nullptr, QWidget *parent = nullptr);
    ~ViewGroup();

public slots:
    void setStyleFromEditor();

private:
    QPlainTextEdit *editor;

private:
    Ui::ViewGroup *ui_viewGroup;
};

#endif // VIEWGROUP_H
