#include "codeeditor.h"

#include <QMessageBox>
#include <QTextBlock>       // 引入文本块相关的头文件
#include <QPainter>
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QFile>
#include <QMenu>
#include <QApplication>
#include <QFileDialog>

CodeEditor::CodeEditor(QWidget *parent):
    QPlainTextEdit(parent), m_lineNumberArea(new LineNumberArea(this))
{
    // 连接信号与槽，更新行号区域的宽度、请求更新行号区域和高亮当前行
    setMouseTracking(true);  // 开启鼠标追踪

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    // 设置自定义右键菜单策略
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,&QPlainTextEdit::customContextMenuRequested,this,&CodeEditor::rightMenu);
    initColorConstant();
    this->initView();
    this->setTabDistance();
}

// 初始化
void CodeEditor::initView()
{
    lab_color = new QLabel(this);
    lab_color->setFixedSize(20, 20);  // 设置颜色框大小
    this->hideColorLabel();

    lineNumberSelectedColor = QColor(255, 0, 0);
    lineNumberNoSelectedColor = QColor(0, 0, 0);
    lineNumberBackgroundColor = QColor(237, 237, 237);

    // 程序一开始运行不会调用它，这里手动调用一次，要不然文本框的左边视图会被数字区域覆盖掉
    this->updateLineNumberAreaWidth(0);

    this->highlightCurrentLine();       // 高亮当前行

    QTextBlock block = firstVisibleBlock(); // 获取第一个可见文本块
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top()); // 当前块顶部
    int bottom = static_cast<int>(blockBoundingRect(block).height()); // 当前块底部
    this->textEditTop = top;
    this->currentRowHeight = bottom - top;    // 初始时的行高

    // 初始化 QLabel 和 QTimer
    fontSizeLabel = new QLabel(this);
    fontSizeLabel->setStyleSheet("background-color: rgba(255, 255, 255, 90); "
                                 "color: white; "
                                 "padding: 5px; "
                                 "border-radius: 5px;");
    fontSizeLabel->setAlignment(Qt::AlignCenter);
    fontSizeLabel->setAttribute(Qt::WA_TransparentForMouseEvents); // 忽略鼠标事件
    fontSizeLabel->hide();

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [=](){
        fontSizeLabel->hide();
    });
}

// 更新行号区域的宽度
void CodeEditor::updateLineNumberAreaWidth(int) // int 是占位符
{
    int w = lineNumberAreaWidth();
    this->setViewportMargins(w, 0, 0, 0);
}

// 调整窗口大小事件
void CodeEditor::resizeEvent(QResizeEvent *e)
{
    m_lineNumberArea->resize(lineNumberAreaWidth(), this->height());

    return QPlainTextEdit::resizeEvent(e); // 调用基类的 resizeEvent
}

void CodeEditor::wheelEvent(QWheelEvent *event)
{
    // 检查是否按下了 Ctrl 键
    if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        QFont font = this->document()->defaultFont();
        int newPointSize = font.pointSize();
        if (event->angleDelta().y() > 0) { // 滚轮向上滚动
            newPointSize += 1;
        }
        else { // 滚轮向下滚动
            newPointSize -= 1;  // 缩小字体
        }
        font.setPointSize(newPointSize);

        // 设置文档默认字体
        this->document()->setDefaultFont(font);

        // 更新已有文本的格式
        QTextCursor cursor(this->document());
        cursor.select(QTextCursor::Document);
        QTextCharFormat format;
        format.setFont(font);
        cursor.mergeCharFormat(format);

        // 更新 QLabel 的文本并显示
        fontSizeLabel->setText(QString("当前字体: %1").arg(newPointSize));
        fontSizeLabel->adjustSize();
        fontSizeLabel->move((width() - fontSizeLabel->width()) / 2, (height() - fontSizeLabel->height()) / 2);
        fontSizeLabel->show();

        // 重置 QTimer
        timer->start(3000); // 3秒后隐藏

        // 阻止事件进一步传播
        event->accept();
        return;
    }
    // 处理其他滚轮事件
    QPlainTextEdit::wheelEvent(event);
}

