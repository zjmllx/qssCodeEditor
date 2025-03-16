#include "highlighter.h"
#include "../config.h"

#include <QTextDocument>
#include <QRegularExpressionMatchIterator>
#include <QRegularExpressionMatch>
#include <QTextCharFormat>
#include <QTextDocument>

// 构造函数
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    this->widgetFormatColor = Config::config()->KWHighlighterData.highlighterColors["widgetFormat_color"];
    this->pseudoStateFormatColor = Config::config()->KWHighlighterData.highlighterColors["pseudoStateFormat_color"];
    this->subControlFormatColor = Config::config()->KWHighlighterData.highlighterColors["subControlFormat_color"];
    this->propertyFormatColor = Config::config()->KWHighlighterData.highlighterColors["propertyFormat_color"];
    this->layoutFormatColor = Config::config()->KWHighlighterData.highlighterColors["layoutFormat_color"];
    this->gradientFormatColor = Config::config()->KWHighlighterData.highlighterColors["gradientFormat_color"];
    this->commentFormatColor = Config::config()->KWHighlighterData.highlighterColors["commentFormat_color"];

    widgetFormat.setForeground(this->widgetFormatColor);
    widgetFormat.setFontWeight(QFont::Bold);
    widgetPatterns = {
        // 基础选择器类型
        "\\bQWidget\\b", "\\bQMainWindow\\b", "\\bQDialog\\b", "\\bQFrame\\b",
        "\\bQLabel\\b", "\\bQPushButton\\b", "\\bQLineEdit\\b", "\\bQComboBox\\b",
        "\\bQCheckBox\\b", "\\bQRadioButton\\b", "\\bQGroupBox\\b", "\\bQTabWidget\\b",
        "\\bQScrollBar\\b", "\\bQSlider\\b", "\\bQProgressBar\\b", "\\bQToolTip\\b",
        "\\bQTextBrowser\\b", "\\bQToolButton\\b", "\\bQDataEdit\\b",
        "\\bQTimeEdit\\b", "\\bQDataTimeEdit\\b", "\\bQDoubleSpinBox\\b"
        "\\bQDoubleSpinBox\\b", "\\bQFontComboBox\\b", "\\bQDial\\b",
        "\\bQPlainTextEdit\\b", "\\bQLCDNumber\\b", "\\bQColumnView\\b",
        "\\bQDoubleSpinBox\\b", "\\bQTextEdit\\b",

        // 高级选择器（Qt 6新增）
        "\\bQAbstractItemView\\b", "\\bQTreeView\\b", "\\bQListView\\b",
        "\\bQTableView\\b", "\\bQHeaderView\\b", "\\bQCalendarWidget\\b"
        };
    this->addRules(widgetPatterns, widgetFormat);

    // 伪状态（深紫色）
    pseudoStateFormat.setForeground(this->pseudoStateFormatColor);
    pseudoStatePatterns = {
        // 伪状态限定符
        "\\b:disabled\\b", "\\b:enabled\\b", "\\b:focus\\b", "\\b:hover\\b",
        "\\b:pressed\\b", "\\b:checked\\b", "\\b:unchecked\\b", "\\b:indeterminate\\b",
        "\\b:active\\b", "\\b:inactive\\b", "\\b:selected\\b", "\\b:exclusive\\b",
        "\\b:non-exclusive\\b", "\\b:horizontal\\b", "\\b:vertical\\b",
        "\\b:window\\b", "\\b:closable\\b", "\\b:default\\b", "\\bf:lat\\b"
    };
    this->addRules(pseudoStatePatterns, pseudoStateFormat);

    // ==== 子控件 ====
    // 子控件（深青色）
    subControlFormat.setForeground(this->subControlFormatColor);
    subControlPatterns = {
        // 子控件标识符（双冒号语法）
        "\\b::indicator\\b", "\\b::menu-indicator\\b", "\\b::item\\b",
        "\\b::up-arrow\\b", "\\b::down-arrow\\b", "\\b::drop-down\\b",
        "\\b::title\\b", "\\b::handle\\b", "\\b::groove\\b", "\\b::add-line\\b",
        "\\b::sub-line\\b", "\\b::branch\\b", "\\b::chunk\\b", "\\b::section\\b"
    };
    this->addRules(subControlPatterns, subControlFormat);

    // ==== 核心属性 ====
    // 核心属性（红色加粗）
    propertyFormat.setForeground(this->propertyFormatColor);
    propertyFormat.setFontWeight(QFont::Bold);
    propertyPatterns = {
        // 核心样式属性（Qt 6扩展）
        "\\bbackground\\s*:\\s*", "\\bbackground-color\\s*:\\s*", "\\bborder\\s*:\\s*",
        "\\bborder-radius\\s*:\\s*", "\\bcolor\\s*:\\s*", "\\bfont\\s*:\\s*", "\\bfont-famliy\\s*:\\s*", "\\bimage\\s*:\\s*",
        "\\bmargin\\s*:\\s*", "\\bpadding\\s*:\\s*", "\\bmin-width\\s*:\\s*", "\\bmax-width\\s*:\\s*",
        "\\bmin-height\\s*:\\s*", "\\bmax-height\\s*:\\s*", "\\bspacing\\s*:\\s*", "\\bsubcontrol-origin\\s*:\\s*",
        "\\bsubcontrol-position\\s*:\\s*", "\\boutline\\s*:\\s*", "\\bselection-background-color\\s*:\\s*"
    };
    this->addRules(propertyPatterns, propertyFormat);

    // ==== 布局属性 ====
    // 布局属性（橙色）
    layoutFormat.setForeground(this->layoutFormatColor);
    layoutPatterns = {
        // 高级布局属性
        "\\balternate-background-color\\s*:\\s*", "\\bgridline-color\\s*:\\s*",
        "\\bborder-style\\s*:\\s*", "\\bborder-width\\s*:\\s*", "\\bborder-color\\s*:\\s*",
        "\\bposition\\s*:\\s*", "\\bleft\\s*:\\s*", "\\bright\\s*:\\s*", "\\btop\\s*:\\s*", "\\bbottom\\s*:\\s*"
    };
    this->addRules(layoutPatterns, layoutFormat);

    // ==== 渐变效果 ====
    // 渐变效果（深绿色）
    gradientFormat.setForeground(this->gradientFormatColor);
    gradientPatterns = {
        // 渐变与效果（Qt 6增强）
        "\\bqlineargradient\\s*:\\s*", "\\bqradialgradient\\s*:\\s*", "\\bqconicalgradient\\s*:\\s*",
        "\\bqpathgradient\\s*:\\s*", "\\bqdrop-shadow\\s*:\\s*", "\\bqblur\\s*:\\s*",
        "\\bqcolorize\\s*:\\s*", "\\bqopacity\\s*:\\s*"
    };
    this->addRules(gradientPatterns, gradientFormat);

    // 注释样式
    commentFormat.setForeground(this->commentFormatColor);
    commentFormatPatterns = {"/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/"};
    this->addRules(commentFormatPatterns, commentFormat);
}

