#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>
#include <QMouseEvent>

class CodeEditor;

class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    explicit LineNumberArea(QWidget* parent=nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent *event) override;

public:
    // 绘制行号区域的事件处理函数
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    // 计算并返回行号区域的宽度
    int updateLineNumberAreaWidth();

    int updateLineNumberArea();

private:
    CodeEditor* m_codeEditor;
};

#endif // LINENUMBERAREA_H