void CodeEditor::mouseMoveEvent(QMouseEvent *event)
{
    //函数功能:查看鼠标附近是否有有效颜色值,如果有效就在旁别显示一个对应颜色的Label
    // 将全局坐标转换为相对于 QPlainTextEdit 的坐标
    QPoint localPos = this->mapFromGlobal(event->globalPosition().toPoint())-QPoint(this->m_lineNumberArea->width(),0);
    // 判断鼠标是否在 QPlainTextEdit 的有效区域内
    if (!rect().contains(localPos)) {
        hideColorLabel();
        return QPlainTextEdit::mouseMoveEvent(event);  // 如果鼠标不在 QPlainTextEdit 内部，跳过处理
    }

    QTextCursor cursor = cursorForPosition(localPos);
    QTextBlock block = cursor.block();  // 获取当前行

    // 获取所在行的文本内容
    QString lineText = block.text();

    // 判断该行是否为空
    if (lineText.isEmpty()) {

        hideColorLabel();
        return QPlainTextEdit::mouseMoveEvent(event);  // 如果是空行，直接返回
    }

    // 获取鼠标在当前行的字符位置
    int cursorPosInLine = cursor.positionInBlock();
    // qDebug()<<"\n\n-------------------------";
    // qDebug()<<"当前鼠标文本长度:"<<lineText.length()<<"\t"<<"鼠标在文本的位置:"<<cursorPosInLine;

    // 判断鼠标是否在有效文本范围内
    if (cursorPosInLine < 0 || cursorPosInLine >= lineText.length()) {

        hideColorLabel();
        return QPlainTextEdit::mouseMoveEvent(event);  // 如果鼠标不在有效文本范围内，直接返回
    }

    // 获取左边界：往左查找，遇到空格或 # 停止
    int startIdx = cursorPosInLine;
    int count = 0;
    while (startIdx >= 3 && count <= 9) {
        QChar c = lineText[startIdx - 1];
        if(c == '#' || (c == ' ' && lineText[startIdx - 3] == ' ') ||  c == ';'){
            startIdx--;
            break;
        }else if(c == 'b' && lineText[startIdx - 2] == 'g' && lineText[startIdx - 3] == 'r'){
            startIdx -= 3;
            break;
        }else if(c == ')' ||
                   (c == 'g' && lineText[startIdx - 2] == 'r' && lineText[startIdx - 3] == ' ')||
                   (c == 'r' && lineText[startIdx - 2] == ' ')){
            break;
        }
        startIdx--;
        count++;
    }

    // 获取右边界：往右查找，遇到空格或 # 停止
    int endIdx = cursorPosInLine;
    count = 0;
    while (endIdx < lineText.length() && count <= 9) {
        QChar c = lineText[endIdx];
        if ( c == ')' || c == '(' || c == '#' || c == ';') {
            endIdx++;
            break;
        }
        endIdx++;
        count++;
    }

    QString textToCheck = lineText.mid(startIdx, endIdx - startIdx);
    // 使用正则表达式来匹配 HEX 或 RGB 颜色
    static QRegularExpression hexColorRegExp("#[0-9A-Fa-f]{6}([0-9A-Fa-f]{2})?");
    static QRegularExpression rgbColorRegExp("rgb\\((\\d{1,3}),\\s*(\\d{1,3}),\\s*(\\d{1,3})\\)");
    static QRegularExpression ColorNameRegExp("\\b("+colorStr+")\\b");

    // 查找符合条件的颜色值
    QRegularExpressionMatch hexMatch = hexColorRegExp.match(textToCheck);
    QRegularExpressionMatch rgbMatch = rgbColorRegExp.match(textToCheck);
    //匹配颜色名字
    QRegularExpressionMatch colorNameMatch = ColorNameRegExp.match(textToCheck);

    QColor color;
    if (hexMatch.hasMatch()) {
        //匹配16进制
        // qDebug()<<"捕获到十六进制:"<<color;
        color = QColor::fromString(hexMatch.captured(0));  // 获取匹配的颜色值
    }
    if (rgbMatch.hasMatch()) {
        //匹配rgb
        // qDebug()<<"捕获到rgb:"<<color;
        int r = rgbMatch.captured(1).toInt();
        int g = rgbMatch.captured(2).toInt();
        int b = rgbMatch.captured(3).toInt();
        color.setRgb(r, g, b);  // 创建 RGB 颜色
    }
    if(colorNameMatch.hasMatch())
    {

        // qDebug()<<"匹配的文本:"<<lineText;
        // qDebug()<<"鼠标位置:"<<localPos;
        QString colorName=colorNameMatch.captured(0);  // 获取匹配的颜色值
        //获取颜色字符在当前鼠标行的位置
        int pos=lineText.indexOf(colorName);
        if(cursorPosInLine>=pos&&cursorPosInLine<=pos+QString(colorName).length())
        {
            //如果鼠标位置在颜色名字范围内
            color=colorKeywords[colorName];
            // qDebug()<<"捕获到的颜色常量:"<<colorName;
        }
        else
        {
            //如果不在
            hideColorLabel();
            return QPlainTextEdit::mouseMoveEvent(event);
        }

    }

    if (color.isValid()) {
        // 显示颜色框
        showColorLabel(localPos, color);
        return QPlainTextEdit::mouseMoveEvent(event);
    }

    hideColorLabel();
    QPlainTextEdit::mouseMoveEvent(event);

}
// 计算行号区域的宽度
int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;     // 行号所需的数字位数
    int max = qMax(1, blockCount()); // 获取当前文本块的最大数量
    while (max >= 10)   // 计算需要多少位数来显示最大行号
    {
        max /= 10;
        ++digits;
    }
    // 返回宽度，考虑到字体的宽度
    int space = 13 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

