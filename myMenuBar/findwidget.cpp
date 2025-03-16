#include "findwidget.h"
#include<QPlainTextEdit>
#include<QTextBlock>
#include<QTextDocument>
#include<windows.h>
#include<QScrollBar>
#include<QStackedWidget>
FindWidget::FindWidget(CodeEditor* edit,QDialog *parent)
    : QDialog{parent},edit(edit)
{
    try {
        setWindowTitle("查找替换");
        mainLayout = new QVBoxLayout(this);

        this->setStyleSheet(R"(/* 全局样式 */
                                * {
                                    background-color: #2E2E2E; /* 墨玉黑背景 */
                                    color: #FFFFFF; /* 白色文字 */
                                    font-family: Arial, sans-serif;
                                    font-size: 8pt;
                                }

                                /* 按钮样式 */
                                QPushButton {
                                    background-color: #3E3E3E; /* 深灰色背景 */
                                    border: 1px solid #5E5E5E; /* 边框颜色 */
                                    border-radius: 3px; /* 圆角 */
                                    padding: 5px 10px; /* 内边距 */
                                }

                                QPushButton:hover {
                                    background-color: #4E4E4E; /* 鼠标悬停时的背景色 */
                                }

                                QPushButton:pressed {
                                    background-color: #5E5E5E; /* 按下时的背景色 */
                                }

                                /* 编辑框样式 */
                                QLineEdit {
                                    background-color: #3E3E3E; /* 深灰色背景 */
                                    border: 1px solid #5E5E5E; /* 边框颜色 */
                                    border-radius: 3px; /* 圆角 */
                                    padding: 2px; /* 内边距 */
                                    color: #FFFFFF; /* 白色文字 */
                                }

                                /* 标签样式 */
                                QLabel {
                                    color: #FFFFFF; /* 白色文字 */
                                    padding: 2px; /* 内边距 */
                                })");

        QHBoxLayout* hLayout_find=new QHBoxLayout;

        hLayout_find->addWidget(new QLabel("查   找："));

        findLineEdit=new QLineEdit;
        hLayout_find->addWidget(findLineEdit);
        connect(findLineEdit,&QLineEdit::textChanged,this, [=](){
            //当搜素文本变换时
            findText();
            if(highlightSelections.isEmpty()){
                current = 0;
                sum = 0;
                countBtn->setText(QString("%1 / %2").arg(current).arg(sum));
            }else {
                currentMatchIndex = 0;
                drawBackground(highlightSelections[0]);
                countBtn->setText(QString("%1 / %2").arg(currentMatchIndex+1).arg(highlightSelections.size()));
            }
        });

        QPushButton* findPreBtn=new QPushButton("查找上一个");
        connect(findPreBtn,&QPushButton::clicked,this,&FindWidget::findPrevious);
        QPushButton* findNextBtn=new QPushButton("查找下一个");
        connect(findNextBtn,&QPushButton::clicked,this,&FindWidget::findNext);
        hLayout_find->addWidget(findPreBtn);
        hLayout_find->addWidget(findNextBtn);

        countBtn = new QLabel(QString("%1 / %2").arg(current).arg(sum));
        hLayout_find->addWidget(countBtn);

        QPushButton* cancelBtn=new QPushButton("X");

        hLayout_find->addWidget(cancelBtn);
        connect(cancelBtn,&QPushButton::clicked,this,&FindWidget::close);

        QHBoxLayout* hLayout_replace=new QHBoxLayout;
        hLayout_replace->addWidget(new QLabel("替换为："));
        repalceLineEdit=new QLineEdit;
        hLayout_replace->addWidget(repalceLineEdit);
        QPushButton* replaceBtn=new QPushButton("替换");
        connect(replaceBtn,&QPushButton::clicked,this,&FindWidget::replaceText);
        QPushButton* replaceAllBtn=new QPushButton("替换所有");
        connect(replaceAllBtn,&QPushButton::clicked,this,&FindWidget::replaceAll);
        QPushButton* advancedBtn=new QPushButton("高级...");
        connect(advancedBtn,&QPushButton::clicked,this,&FindWidget::advancedFunction);

        advancedBtn->setFixedSize(70, 25);

        hLayout_replace->addWidget(replaceBtn);
        hLayout_replace->addWidget(replaceAllBtn);
        hLayout_replace->addWidget(advancedBtn);

        //当文本内容改变的时候
        connect(edit,&QPlainTextEdit::blockCountChanged, this, [=]
                {
                    if(!highlightSelections.isEmpty())
                    {
                        edit->setExtraSelections({}); // 清除所有高亮
                        highlightSelections.clear();
                        currentMatchIndex = -1; // 重置查找索引
                    }
                });

        mainLayout->addLayout(hLayout_find);
        mainLayout->addLayout(hLayout_replace);

        if (advancedWidget) {
            advancedWidget->installEventFilter(this);
        }

        // 监听 plainTextEdit 的 cursorPositionChanged 信号
        connect(edit, &QPlainTextEdit::cursorPositionChanged, this, &FindWidget::onCursorPositionChanged);


    } catch (...) {
        qDebug()<<"出现错误";
    }

}

