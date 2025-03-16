#include "config.h"
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

Config::Config()
{
    this->curThemeConfig = this->getCurrentThemeConfig(":/config/currentTheme.ini");
    this->setCurrentConfigInfo();
}

void Config::setCurrentConfigInfo()
{
    currentThemeName = curThemeConfig.currentThemeName;
    topBarJson_qss = JsonParse::getJsonParse()->parseThemeJsonFromFile(
        this->curThemeConfig.curThemePath["currentTheme_topBar"]);
    menuJson_qss = JsonParse::getJsonParse()->parseThemeJsonFromFile(
        this->curThemeConfig.curThemePath["currentTheme_menu"]);
    framecompoJson_qss = JsonParse::getJsonParse()->parseThemeJsonFromFile(
        this->curThemeConfig.curThemePath["currentTheme_framecompo"]);
    codeEditorJson_qss = JsonParse::getJsonParse()->parseThemeJsonFromFile(
        this->curThemeConfig.curThemePath["currentTheme_codeeditor"]);
    splittercompoJson_qss = JsonParse::getJsonParse()->parseThemeJsonFromFile(
        this->curThemeConfig.curThemePath["currentTheme_splittercompo"]);

    lineNumberColor = JsonParse::getJsonParse()->parseLineNumberJsonGetAllColor(
        this->curThemeConfig.curThemePath["lineNumber_style"],
        this->curThemeConfig.currentThemeName);
    KWHighlighterData = JsonParse::getJsonParse()->parseHighlighterColorsFromFile(
        this->curThemeConfig.curThemePath["highlightwk_color"]);
}

void Config::getMainHighlighter(Highlighter *highlighter)
{
    this->highlighter = highlighter;
}

// 天鹅白主题
void Config::setCignoBlankaStyle()
{
    this->curThemeConfig.currentThemeName = "天鹅白";
    this->curThemeConfig.curThemePath["currentTheme_codeeditor"] = ":/style/systemStyle/tianebai/codeeditor.json";
    this->curThemeConfig.curThemePath["currentTheme_framecompo"] = ":/style/systemStyle/tianebai/framecompo.json";
    this->curThemeConfig.curThemePath["currentTheme_menu"] = ":/style/systemStyle/tianebai/menu.json";
    this->curThemeConfig.curThemePath["currentTheme_splittercompo"] = ":/style/systemStyle/tianebai/splittercompo.json";
    this->curThemeConfig.curThemePath["currentTheme_topBar"] = ":/style/systemStyle/tianebai/topBar.json";
    this->curThemeConfig.curThemePath["highlightwk_color"] = ":/style/systemStyle/tianebai/highlightwk.json";
    this->curThemeConfig.curThemePath["lineNumber_style"] = ":/style/systemStyle/tianebai/lineNumber.json";

    this->setCurrentConfigInfo();
    this->updataCurrentThemeStyle();
}

// 蓝紫粉主题
void Config::setLanFenZiStyle()
{
    this->curThemeConfig.currentThemeName = "蓝紫粉";
    this->curThemeConfig.curThemePath["currentTheme_codeeditor"] = ":/style/systemStyle/lanzifen/codeeditor.json";
    this->curThemeConfig.curThemePath["currentTheme_framecompo"] = ":/style/systemStyle/lanzifen/framecompo.json";
    this->curThemeConfig.curThemePath["currentTheme_menu"] = ":/style/systemStyle/lanzifen/menu.json";
    this->curThemeConfig.curThemePath["currentTheme_splittercompo"] = ":/style/systemStyle/lanzifen/splittercompo.json";
    this->curThemeConfig.curThemePath["currentTheme_topBar"] = ":/style/systemStyle/lanzifen/topBar.json";
    this->curThemeConfig.curThemePath["highlightwk_color"] = ":/style/systemStyle/lanzifen/highlightwk.json";
    this->curThemeConfig.curThemePath["lineNumber_style"] = ":/style/systemStyle/lanzifen/lineNumber.json";
    this->setCurrentConfigInfo();
    this->updataCurrentThemeStyle();
}