// 更新行号区域
void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        m_lineNumberArea->scroll(0, dy); // 如果有鼠标垂直滚动，滚动行号区域
    } else {
        // 更新行号区域的绘制
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    // 如果更新区域包含视口，重新计算行号区域宽度
    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

// 绘制行号区域
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea); // 创建绘图对象
    painter.fillRect(event->rect(), lineNumberBackgroundColor); // 填充行号区域背景

    // 设置字体样式与编辑框一致
    QFont editorFont =  this->document()->defaultFont();
    painter.setFont(editorFont);
    QFontMetrics fm(editorFont);

    QTextBlock block = firstVisibleBlock(); // 获取第一个可见文本块
    int blockNumber = block.blockNumber(); // 当前块的行号
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top()); // 当前块顶部
    int bottom = top + static_cast<int>(blockBoundingRect(block).height()); // 当前块底部

    // 获取当前选中的文本区域的起始和结束行
    QTextCursor cursor = textCursor();
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    int startLine = document()->findBlock(selectionStart).blockNumber();
    int endLine = document()->findBlock(selectionEnd).blockNumber();

    // 绘制行号
    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        { // 检查块是否可见
            QString number = QString::number(blockNumber + 1); // 行号

            // 如果当前行在选中范围内，将行号设置为蓝色
            if (blockNumber >= startLine && blockNumber <= endLine) {
                painter.setPen(lineNumberSelectedColor);
            } else {
                painter.setPen(lineNumberNoSelectedColor);
            }

            // 计算背景矩形的高度
            //QRect rect(0, top, m_lineNumberArea->width(), this->fontMetrics().height() + 4);

            // 填充背景颜色
            //painter.fillRect(rect, QColorConstants::Svg::lightblue); // 设置背景颜色

            // 绘制行号
            painter.drawText(0, top, m_lineNumberArea->width(), fm.height(),
                             Qt::AlignCenter, number);
        }
        block = block.next(); // 移动到下一个块
        top = bottom; // 更新顶部
        bottom = top + static_cast<int>(blockBoundingRect(block).height()); // 更新底部
        ++blockNumber; // 增加行号
    }
}

