#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QInputMethodEvent>
#include <QLabel>
#include <QTimer>

#include "linenumberarea.h"
#include "completer.h"

QT_BEGIN_NAMESPACE
class QCompleter;
QT_END_NAMESPACE

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    QColor textColor;
    QColor selectBackColor;
    QColor selectTextColor;
    QColor highlightColor;

    void setCompleter(MyCompleter *c);
    MyCompleter *completer() const;

    void setLineNumberSelectedColor(const QColor &color);
    void setLineNumberNoSelectedColor(const QColor &color);
    void setLineNumberBackgroundColor(const QColor &color);

    // 声明 getColorFromText 函数
    QColor getColorFromText(const QString &text) const;
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);

private:
    QString completionPrefix() const;

    QLabel *lab_color;
    QColor lineNumberSelectedColor;
    QColor lineNumberNoSelectedColor;
    QColor lineNumberBackgroundColor;

private:
    MyCompleter *c = nullptr;

public:
    // 构造函数，接受一个可选的父窗口指针
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    int textEditTop;                    // 编辑框的 top 间距
    int currentRowHeight;               // 当前行高
    int lineNumberAreaWidth();          // 计算并返回行号区域的宽度
    void setTabDistance();     // 设置tab样式

    // 绘制行号区域的事件处理函数
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    QByteArray openFile();
    void saveAs();

    void rightMenu(const QPoint&pos);
protected:
    // 重写调整窗口大小事件
    void resizeEvent(QResizeEvent *event) override;

    //重写滚轮事件
    void wheelEvent(QWheelEvent* event)override;

    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    // 更新行号区域的宽度
    void updateLineNumberAreaWidth(int rowCount);

    // 更新行号区域
    void updateLineNumberArea(const QRect &rect, int dy);

    // 高亮当前行
    void highlightCurrentLine();

private:
    LineNumberArea *m_lineNumberArea;
    QLabel *fontSizeLabel;
    QTimer *timer;

private:
    void initView();                 // 初始化
    int getRowHeight() const;        // 获取行高

    void updateCompleter();

    void showColorLabel(const QPoint &pos, const QColor &color);
    void hideColorLabel();
    void initColorConstant();
    QMap<QString, QColor> colorKeywords;  // 存储颜色名称与对应的 QColor
    QStringList colorNameList;//颜色名称列表
    QString colorStr;//颜色字符串组 "blue|pink|black"

    bool checkFileExtension(const QString &filePath);
    bool readFileContent(const QString &filePath, QString &content);
};
#endif