// 墨玉黑主题
void Config::setMoYuHeiStyle()
{
    this->curThemeConfig.currentThemeName = "墨玉黑";
    this->curThemeConfig.curThemePath["currentTheme_codeeditor"] = ":/style/systemStyle/moyuhei/codeeditor.json";
    this->curThemeConfig.curThemePath["currentTheme_framecompo"] = ":/style/systemStyle/moyuhei/framecompo.json";
    this->curThemeConfig.curThemePath["currentTheme_menu"] = ":/style/systemStyle/moyuhei/menu.json";
    this->curThemeConfig.curThemePath["currentTheme_splittercompo"] = ":/style/systemStyle/moyuhei/splittercompo.json";
    this->curThemeConfig.curThemePath["currentTheme_topBar"] = ":/style/systemStyle/moyuhei/topBar.json";
    this->curThemeConfig.curThemePath["highlightwk_color"] = ":/style/systemStyle/moyuhei/highlightwk.json";
    this->curThemeConfig.curThemePath["lineNumber_style"] = ":/style/systemStyle/moyuhei/lineNumber.json";
    this->setCurrentConfigInfo();
    this->updataCurrentThemeStyle();
}

// 浅白灰主题
void Config::setQianBaiHuiStyle()
{
    this->curThemeConfig.currentThemeName = "浅白灰";
    this->curThemeConfig.curThemePath["currentTheme_codeeditor"] = ":/style/systemStyle/qianbaihui/codeeditor.json";
    this->curThemeConfig.curThemePath["currentTheme_framecompo"] = ":/style/systemStyle/qianbaihui/framecompo.json";
    this->curThemeConfig.curThemePath["currentTheme_menu"] = ":/style/systemStyle/qianbaihui/menu.json";
    this->curThemeConfig.curThemePath["currentTheme_splittercompo"] = ":/style/systemStyle/qianbaihui/splittercompo.json";
    this->curThemeConfig.curThemePath["currentTheme_topBar"] = ":/style/systemStyle/qianbaihui/topBar.json";
    this->curThemeConfig.curThemePath["highlightwk_color"] = ":/style/systemStyle/qianbaihui/highlightwk.json";
    this->curThemeConfig.curThemePath["lineNumber_style"] = ":/style/systemStyle/qianbaihui/lineNumber.json";
    this->setCurrentConfigInfo();
    this->updataCurrentThemeStyle();
}

void Config::setLineNumberStyle(CodeEditor *codeEditor, const QColor &selectedColor, const QColor &noSelectedColor, const QColor &backgroundColor)
{
    codeEditor->setLineNumberSelectedColor(selectedColor);
    codeEditor->setLineNumberNoSelectedColor(noSelectedColor);
    codeEditor->setLineNumberBackgroundColor(backgroundColor);
}

QByteArray Config::openFile(const QString &path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(nullptr, "错误", QString("打开文件失败！%1").arg(path));
        return QByteArray();
    }

    QByteArray styleData = file.readAll();
    file.close();
    return styleData;
}

Config* Config::config()
{
    static Config* config = nullptr;
    if(!config)
    {
        config =new Config;
    }

    return config;
}

void Config::getTopBar(TopBar *topBar)
{
    this->topBar = topBar;
}

void Config::getCodeEditor(CodeEditor *codeEditor)
{
    this->codeEditor = codeEditor;
}

void Config::getSplitter(QSplitter *splitter)
{
    this->splitter = splitter;
}

void Config::getRightFrame(QFrame *frame)
{
    this->frame = frame;
}


void Config::resumeDefaultText()
{
    if(this->codeEditor){
        this->codeEditor->setPlainText(this->openFile(":/config/style.txt"));
    }
}