QByteArray CodeEditor::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "打开文件",
        QDir::homePath(),
        "文本文件 (*.txt *.css *.qss *.ini *.xml *.json *.log);;所有文件 (*)"
        );

    if (filePath.isEmpty()) {
        // 用户取消了选择
        return QByteArray();
    }

    // 检查文件后缀名是否符合要求
    if (!checkFileExtension(filePath)) {
        QMessageBox::warning(this, "打开文件", "打开失败，请选择支持的文件类型！");
        return QByteArray();
    }

    // 获取文件内容
    QFile file(filePath);
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) { // 以只读模式打开文件
            QByteArray fileContent = file.readAll(); // 读取文件的二进制内容
            file.close();
            return fileContent; // 返回文件内容
        } else {
            // 文件打开失败
            QMessageBox::critical(this, "打开文件", "打开文件失败: " + filePath + "\n错误: " + file.errorString());
            return QByteArray(); // 返回空字节数组
        }
    } else {
        // 文件不存在
        QMessageBox::critical(this, "打开文件", "文件不存在: " + filePath);
        return QByteArray(); // 返回空字节数组
    }
}

// 高亮当前行
void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections; // 用于高亮的额外选择
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection; // 创建选择对象
        highlightColor = QColor(46,47,48).lighter(130); // 设置高亮颜色
        selection.format.setBackground(highlightColor); // 设置背景颜色
        selection.format.setProperty(QTextFormat::FullWidthSelection, true); // 全宽选择,一整行
        selection.cursor = textCursor(); // 获取当前光标
        selection.cursor.clearSelection(); // 清除选择(如果当前行被选中，则清除选中，使得任然以高光背景显示)
        extraSelections.append(selection); // 添加到额外选择列表
    }
    setExtraSelections(extraSelections); // 设置额外选择
}

void CodeEditor::setTabDistance()
{
    // 获取当前字体的度量信息
    QFontMetrics metrics(this->font());
    int charWidth = metrics.averageCharWidth(); // 计算四个空格的宽度
    // 设置 Tab 停止距离
    QTextOption textOption = this->document()->defaultTextOption();
    textOption.setTabStopDistance(charWidth * 5);  // 设置 Tab 键的字符长度
    this->document()->setDefaultTextOption(textOption);

    this->setLineWrapMode(QPlainTextEdit::NoWrap);
    this->currentRowHeight = this->getRowHeight();
}

int CodeEditor::getRowHeight() const
{
    QTextBlock block = firstVisibleBlock(); // 获取第一个可见文本块
    int bottom = static_cast<int>(blockBoundingRect(block).height()); // 当前块底部
    return bottom;
}

void CodeEditor::showColorLabel(const QPoint &pos, const QColor &color)
{
    // 设置颜色框的背景色
    lab_color->setStyleSheet(QString("background-color: %1;").arg(color.name()));
    // 设置颜色框的位置，调整为鼠标位置旁边
    lab_color->move(pos.x() + 43, pos.y() + 15);
    lab_color->show();  // 显示颜色框
}

void CodeEditor::hideColorLabel()
{
    lab_color->hide();
}

void CodeEditor::saveAs()
{
    // 打开文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,  // 父窗口
        tr("保存文件"),  // 对话框标题
        QDir::homePath(),  // 默认路径（用户主目录）
        tr("样式文件 (*txt *css *qss);;所有文件 (*)")  // 文件过滤器
        );

    // 检查用户是否取消了操作
    if (filePath.isEmpty()) {
        QMessageBox::information(nullptr, tr("保存文件"), tr("取消保存！"));
        return;
    }

    // 打开文件进行写入
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << this->toPlainText();  // 将 QPlainTextEdit 的内容写入文件
        file.close();
        QMessageBox::information(nullptr, tr("保存文件"), tr("文件保存成功！"));
    } else {
        QMessageBox::critical(nullptr, tr("保存文件"), tr("文件保存失败！"));
    }
}

bool CodeEditor::checkFileExtension(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString fileExtension = fileInfo.suffix().toLower();

    // 允许的文件后缀
    const QStringList allowedExtensions = {"txt", "css", "qss", "ini", "xml", "json", "log"};

    return allowedExtensions.contains(fileExtension);
}

