#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = nullptr);

    void setWidgetFormat_color(const QColor &color);
    void setPseudoStateFormat_color(const QColor &color);
    void setSubControlFormat_color(const QColor &color);
    void setPropertyFormat_color(const QColor &color);
    void setLayoutFormat_color(const QColor &color);
    void setGradientFormat_color(const QColor &color);
    void setCommentFormat_color(const QColor &color);

    QColor widgetFormatColor;
    QColor pseudoStateFormatColor;
    QColor subControlFormatColor;
    QColor propertyFormatColor;
    QColor layoutFormatColor;
    QColor gradientFormatColor;
    QColor commentFormatColor;

    void updataAllValue();

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QTextCharFormat widgetFormat;        // 组件选择器
    QTextCharFormat pseudoStateFormat;   // 伪状态限定符
    QTextCharFormat subControlFormat;    // 子控件标识符
    QTextCharFormat propertyFormat;      // 核心属性
    QTextCharFormat layoutFormat;        // 布局属性
    QTextCharFormat gradientFormat;      // 渐变效果
    QTextCharFormat commentFormat;       // 注释格式

    void updateRules(const QStringList &keywordPatterns, QTextCharFormat keywordFormat);

public:
    QVector<HighlightingRule> highlightingRules;
    QStringList widgetPatterns;
    QStringList pseudoStatePatterns;
    QStringList subControlPatterns;
    QStringList propertyPatterns;
    QStringList layoutPatterns;
    QStringList gradientPatterns;
    QStringList commentFormatPatterns;
    void addRules(const QStringList &keywordPatterns, QTextCharFormat keywordFormat);
};

#endif // HIGHLIGHTER_H

