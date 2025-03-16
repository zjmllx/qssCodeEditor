#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QDebug>

#include "linenumberarea.h"
#include "codeeditor.h"

LineNumberArea::LineNumberArea(QWidget *parent)
    : QWidget(parent)
{
    m_codeEditor = static_cast<CodeEditor*>(parent);
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
    m_codeEditor->lineNumberAreaPaintEvent(event); // 调用 CodeEditor 的绘制行号区域函数
    return QWidget::paintEvent(event);
}

void LineNumberArea::mousePressEvent(QMouseEvent *event)
{
    if(m_codeEditor->verticalScrollBar()->value() <= 0)
    {
        int row = (event->pos().y() - m_codeEditor->textEditTop)
                  / m_codeEditor->currentRowHeight;

        QTextCursor cursor = m_codeEditor->textCursor();
        int targetLine = row;  // 要跳转到的行号，从 0 开始

        // 获取文档中的第 targetLine 行的 QTextBlock
        QTextBlock block = m_codeEditor->document()->findBlockByLineNumber(targetLine);

        if (block.isValid()) {
            // 将光标移动到该 QTextBlock 的开始
            cursor.setPosition(block.position());
            m_codeEditor->setTextCursor(cursor);  // 设置光标
        }
    }
    else
    {
        int row = (event->pos().y()) /
                      m_codeEditor->currentRowHeight + m_codeEditor->verticalScrollBar()->value();
        QTextCursor cursor = m_codeEditor->textCursor();
        int targetLine = row;  // 要跳转到的行号，从 0 开始

        // 获取文档中的第 targetLine 行的 QTextBlock
        QTextBlock block = m_codeEditor->document()->findBlockByLineNumber(targetLine);

        if (block.isValid()) {
            // 将光标移动到该 QTextBlock 的开始
            cursor.setPosition(block.position());
            m_codeEditor->setTextCursor(cursor);  // 设置光标
        }
    }
}