void FindWidget::onCursorPositionChanged()
{
    //当点击plaintTextEdit时如果自己是打开状态,就不关闭高亮
    if (isVisible()) {
        qDebug()<<"当前下标改变";
        //findText(); // 重新执行 findText()，恢复匹配颜色
        drawBackground();
        return; // 不恢复匹配项背景
    }
}

/*
功能:画查找到的目标字符串背景
target:需要画为蓝色背景的字符转的鼠标位置

*/
void FindWidget::drawBackground(const QTextCursor& target)
{
    try {
        QList<QTextEdit::ExtraSelection> extraSelections;
        for(auto& item:highlightSelections)
        {
            // 设置黄色高亮
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(QColor(145, 181, 117));
            selection.cursor = item;
            extraSelections.append(selection);
        }
        if(!target.isNull())
        {
            // 设置当前匹配项为蓝色
            QTextEdit::ExtraSelection currentSelection;
            currentSelection.format.setBackground(QColor(51, 216, 224));
            currentSelection.cursor = target;
            extraSelections.append(currentSelection);
        }
        // 应用高亮
        edit->setExtraSelections(extraSelections);
    } catch (...) {
        qDebug()<<__FUNCTION__<<"出错";
    }

}

void FindWidget::drawBackground()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    for(auto& item:highlightSelections)
    {
        // 设置黄色高亮
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor(120, 150, 95));
        selection.cursor = item;
        extraSelections.append(selection);
    }
    edit->setExtraSelections(extraSelections);
}

void FindWidget::findText()
{
    try {
        //获取查找字符串
        QString target = findLineEdit->text();

        //如果要查找的字符串为空
        if (target.isEmpty()) {
            edit->setExtraSelections({}); // 清除所有高亮
            highlightSelections.clear();
            return;
        }
        //获取编辑器的QTextDocument用于处理查找
        QTextDocument* document = edit->document();
        QTextCursor cursor(document);
        cursor.movePosition(QTextCursor::Start); // 从文档开头开始查找
        highlightSelections.clear(); // 清空上次的匹配
        currentMatchIndex = -1; // 重置查找索引

        while (true) {
            cursor = document->find(target, cursor,options);
            if (cursor.isNull()) break;
            highlightSelections.append(cursor); // 记录匹配项光标
        }
    } catch (...) {
        qDebug()<<__FUNCTION__<<"出错";
    }
}

void FindWidget::findNext()
{
    try {
        if (highlightSelections.isEmpty())
            return;

        // 计算下一个匹配项索引（循环）
        currentMatchIndex = (currentMatchIndex + 1) % highlightSelections.size();

        qDebug()<<"当前位置:"<<currentMatchIndex+1;
        QTextCursor cursor = highlightSelections[currentMatchIndex];

        drawBackground(cursor);

        // 移动光标到当前匹配项
        // **计算滚动条位置**
        QTextBlock block = cursor.block();
        int blockNumber = block.blockNumber(); // 获取当前匹配项的行号
        QScrollBar *scrollBar = edit->verticalScrollBar();
        scrollBar->setValue(blockNumber); // 滚动到对应行

        //更新显示当前位置Label
        int index = currentMatchIndex;
        current=index+1;
        sum=highlightSelections.size();
        countBtn->setText(QString("%1 / %2").arg(current).arg(sum));
    } catch (...) {
         qDebug()<<__FUNCTION__<<"出错";
    }
}

void FindWidget::findPrevious()
{

    if (highlightSelections.isEmpty()) return;
    // 计算上一个匹配项索引（循环查找）
    currentMatchIndex = (currentMatchIndex - 1 + highlightSelections.size()) % highlightSelections.size();

    qDebug()<<"当前位置:"<<currentMatchIndex+1;
    QTextCursor cursor = highlightSelections[currentMatchIndex];
    drawBackground(cursor);

    // 获取滑动条
    // **计算滚动条位置**
    QTextBlock block = cursor.block();
    int blockNumber = block.blockNumber(); // 获取当前匹配项的行号
    QScrollBar *scrollBar = edit->verticalScrollBar();
    scrollBar->setValue(blockNumber); // 滚动到对应行

    //更新显示当前位置Label
    int index = currentMatchIndex;
    current=index+1;
    sum=highlightSelections.size();
    countBtn->setText(QString("%1 / %2").arg(current).arg(sum));
}