bool CodeEditor::readFileContent(const QString &filePath, QString &content)
{
    QFile file(filePath);
    if (file.exists()) {
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            content = in.readAll();
            file.close();
            return true;
        } else {
            // 文件打开失败
            QMessageBox::critical(this, "Open Failed", "Failed to open the file: " + filePath + "\nError: " + file.errorString());
            return false;
        }
    } else {
        // 文件不存在
        QMessageBox::critical(this, "File Not Found", "The file does not exist: " + filePath);
        return false;
    }
}

void CodeEditor::rightMenu(const QPoint &pos)
{

    QMenu menu;
    QAction *copyAction = menu.addAction(tr("复制"));
    QAction *pasteAction = menu.addAction(tr("粘贴"));
    QAction *selectAllAction = menu.addAction(tr("全选"));
    menu.addSeparator();
    QAction *customAction = menu.addAction(tr("其他"));

    // 连接自定义操作
    connect(copyAction, &QAction::triggered, this, &QPlainTextEdit::copy);
    connect(pasteAction, &QAction::triggered, this, &QPlainTextEdit::paste);
    connect(selectAllAction, &QAction::triggered, this, &QPlainTextEdit::selectAll);
    connect(customAction, &QAction::triggered, []() {
        QMessageBox::information(nullptr, tr("Custom Action"), tr("你点击了其他!"));
    });
    menu.setStyleSheet(R"(
        QMenu {
            background: #EEFCFF;
            border: 2px solid #566266;
            border-radius: 0px;
            font-size: 10pt;
            padding: 2px;
            font-family: 华文中宋;
        }

        QMenu::item {
            background-color: transparent;
            color: #1455FF;
            min-width: 100px;
            min-height: 30px;
            padding-left: 5px;
            border-radius: 10px;
        }

        QMenu::item:selected {
            background-color: #737475;
            color: #F0F0F0;
        }

        QMenu::separator {
            height: 1px;
            background: #c8d6e5;
            margin: 5px 0;
        }

        QMenu::icon {
            width: 16px;
            height: 16px;
            margin-left: 10px;
        }
    )");
    // 显示菜单
    menu.exec(this->mapToGlobal(pos));
}

void CodeEditor::setCompleter(MyCompleter *completer)
{
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, QOverload<const QString &>::of(&QCompleter::activated),
                     this, &CodeEditor::insertCompletion);
}

MyCompleter *CodeEditor::completer() const
{
    return c;
}

void CodeEditor::setLineNumberSelectedColor(const QColor &color)
{
    this->lineNumberSelectedColor = color;
}

void CodeEditor::setLineNumberNoSelectedColor(const QColor &color)
{
    this->lineNumberNoSelectedColor = color;
}

void CodeEditor::setLineNumberBackgroundColor(const QColor &color)
{
    this->lineNumberBackgroundColor = color;
}

QColor CodeEditor::getColorFromText(const QString &text) const
{
    static QRegularExpression hexColorRegExp("#[0-9A-Fa-f]{6}([0-9A-Fa-f]{2})?");
    static QRegularExpression rgbColorRegExp("rgb\\((\\d{1,3}),\\s*(\\d{1,3}),\\s*(\\d{1,3})\\)");

    QRegularExpressionMatch hexMatch = hexColorRegExp.match(text);
    QRegularExpressionMatch rgbMatch = rgbColorRegExp.match(text);

    if (hexMatch.hasMatch()) {
        return QColor::fromString(hexMatch.captured(0));
    } else if (rgbMatch.hasMatch()) {
        int r = rgbMatch.captured(1).toInt();
        int g = rgbMatch.captured(2).toInt();
        int b = rgbMatch.captured(3).toInt();
        return QColor(r, g, b);
    }

    return QColor();  // 无效颜色
}

