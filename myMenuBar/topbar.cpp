#include "topbar.h"
#include "../config.h"
#include "./customDialog/myCustomStyle/mycustomstyle.h"
#include "./customDialog/myColorDialog/mycolordialog.h"
#include "../interfaceUi/useguide.h"
#include "../interfaceUi/aboutus.h"

#include <QMessageBox>
#include <QFontDialog>

TopBar::TopBar(QWidget *parent)
    : QWidget{parent}
{
    // 顶部菜单栏
    this->setAttribute(Qt::WA_StyledBackground);
    this->customstyle = nullptr;

    spacing = 5;
    btn_height = 26;
    btn_width = 70;
    leftMargin = 2;
    topMargin = 0;

    // 初始化按钮和菜单
    btn_file = new QPushButton(this);
    btn_edit = new QPushButton(this);
    btn_setting = new QPushButton(this);
    btn_help = new QPushButton(this);
    btn_about = new QPushButton(this);

    menu_file = new MyMenu;
    menu_edit = new MyMenu;
    menu_setting = new MyMenu;
    menu_help = new MyMenu;
    menu_about = new MyMenu;

    auto openFileAction=new QAction(QIcon(":/img/images/openfile.png"),"打开文件");
    connect(openFileAction, &QAction::triggered, this, [&](){
        auto fileContent = editor->openFile();
        if(!fileContent.isEmpty())
            editor->setPlainText(fileContent);
    });
    auto exportFileAction=new QAction(QIcon(":/img/images/savefile.png"),"导出文件");
    auto saveAsAction=new QAction("另存为...");
    connect(exportFileAction,&QAction::triggered, this, [&](){
        editor->saveAs();
    });
    connect(saveAsAction,&QAction::triggered, this, [&](){
        editor->saveAs();
    });
    menu_file->addAction(openFileAction);
    menu_file->addAction(exportFileAction);
    menu_file->addAction(saveAsAction);

    QAction *act_selAll = new QAction("全选");
    QAction *act_last = new QAction("撤销");
    QAction *act_next = new QAction("恢复");
    QAction *act_c = new QAction("复制");
    QAction *act_v = new QAction("粘贴");
    QAction *act_find = new QAction("查找");

    act_selAll->setIcon(QIcon(":/image_edit/img_edit/select_all.png"));
    act_last->setIcon(QIcon(":/image_edit/img_edit/back.png"));
    act_next->setIcon(QIcon(":/image_edit/img_edit/next.png"));
    act_c->setIcon(QIcon(":/image_edit/img_edit/copy.png"));
    act_v->setIcon(QIcon(":/image_edit/img_edit/paste.png"));
    act_find->setIcon(QIcon(":/image_edit/img_edit/find.png"));

    connect(act_find,&QAction::triggered,this,&TopBar::onFindSlots);

    menu_edit->addAction(act_selAll);
    menu_edit->addAction(act_last);
    menu_edit->addAction(act_next);
    menu_edit->addAction(act_c);
    menu_edit->addAction(act_v);
    menu_edit->addAction(act_find);

    // 设置快捷键
    act_selAll->setShortcut(QKeySequence::SelectAll);  // Ctrl+A
    act_last->setShortcut(QKeySequence::Undo);       // Ctrl+Z
    act_next->setShortcut(QKeySequence::Redo);       // Ctrl+Y
    act_c->setShortcut(QKeySequence::Copy);            // Ctrl+C
    act_v->setShortcut(QKeySequence::Paste);           // Ctrl+V
    act_find->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));  // Ctrl+F (查找)

    // 连接槽函数
    connect(act_selAll, &QAction::triggered, this, &TopBar::onSelectAll);
    connect(act_last, &QAction::triggered, this, &TopBar::onUndo);
    connect(act_c, &QAction::triggered, this, &TopBar::onCopy);
    connect(act_v, &QAction::triggered, this, &TopBar::onPaste);

    QAction *act_theme = new QAction("主题");
    QAction *act_increaseFontSize = new QAction("增大字号");
    QAction *act_decreaseFontSize = new QAction("减小字号");
    QAction *act_reLoadDefaultText = new QAction("重新加载默认样式文本");
    QAction *act_resetDefaults = new QAction("恢复默认设置");

    act_theme->setShortcut(QKeySequence("Ctrl+T")); // 使用 Ctrl + T 键
    act_increaseFontSize->setShortcut(QKeySequence("Ctrl+>")); // 使用 Ctrl + >
    act_decreaseFontSize->setShortcut(QKeySequence("Ctrl+<")); // 使用 Ctrl + <
    act_reLoadDefaultText->setShortcut(QKeySequence("Ctrl+L")); // 使用 Ctrl +
    act_resetDefaults->setShortcut(QKeySequence("Ctrl+D")); // 使用 Ctrl + D

    // 连接槽函数
    connect(act_increaseFontSize, &QAction::triggered, this, &TopBar::onIncreaseFontSize);
    connect(act_decreaseFontSize, &QAction::triggered, this, &TopBar::onDecreaseFontSize);
    connect(act_reLoadDefaultText, &QAction::triggered, this, &TopBar::loadDefaultText);
    connect(act_resetDefaults, &QAction::triggered, this, &TopBar::onResetDefaults);

    menu_setting->addAction(act_theme);
    menu_setting->addAction(act_increaseFontSize);
    menu_setting->addAction(act_decreaseFontSize);
    menu_setting->addAction(act_reLoadDefaultText);
    menu_setting->addAction(act_resetDefaults);

    menu_setTheme = new MyMenu("选择主题");
    QAction *act_swanWhite = new QAction("天鹅白");
    QAction *act_inkBlack = new QAction("墨玉黑");
    QAction *act_bluePurplePink = new QAction("蓝紫粉");
    QAction *act_lightGray = new QAction("浅白灰");
    QAction *act_customTheme = new QAction("自定义主题");

    // 连接槽函数
    connect(act_swanWhite, &QAction::triggered, this, &TopBar::onSelectSwanWhite);
    connect(act_inkBlack, &QAction::triggered, this, &TopBar::onSelectInkBlack);
    connect(act_bluePurplePink, &QAction::triggered, this, &TopBar::onSelectBluePurplePink);
    connect(act_lightGray, &QAction::triggered, this, &TopBar::onSelectLightGray);
    connect(act_customTheme, &QAction::triggered, this, &TopBar::onSelectCustomTheme);

    menu_setTheme->addAction(act_swanWhite);
    menu_setTheme->addAction(act_inkBlack);
    menu_setTheme->addAction(act_bluePurplePink);
    menu_setTheme->addAction(act_lightGray);
    menu_setTheme->addAction(act_customTheme);
    menu_setting->actions().at(0)->setMenu(menu_setTheme);

    QAction *act_openColorDialog = new QAction("选择颜色");
    QAction *act_openFontDialog = new QAction("选择字体");
    QAction *act_userGuide = new QAction("使用指南");
    QAction *act_aboutUs = new QAction("关于我们");

    connect(act_userGuide, &QAction::triggered, this, &TopBar::onUseGuide);
    connect(act_aboutUs, &QAction::triggered, this, &TopBar::onAboutUs);
    act_aboutUs->setIcon(QIcon(":/img/images/about_we.png"));


    menu_help->addAction(act_openColorDialog);
    menu_help->addAction(act_openFontDialog);
    menu_help->addAction(act_userGuide);
    connect(act_openColorDialog, &QAction::triggered, this, [=](){
        QColor color = MyColorDialog::getCurrentColor(QColor(), this);
        if (color.isValid())
        {
            QString colorHex;
            if(color.alpha() == 255)
                colorHex = color.name(QColor::HexRgb);
            else
                colorHex = color.name(QColor::HexArgb); // 获取带透明度的十六进制颜色值

            QTextCursor cursor = editor->textCursor();
            cursor.insertText(colorHex);
        }
    });
    connect(act_openFontDialog, &QAction::triggered, this, [=](){
        QFontDialog *fontDialog = Config::config()->openFontDialog(this);
        if (fontDialog->exec() == QDialog::Accepted) {
            QFont font = fontDialog->selectedFont();
            if (!font.toString().isEmpty()) {
                QStringList fontPart = font.toString().split(',');
                QString fontString = fontPart[0] + " " + fontPart[1] + "pt " + fontPart[fontPart.size() - 1];

                // 获取当前编辑器
                QTextCursor cursor = editor->textCursor();
                cursor.insertText(fontString);
            }
        }
    });

    menu_about->addAction(act_aboutUs);

    // 设置按钮位置和样式
    this->setBtnPosition(btn_file, menu_file, (spacing + btn_width) * 0, "文件(&F)", ":/img/images/file.png");
    this->setBtnPosition(btn_edit, menu_edit, (spacing + btn_width) * 1, "编辑(&E)", ":/img/images/edit.png");
    this->setBtnPosition(btn_setting, menu_setting, (spacing + btn_width) * 2, "设置(&S)", ":/img/images/setting.png");
    this->setBtnPosition(btn_help, menu_help, (spacing + btn_width) * 3, "帮助(&H)", ":/img/images/help.png");
    this->setBtnPosition(btn_about, menu_about, (spacing + btn_width) * 4, "关于(&A)", ":/img/images/about.png");
}