void Highlighter::setWidgetFormat_color(const QColor &color)
{
    // 组件选择器（蓝色加粗）
    widgetFormat.setForeground(color);
    widgetFormat.setFontWeight(QFont::Bold);
    this->updateRules(widgetPatterns, widgetFormat);
}

void Highlighter::setPseudoStateFormat_color(const QColor &color)
{
    pseudoStateFormat.setForeground(color);
    this->updateRules(pseudoStatePatterns, pseudoStateFormat);
}

void Highlighter::setSubControlFormat_color(const QColor &color)
{
    subControlFormat.setForeground(color);
    this->updateRules(subControlPatterns, subControlFormat);
}

void Highlighter::setPropertyFormat_color(const QColor &color)
{
    propertyFormat.setForeground(color);
    propertyFormat.setFontWeight(QFont::Bold);
    this->updateRules(propertyPatterns, propertyFormat);
}

void Highlighter::setLayoutFormat_color(const QColor &color)
{
    layoutFormat.setForeground(color);
    this->updateRules(layoutPatterns, layoutFormat);
}

void Highlighter::setGradientFormat_color(const QColor &color)
{
    gradientFormat.setForeground(color);
    this->updateRules(gradientPatterns, gradientFormat);
}

void Highlighter::setCommentFormat_color(const QColor &color)
{
    commentFormat.setForeground(color);
    this->updateRules(commentFormatPatterns, commentFormat);
}

// // 实现高亮
void Highlighter::highlightBlock(const QString &text) {
    // 使用标准库的 std::as_const 替代 qAsConst
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            int index = match.capturedStart();
            int length = match.capturedLength();
            setFormat(index, length, rule.format);
        }
    }
}

void Highlighter::addRules(const QStringList &keywordPatterns, QTextCharFormat keywordFormat)
{
    // 添加关键词高亮规则
    for (const QString &pattern : keywordPatterns)
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
}

void Highlighter::updateRules(const QStringList &keywordPatterns, QTextCharFormat keywordFormat)
{
    for (HighlightingRule &rule : highlightingRules) {
        if (keywordPatterns.contains(rule.pattern.pattern())) {
            rule.format = keywordFormat;
        }
    }
    rehighlight();
}
