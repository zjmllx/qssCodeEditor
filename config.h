#ifndef CONFIG_H
#define CONFIG_H

#include <QByteArray>
#include <QSplitter>
#include <QSettings>
#include <QFontDialog>

#include "myMenuBar/mytolbtn.h"
#include "myEditor/codeeditor.h"
#include "myEditor/highlighter.h"
#include "myCustomControl/myCustomBtn/jsonparse.h"
#include "myMenuBar/topbar.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>

struct CurrentThemeConfig
{
    QString currentThemeName;
    QMap<QString, QString> curThemePath;
};
class Config
{
public:
    Config();

    // json and qss
    ParseThemeData topBarJson_qss, menuJson_qss, codeEditorJson_qss, framecompoJson_qss, splittercompoJson_qss;
    LineNumberColor lineNumberColor;
    HighlighterData KWHighlighterData;  // 关键字高亮
    CurrentThemeConfig curThemeConfig;

    Highlighter *highlighter;

    void getMainHighlighter(Highlighter *highlighter);

    QString currentThemeName; // 当前主题名
    QDir projectPath; // 当前工程目录
    QDir getProjectIniPath();
    QString iniPath; // 主题文件路径

private:
    TopBar* topBar;
    MyTolBtn* tolBtn;
    CodeEditor* codeEditor;
    QFrame* frame;
    QSplitter *splitter;

public:
    void saveUserStyleConfig();

    CurrentThemeConfig getCurrentThemeConfig(const QString &filePath);
    void setCurrentConfigInfo();
    void updateCurrentThemeConfig(const QString &filePath, const CurrentThemeConfig &config);
    void updateCurrentThemeStyle();

    void setCignoBlankaStyle(); // 天鹅白主题样式
    void setLanFenZiStyle();  // 蓝粉紫主题样式
    void setMoYuHeiStyle();  // 墨玉黑主题样式
    void setQianBaiHuiStyle();  // 浅白灰主题样式

    void setLineNumberStyle(CodeEditor *codeEditor,
                            const QColor &selectedColor,
                            const QColor &noSelectedColor,
                            const QColor &backgroundColor);   // 设置行号样式

    void resumeDefaultText();  // 恢复默认的文字
    void updateAllKWHighlight(Highlighter *highlighter);

    QFontDialog* openFontDialog(QWidget *parent = nullptr);
    static Config* config();

public:
    void getTopBar(TopBar *topBar);
    void getMyTolBtn(MyTolBtn *tolBtn);
    void getCodeEditor(CodeEditor* codeEdotor);
    void getRightFrame(QFrame *frame);
    void getSplitter(QSplitter *splitter);
    QByteArray openFile(const QString &path);
};

#endif // CONFIG_H