TopBar::~TopBar()
{
}

void TopBar::setBtnPosition(QPushButton *btn, MyMenu *menu, int mx, QString text, QString iconPath)
{
    QSize size(btn_width, btn_height);
    btn->resize(size);
    btn->move(mx + leftMargin, topMargin);
    btn->setText(text);
    btn->setIcon(QIcon(iconPath));

    QFont font;
    font.setFamily("微软雅黑");
    font.setBold(true);
    font.setPixelSize(12);

    btn->setIconSize(QSize(16, 16));
    btn->setFont(font);
    btn->setMenu(menu);
}


void TopBar::onSelectAll() {
    if (editor) {
        editor->selectAll();
    }
}

void TopBar::onUndo() {
    if (editor && editor->isUndoRedoEnabled()) {
        editor->undo();
    }
}

void TopBar::onCopy() {
    if (editor) {
        editor->copy();
    }
}

void TopBar::onPaste() {
    if (editor) {
        editor->paste();
    }
}

void TopBar::getEditor(CodeEditor *editor)
{
    this->editor = editor;
}

void TopBar::setTopBarBtnToMenuStyle(const QString &style)
{
    menu_file->setStyleSheet(style);
    menu_edit->setStyleSheet(style);
    menu_setting->setStyleSheet(style);
    menu_help->setStyleSheet(style);
    menu_about->setStyleSheet(style);
    menu_setTheme->setStyleSheet(style);
}

