#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QToolButton>
#include <QSplitter>
#include <QStackedWidget>

#include "../myEditor/codeeditor.h"
#include "../myMenuBar/topbar.h"

#include "../myEditor/completer.h"
#include "buttongroup.h"
#include "inputgroup.h"
#include "texteditgroup.h"
#include "viewgroup.h"

// #include "./interfaceUi/viewgroup.h"

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QCompleter;
class CodeEditor;
class Highlighter;
class FindWidget;
class Config;
QT_END_NAMESPACE

// 主窗口界面
class App : public QWidget
{
    Q_OBJECT

public:
    App(QWidget *parent = nullptr);
    ~App();

    int tolBarH;

private:
    TopBar *tolBar;
    QFrame *fra_text;
    QFrame *fra_control;
    QSplitter *splitter;
    QStackedWidget *stackedWidget;

    ButtonGroup *btnGroup;
    InputGroup *inputGroup;
    TextEditGroup *textEditGroup;
    ViewGroup *viewGroup;

    MyCompleter *completer = nullptr;
    CodeEditor *editor;
    FindWidget *findWidget;
    Highlighter *highlighter;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;

    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