void Config::updataAllKWHighlight()
{
    if(this->highlighter){
        this->highlighter->setWidgetFormat_color(KWHighlighterData.highlighterColors["widgetFormat_color"]);
        this->highlighter->setPseudoStateFormat_color(KWHighlighterData.highlighterColors["pseudoStateFormat_color"]);
        this->highlighter->setSubControlFormat_color(KWHighlighterData.highlighterColors["subControlFormat_color"]);
        this->highlighter->setPropertyFormat_color(KWHighlighterData.highlighterColors["propertyFormat_color"]);
        this->highlighter->setLayoutFormat_color(KWHighlighterData.highlighterColors["layoutFormat_color"]);
        this->highlighter->setGradientFormat_color(KWHighlighterData.highlighterColors["gradientFormat_color"]);
        this->highlighter->setCommentFormat_color(KWHighlighterData.highlighterColors["commentFormat_color"]);
    }
}

CurrentThemeConfig Config::getCurrentThemeConfig(const QString &filePath)
{
    QSettings settings(filePath, QSettings::IniFormat);
    CurrentThemeConfig config;

    // 获取 currentThemeStyle 段落的值
    settings.beginGroup("currentThemeStyle");
    config.currentThemeName = settings.value("name").toString();
    QStringList keys = settings.childKeys();
    foreach (const QString &key, keys) {
        if (key != "name") {
            config.curThemePath.insert(key, settings.value(key).toString());
        }
    }
    settings.endGroup();
    return config;
}

void Config::updataCurrentThemeStyle()
{
    if(this->topBar){
        //设置样式
        this->topBar->setStyleSheet(topBarJson_qss.themeQssData);
        this->topBar->setTopBarBtnToMenuStyle(menuJson_qss.themeQssData);
    }
    if(this->codeEditor){
        this->codeEditor->setStyleSheet(codeEditorJson_qss.themeQssData);
        this->setLineNumberStyle(this->codeEditor,lineNumberColor.selectedColor,
                                 lineNumberColor.noSelectedColor,
                                 lineNumberColor.backgroundColor);
    }
    if(this->frame){
        this->frame->setStyleSheet(framecompoJson_qss.themeQssData);
    }
    if(this->splitter){
        this->splitter->setStyleSheet(splittercompoJson_qss.themeQssData);
    }
    this->updataAllKWHighlight();
}

void Config::updateCurrentThemeConfig(const QString &filePath, const CurrentThemeConfig &config)
{
    QSettings settings(filePath, QSettings::IniFormat);

    // 更新 currentThemeStyle 段落的值
    settings.beginGroup("currentThemeStyle");
    settings.setValue("name", config.currentThemeName);
    for (auto it = config.curThemePath.begin(); it != config.curThemePath.end(); ++it) {
        settings.setValue(it.key(), it.value());
    }
    settings.endGroup();
}

QFontDialog* Config::openFontDialog(QWidget *parent)
{
    QFontDialog *fontDialog = new QFontDialog(parent);
    fontDialog->setWindowIcon(QIcon(":/img/images/font.png"));
    fontDialog->setStyleSheet(R"(
        QFontDialog {
            background-color: #f0f0ff; /* 淡蓝色背景 */
        }
        QFontDialog QDialogButtonBox {
            background-color: #e0e0ff; /* 淡紫色背景 */
        }
        QFontDialog QPushButton {
            background-color: #ffccff; /* 粉色背景 */
            border: 1px solid #cc99cc; /* 粉色边框 */
            color: blue;
            width: 60px;
            border-radius: 5px;
            padding: 5px;
        }
        QFontDialog QPushButton:hover {
            background-color: #ff99ff; /* 悬停时的粉色背景 */
        }
        QFontDialog QLabel {
            color: #660066; /* 深紫色文本 */
        }
        QFontDialog QLineEdit {
            background-color: #ffffff; /* 白色背景 */
            border: 1px solid #cc99cc; /* 粉色边框 */
            border-radius: 3px;
            padding: 3px;
        }
        QFontDialog QListView {
            background-color: #ffffff; /* 白色背景 */
            border: 1px solid #cc99cc; /* 粉色边框 */
        }
    )");
    return fontDialog;
}