void CodeEditor::insertCompletion(const QString &completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CodeEditor::completionPrefix() const
{
    QTextCursor tc = textCursor();
    QString blockText = tc.block().text();
    int pos = tc.positionInBlock();
    int start = pos;
    while (start > 0) {
        QChar ch = blockText.at(start - 1);
        if (ch.isLetterOrNumber() || ch == '-') {
            start--;
        } else {
            break;
        }
    }
    return blockText.mid(start, pos - start);
}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        const int initPos = cursor.position();

        // 检测前一个字符
        cursor.movePosition(QTextCursor::Left,  QTextCursor::KeepAnchor, 1);
        const QString leftChar = cursor.selectedText();
        cursor.setPosition(initPos);

        // 检测后一个字符
        cursor.movePosition(QTextCursor::Right,  QTextCursor::KeepAnchor, 1);
        const QString rightChar = cursor.selectedText();
        cursor.setPosition(initPos);

        // 智能补全逻辑
        if (leftChar == "{" && rightChar != "}") {
            // 在未闭合的大括号末尾换行
            cursor.insertText("\n\t\n}");
            cursor.movePosition(QTextCursor::Left);
        } else if (leftChar == "(" && rightChar != ")") {
            // 在未闭合的圆括号末尾换行
            cursor.insertText("\n\t\n)");
            cursor.movePosition(QTextCursor::Left);
        } else if ((leftChar == "{" && rightChar == "}") ||
                   (leftChar == "(" && rightChar == ")")) {
            // 在闭合符号内部换行
            cursor.insertText("\n\t\n");
        } else if(leftChar == "}"){
            cursor.insertText("\n");
            return;
        } else {
            // 默认换行行为
            cursor.insertText("\n\t");
            return;
        }
        // 光标定位优化
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }

    // 改进后的括号输入处理
    if (e->text() == "{") {
        QTextCursor cursor = textCursor();
        cursor.insertText("{}");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    } else if (e->text() == "(") {
        QTextCursor cursor = textCursor();
        cursor.insertText("()");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = this->completionPrefix();
    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 1
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}

void CodeEditor::initColorConstant()
{
    // 初始化颜色名称映射
    colorKeywords["light"] = QColor(255, 255, 255);
    colorKeywords["lightgray"] = QColor(211, 211, 211);
    colorKeywords["lightblue"] = QColor(173, 216, 230);
    colorKeywords["lightgreen"] = QColor(144, 238, 144);
    colorKeywords["lightyellow"] = QColor(255, 255, 224);
    colorKeywords["lightpink"] = QColor(255, 182, 193);
    colorKeywords["lightcyan"] = QColor(224, 255, 255);
    colorKeywords["lightcoral"] = QColor(240, 128, 128);
    colorKeywords["lightgoldenrodyellow"] = QColor(250, 250, 210);

    colorKeywords["black"] = QColor(0, 0, 0);
    colorKeywords["white"] = QColor(255, 255, 255);
    colorKeywords["red"] = QColor(255, 0, 0);
    colorKeywords["green"] = QColor(0, 255, 0);
    colorKeywords["blue"] = QColor(0, 0, 255);
    colorKeywords["yellow"] = QColor(255, 255, 0);
    colorKeywords["cyan"] = QColor(0, 255, 255);
    colorKeywords["magenta"] = QColor(255, 0, 255);
    colorKeywords["gray"] = QColor(128, 128, 128);
    colorKeywords["darkgray"] = QColor(169, 169, 169);
    colorKeywords["darkblue"] = QColor(0, 0, 139);
    colorKeywords["darkgreen"] = QColor(0, 100, 0);
    colorKeywords["darkred"] = QColor(139, 0, 0);
    colorKeywords["darkyellow"] = QColor(204, 204, 0);
    colorKeywords["darkcyan"] = QColor(0, 139, 139);
    colorKeywords["darkmagenta"] = QColor(139, 0, 139);

    colorKeywords["transparent"] = QColor(0, 0, 229, 50);
    colorKeywords["darkslategray"] = QColor(47, 79, 79);
    colorKeywords["cornflowerblue"] = QColor(100, 149, 237);
    colorKeywords["seashell"] = QColor(255, 245, 238);
    colorKeywords["snow"] = QColor(255, 250, 250);
    colorKeywords["ivory"] = QColor(255, 255, 240);
    colorKeywords["honeydew"] = QColor(240, 255, 240);
    colorKeywords["pink"] = QColor(255, 192, 203);

    //获得颜色列表
    colorNameList=colorKeywords.keys();
    //根据列表构建匹配表达式
    colorStr=colorNameList.join("|");
}

CodeEditor::~CodeEditor(){}