void FindWidget::replaceText()
{
    if(highlightSelections.isEmpty())
        return;
    try {
        // 计算下一个匹配项索引（循环）
        currentMatchIndex = (currentMatchIndex) % highlightSelections.size();
        QTextCursor cursor = highlightSelections[currentMatchIndex];

        if (cursor.hasSelection()) {
            cursor.insertText(repalceLineEdit->text());  // 替换选中的文字
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, repalceLineEdit->text().length());
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, repalceLineEdit->text().length());
            edit->setTextCursor(cursor);  // 选中新插入的文本
        }
        // qDebug ()<<"要删除的下标:"<<currentMatchIndex;

        highlightSelections.remove(currentMatchIndex);

        int index = currentMatchIndex;
        current=index+1;
        sum=highlightSelections.size();
        if(highlightSelections.isEmpty())
            current=0;
        countBtn->setText(QString("%1 / %2").arg(current).arg(sum));

        if(!highlightSelections.isEmpty())
            this->drawBackground(highlightSelections[currentMatchIndex % highlightSelections.size()]);
    } catch (...) {
         qDebug()<<__FUNCTION__<<"出错";
    }
}


void FindWidget::replaceAll()
{
    QTextDocument* document = edit->document();
    QTextCursor cursor(document);

    // 从文档开头开始查找
    cursor.movePosition(QTextCursor::Start);

    QString targetText = findLineEdit->text();
    QString replacementText = repalceLineEdit->text();

    // 开启批量修改，避免多次操作影响性能
    edit->blockSignals(true);
    edit->setUpdatesEnabled(false);

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = document->find(targetText, cursor);
        if (!cursor.isNull()) {
            cursor.insertText(replacementText);  // 替换文本
        }
    }

    // 重新启用界面更新
    edit->blockSignals(false);
    edit->setUpdatesEnabled(true);

    highlightSelections.clear();  // 清空高亮信息
}

//高级界面窗口
void FindWidget::advancedFunction()
{
    try {
        if (!advancedWidget) { // 只有当界面不存在时才创建
            advancedWidget = new QWidget(this);
            QVBoxLayout *topVlayout = new QVBoxLayout(advancedWidget);

            //标题布局
            QHBoxLayout* titleLayout=new QHBoxLayout;
            titleLayout->addStretch();
            QPushButton* cancelBtn=new QPushButton("v");
            cancelBtn->setFixedSize(20, 15);
            cancelBtn->setStyleSheet(cancelBtn->styleSheet()+"QPushButton{padding: 0;}");
            connect(cancelBtn, &QPushButton::clicked, this, &FindWidget::hideAdvancedPanel);
            titleLayout->addWidget(cancelBtn);
            topVlayout->addLayout(titleLayout);

            //第一层水平布局
            QHBoxLayout* hLayout=new QHBoxLayout;
            QCheckBox* isBigCkB=new QCheckBox("区分大小写");
            connect(isBigCkB,&QPushButton::clicked,this,[=]{
                qDebug()<<"变换";
                if(isBigCkB->isChecked())
                    this->options=QTextDocument::FindCaseSensitively;
                else
                    this->options=QTextDocument::FindFlags();
                qDebug()<<this->options;
                //更新查找
                findText();
                //画查找背景
                this->drawBackground();
                //更新显示个数
                if(!this->highlightSelections.isEmpty())
                {
                    currentMatchIndex = 0;
                    drawBackground(highlightSelections[0]);
                    countBtn->setText(QString("%1 / %2").arg(currentMatchIndex+1).arg(highlightSelections.size()));
                }

            });
            hLayout->addWidget(isBigCkB);
            topVlayout->addLayout(hLayout);

            mainLayout->addWidget(advancedWidget);

        }
        advancedWidget->show();
    } catch (...) {
        qDebug()<<__FUNCTION__<<"出错";
    }

}

void FindWidget::onAdvancedWidgetClosed(bool visible)
{
    if (!visible) {  // 当 advancedWidget 关闭时
        qDebug() << "advancedWidget 关闭，恢复高亮";
        findText(); // 重新应用高亮
    }
}
void FindWidget::hideAdvancedPanel() {
    if (advancedWidget) {
        advancedWidget->hide();
    }
}

void FindWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    edit->setExtraSelections({}); // 清除所有高亮
    highlightSelections.clear();
}