void TopBar::onIncreaseFontSize() {
    if (editor) {
        QFont font = editor->document()->defaultFont();
        int newPointSize = font.pointSize() + 2;
        font.setPointSize(newPointSize);

        // 设置文档默认字体
        editor->document()->setDefaultFont(font);

        // 更新已有文本的格式
        QTextCursor cursor(editor->document());
        cursor.select(QTextCursor::Document);
        QTextCharFormat format;
        format.setFont(font);
        cursor.mergeCharFormat(format);
    }
}
void TopBar::onDecreaseFontSize() {
    if (editor) {
        QFont font = editor->document()->defaultFont();
        int newPointSize = font.pointSize() - 2;
        font.setPointSize(newPointSize);

        // 设置文档默认字体
        editor->document()->setDefaultFont(font);

        // 更新已有文本的格式
        QTextCursor cursor(editor->document());
        cursor.select(QTextCursor::Document);
        QTextCharFormat format;
        format.setFont(font);
        cursor.mergeCharFormat(format);
    }
}

void TopBar::loadDefaultText()
{
    Config::config()->resumeDefaultText();
}

void TopBar::onResetDefaults() {
    Config::config()->setLanFenZiStyle();
    Config::config()->resumeDefaultText();
    CurrentThemeConfig cfg;
    // 设置主题名称
    cfg.currentThemeName = "蓝紫粉";
     // 配置各部分的 JSON 文件路径
    cfg.curThemePath["currentTheme_codeeditor"] = ":/style/systemStyle/lanzifen/codeeditor.json";
    cfg.curThemePath["currentTheme_framecompo"] = ":/style/systemStyle/lanzifen/framecompo.json";
    cfg.curThemePath["currentTheme_menu"] = ":/style/systemStyle/lanzifen/menu.json";
    cfg.curThemePath["currentTheme_splittercompo"] = ":/style/systemStyle/lanzifen/splittercompo.json";
    cfg.curThemePath["currentTheme_topBar"] = ":/style/systemStyle/lanzifen/topBar.json";
    cfg.curThemePath["highlightwk_color"] = ":/style/systemStyle/lanzifen/highlightwk.json";
    cfg.curThemePath["lineNumber_style"] = ":/style/systemStyle/lanzifen/lineNumber.json";
    Config::config()->updateCurrentThemeConfig(Config::config()->iniPath, cfg);
}

void TopBar::onSelectSwanWhite() {
    // 更改背景颜色为天鹅白
    Config::config()->setCignoBlankaStyle();  // 天鹅白色
}

void TopBar::onSelectInkBlack() {
    // 更改背景颜色为墨玉黑
    Config::config()->setMoYuHeiStyle();  // 墨玉黑色
}

void TopBar::onSelectBluePurplePink() {
    // 更改背景颜色为蓝紫粉
    Config::config()->setLanFenZiStyle();  // 蓝紫粉色
}

void TopBar::onSelectLightGray() {
    // 更改背景颜色为浅白灰
    Config::config()->setQianBaiHuiStyle();    // 浅白灰色
}

void TopBar::onSelectCustomTheme()
{
    customstyle = new MyCustomStyle;
    customstyle->show();
}

void TopBar::onFindSlots()
{
    emit findBtnClicked();
}

void TopBar::onUseGuide()
{
    UseGuide useGuide;
    useGuide.exec();
}

void TopBar::onAboutUs()
{
    AboutUs aboutus;
    aboutus.exec();
}


