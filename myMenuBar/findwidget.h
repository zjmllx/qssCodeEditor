#ifndef FINDWIDGET_H
#define FINDWIDGET_H
#include<QLabel>
#include<QLineEdit>
#include<QPushButton>
#include<QLayout>
#include "../myEditor/codeeditor.h"
#include<QDialog>
#include<QCheckBox>

class FindWidget : public QDialog
{
    Q_OBJECT
public:
    explicit FindWidget(CodeEditor* edit,QDialog *parent = nullptr);
private:
    QCheckBox* bigIfBox;
    QLineEdit* findLineEdit;
    QLineEdit* repalceLineEdit;
    CodeEditor* edit;
    QVBoxLayout* mainLayout;

    bool isReplaced = false;

    int sum=0;
    int current=0;

    //高级功能窗口
    QWidget* advancedWidget=nullptr;
    bool isVisFlag=false;
    QLabel* countBtn;
    //
    QList<QTextCursor> highlightSelections; // 存储所有匹配的光标位置
    int currentMatchIndex = -1; // 记录当前查找到的索引
    //查找的选项
    QTextDocument::FindFlags options=QTextDocument::FindFlags();


private:
    bool isAdvancedWidgetOpen = false;  // 标记 advancedWidget 是否打开
    void onCursorPositionChanged();
    //画背景
    void drawBackground(const QTextCursor& cursor);
    void drawBackground();
private slots:
    void findText();
    void findNext();
    void findPrevious();
    void replaceText();
    void replaceAll();
    void advancedFunction();
    void onAdvancedWidgetClosed(bool visible);
    void hideAdvancedPanel();
private:
    void closeEvent(QCloseEvent *event)override;
signals:
    void ifBig(bool ifBigtabale);
};

#endif // FINDWIDGET_H
