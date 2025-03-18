#include "mycustomstyle.h"
#include "../myColorDialog/mycolordialog.h"

#include <QDir>
#include <QFile>
#include <QFontDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QListView>
#include <QMessageBox>
#include <QClipboard>

MyCustomStyle::MyCustomStyle(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mycustomstyle)
{
    ui->setupUi(this);
    this->setWindowTitle("自定义主题");
    this->setWindowIcon(QIcon(":/img/images/theme.png"));

    isPreviewOnMainWidget = false;

    codeEditor= new CodeEditor;
    fra_right = new QFrame;
    menu = new QMenu;

    codeEditor->appendPlainText("QWidget, QPushButton, 字体颜色");
    highlighter = new Highlighter(codeEditor->document());

    // 初始化文件系统监视器
    watcher = new QFileSystemWatcher(this);

    QDir dir = Config::config()->projectPath;
    this->createUserThemeDir(dir.absolutePath(),"userTheme");

    QStringList initListData;
    QDir dir_useTheme(this->currentPreViewFileDir);
    if (dir.exists()) {
        // 获取目录中的所有子目录名称
        initListData = dir_useTheme.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    }
    watcher->addPath(this->currentPreViewFileDir);
    listModel.setStringList(initListData);
    ui->listView->setModel(&listModel);

    // 为 listView 设置右键菜单策略
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);

    // 连接右键菜单信号
    connect(ui->listView, &QListView::customContextMenuRequested,
            this, &MyCustomStyle::onListViewCustomContextMenu);

    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &MyCustomStyle::updateListView);
    // 连接信号和槽
    connect(ui->listView, &QListView::doubleClicked, this, [&](const QModelIndex &index){
        this->selectedThemeName = index.data().toString();
    });
    connect(&listModel, &QStringListModel::dataChanged, this, &MyCustomStyle::onIntoEdit);

    menu->resize(100,200);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowFlags(Qt::Popup|Qt::NoDropShadowWindowHint|Qt::FramelessWindowHint);

    QAction *action_1 = new QAction("选择1");
    QAction *action_2 = new QAction("选择2");
    QAction *action_3 = new QAction("选择3");
    QAction *action_4 = new QAction("选择4");

    ui->btn_pre_file->setMenu(menu);
    menu->addAction(action_1);
    menu->addAction(action_2);
    menu->addAction(action_3);
    menu->addAction(action_4);

    // 创建 QSplitter
    splitter = new QSplitter(ui->fra_pre_viewArea);
    splitter->setOrientation(Qt::Horizontal); // 分割条方向（横向或纵向）
    splitter->addWidget(codeEditor);
    splitter->addWidget(fra_right);

    splitter->resize(ui->fra_pre_viewArea->size());
    codeEditor->resize(splitter->width()/2, splitter->height());
    fra_right->resize(splitter->width()-codeEditor->width(), splitter->height());

    this->themeName = Config::config()->currentThemeName;
    temporary_topBarJson_qss = Config::config()->topBarJson_qss;
    temporary_menuJson_qss = Config::config()->menuJson_qss;
    temporary_codeEditorJson_qss = Config::config()->codeEditorJson_qss;
    temporary_framecompoJson_qss = Config::config()->framecompoJson_qss;
    temporary_splittercompoJson_qss = Config::config()->splittercompoJson_qss;

    this->temporaryLineNumberColor = Config::config()->lineNumberColor;
    this->temporaryKWHighlighterData = Config::config()->KWHighlighterData;

    /* 主题名单独设置 */
    old_themeName = Config::config()->currentThemeName;
    this->upDateOldTheme();

    this->setCurrentTheme();
    this->initAllControl();
    this->themeName = Config::config()->currentThemeName;

}

// 设置当前预览窗口的主题
void MyCustomStyle::setCurrentTheme()
{
    //设置样式
    ui->wid_pre_titleBar->setStyleSheet(temporary_topBarJson_qss.themeQssData);
    this->menu->setStyleSheet(temporary_menuJson_qss.themeQssData);

    this->codeEditor->setStyleSheet(temporary_codeEditorJson_qss.themeQssData);
    Config::config()->setLineNumberStyle(this->codeEditor,
                                temporaryLineNumberColor.selectedColor,
                                temporaryLineNumberColor.noSelectedColor,
                                temporaryLineNumberColor.backgroundColor);

    this->fra_right->setStyleSheet(temporary_framecompoJson_qss.themeQssData);
    this->splitter->setStyleSheet(temporary_splittercompoJson_qss.themeQssData);
    Config::config()->updateAllKWHighlight(this->highlighter);
}

void MyCustomStyle::initAllControl()
{
    // 初始化菜单栏背景颜色按钮
    QString colorStr = this->getJsonValue(this->temporary_topBarJson_qss.themeJsonData, "QWidget", "background-color");
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_bgdcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_bgdcol->setStyleSheet(btn_style);

    // 初始化菜单栏按钮悬浮背景颜色按钮
    colorStr = this->getJsonValue(this->temporary_topBarJson_qss.themeJsonData, "QPushButton:hover", "background-color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_hoverbgcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_hoverbgcol->setStyleSheet(btn_style);

    // 初始化菜单栏字体颜色按钮
    colorStr = this->getJsonValue(this->temporary_topBarJson_qss.themeJsonData, "QPushButton", "color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_fontcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_fontcol->setStyleSheet(btn_style);

    // 初始化菜单栏字体样式按钮
    QString fontFamily = this->getJsonValue(this->temporary_topBarJson_qss.themeJsonData, "QPushButton", "font-family");
    QString fontSize = this->getJsonValue(this->temporary_topBarJson_qss.themeJsonData, "QPushButton", "font-size");
    QString fontWeight = this->getJsonValue(this->temporary_topBarJson_qss.themeJsonData, "QPushButton", "font-weight");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_fontStyle->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "font-family", fontFamily);
    this->modifyJsonValue(btn_style, "QPushButton", "font-size", fontSize);
    this->modifyJsonValue(btn_style, "QPushButton", "font-weight", fontWeight);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_fontStyle->setStyleSheet(btn_style);

    // 初始化菜单基础背景按钮
    colorStr = this->getJsonValue(this->temporary_menuJson_qss.themeJsonData, "QMenu", "background-color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_bg_col->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_bg_col->setStyleSheet(btn_style);

    // 初始化菜单悬浮背景按钮
    colorStr = this->getJsonValue(this->temporary_menuJson_qss.themeJsonData, "QMenu::item:selected", "background-color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_hoverbgcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_hoverbgcol->setStyleSheet(btn_style);

    // 初始化菜单基础字体颜色按钮
    colorStr = this->getJsonValue(this->temporary_menuJson_qss.themeJsonData, "QMenu::item", "color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_fontcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_fontcol->setStyleSheet(btn_style);

    // 初始化菜单悬浮字体颜色按钮
    colorStr = this->getJsonValue(this->temporary_menuJson_qss.themeJsonData, "QMenu::item:selected", "color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_fonthovercol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_fonthovercol->setStyleSheet(btn_style);

    // 初始化菜单字体样式按钮
    fontFamily = this->getJsonValue(this->temporary_menuJson_qss.themeJsonData, "QMenu", "font-family");
    fontSize = this->getJsonValue(this->temporary_menuJson_qss.themeJsonData, "QMenu", "font-size");
    fontWeight = this->getJsonValue(this->temporary_menuJson_qss.themeJsonData, "QMenu", "font-weight");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_fontStyle->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "font-family", fontFamily);
    this->modifyJsonValue(btn_style, "QPushButton", "font-size", fontSize);
    this->modifyJsonValue(btn_style, "QPushButton", "font-weight", fontWeight);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_fontStyle->setStyleSheet(btn_style);

    // 初始化编辑器的基础背景颜色按钮
    colorStr = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QWidget", "background-color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_bgdcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_edit_bgdcol->setStyleSheet(btn_style);

    // 初始化编辑器的字体颜色按钮
    colorStr = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QWidget", "color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_fontcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_edit_fontcol->setStyleSheet(btn_style);

    // 初始化编辑器的字体样式按钮
    fontFamily = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QWidget", "font-family");
    fontSize = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QWidget", "font-size");
    fontWeight = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QWidget", "font-weight");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_fontstyle->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "font-family", fontFamily);
    this->modifyJsonValue(btn_style, "QPushButton", "font-size", fontSize);
    this->modifyJsonValue(btn_style, "QPushButton", "font-weight", fontWeight);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_edit_fontstyle->setStyleSheet(btn_style);

    // 初始化行号区域背景颜色按钮
    colorStr = temporaryLineNumberColor.backgroundColor.name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_line_num_bgdcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_line_num_bgdcol->setStyleSheet(btn_style);

    // 初始化行号的基础字体颜色按钮
    colorStr = temporaryLineNumberColor.noSelectedColor.name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_line_fontcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_line_fontcol->setStyleSheet(btn_style);

    // 初始化行号被选中颜色按钮
    colorStr = temporaryLineNumberColor.selectedColor.name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_line_sel_fontcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_line_sel_fontcol->setStyleSheet(btn_style);

    // 初始化关键字高亮--主控件按钮
    colorStr = temporaryKWHighlighterData.highlighterColors["widgetFormat_color"].name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_main_control->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_main_control->setStyleSheet(btn_style);

    // 初始化关键字高亮--伪控件按钮
    colorStr = temporaryKWHighlighterData.highlighterColors["pseudoStateFormat_color"].name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_fake_states->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_fake_states->setStyleSheet(btn_style);

    // 初始化关键字高亮--子控件按钮
    colorStr = temporaryKWHighlighterData.highlighterColors["subControlFormat_color"].name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_chidren_control->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_chidren_control->setStyleSheet(btn_style);

    // 初始化关键字高亮--样式属性按钮
    colorStr = temporaryKWHighlighterData.highlighterColors["propertyFormat_color"].name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_style_attri->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_style_attri->setStyleSheet(btn_style);

    // 初始化关键字高亮--布局属性按钮
    colorStr = temporaryKWHighlighterData.highlighterColors["layoutFormat_color"].name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_lay_attri->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_lay_attri->setStyleSheet(btn_style);

    // 初始化关键字高亮--渐变属性按钮
    colorStr = temporaryKWHighlighterData.highlighterColors["gradientFormat_color"].name(QColor::HexArgb);
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_opacity_attri->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_opacity_attri->setStyleSheet(btn_style);
}

// 修改json数据
bool MyCustomStyle::modifyJsonValue(QString &jsonString, const QString &object, const QString &property, const QString &newValue)
{
    // 将 JSON 字符串转换为 QByteArray
    QByteArray jsonData = jsonString.toUtf8();

    // 解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject obj;

    // 如果指定对象不存在或不是对象，则创建一个新的对象
    if (jsonObj.contains(object) && jsonObj[object].isObject()) {
        obj = jsonObj[object].toObject();
    } else {
        obj = QJsonObject();
    }

    // 修改（或添加）指定属性的值
    obj[property] = newValue;
    jsonObj[object] = obj;

    // 将修改后的 JSON 数据转换回字符串
    jsonDoc.setObject(jsonObj);
    jsonString = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
    return true;
}

// 修改颜色数据
bool MyCustomStyle::modifyColorValue(QString &jsonString, const QString &object, const QColor &newColor)
{
    // 将 JSON 字符串转换为 QByteArray
    QByteArray jsonData = jsonString.toUtf8();

    // 解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON解析错误:" << parseError.errorString();
        return false;
    }

    QJsonObject rootObj = jsonDoc.object();

    // 构造颜色对象，赋予新的 r, g, b 值，并将 a 固定为 255
    QJsonObject colorObj;
    colorObj["r"] = newColor.red();
    colorObj["g"] = newColor.green();
    colorObj["b"] = newColor.blue();
    colorObj["a"] = 255;

    // 直接设置目标对象对应的颜色（如果该对象不存在则自动新增）
    rootObj[object] = colorObj;

    // 将修改后的 JSON 数据转换回字符串（紧凑格式）
    jsonDoc.setObject(rootObj);
    jsonString = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
    return true;
}

// 获取json值
QString MyCustomStyle::getJsonValue(const QString &jsonString, const QString &object, const QString &property)
{
    // 将 JSON 字符串转换为 QByteArray
    QByteArray jsonData = jsonString.toUtf8();

    // 解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        // qWarning() << "JSON parse error:" << parseError.errorString();
        return QString();
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 获取指定对象和属性的值
    if (jsonObj.contains(object) && jsonObj[object].isObject()) {
        QJsonObject obj = jsonObj[object].toObject();
        if (obj.contains(property)) {
            return obj[property].toString();
        } else {
            // qWarning() << "Property not found in JSON object.";
            return QString();
        }
    } else {
        // qWarning() << "Object not found in JSON.";
        return QString();
    }
}

// 设置主题名称
bool MyCustomStyle::setThemeName()
{
    // 获取 QListView 的模型
    QAbstractItemModel *model = ui->listView->model();
    QString themeNum = QString::number(model->rowCount() + 1);

    bool ok;
    do {
        themeName = QInputDialog::getText(nullptr, tr("保存主题"),
                                          tr("请输入主题名称:"), QLineEdit::Normal,
                                          "我的主题" + themeNum, &ok);
        if (!ok || themeName.isEmpty()) {
            qWarning() << "用户取消或未输入主题名称";
            return false;
        }
        QDir dir(this->currentPreViewFileDir + "/" + themeName);

        if (dir.exists()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(nullptr, tr("主题已存在"),
                                          tr("主题已存在，是否覆盖？"),
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                break; // 用户选择覆盖，跳出循环
            }
        } else {
            dir.mkpath("."); // 创建目录
            break; // 目录不存在，已创建，跳出循环
        }
    } while (true);
    return true;
}

void MyCustomStyle::upDateOldTheme()
{
    /*初始化旧的主题*/
    old_temporary_topBarJson_qss = temporary_topBarJson_qss;
    old_temporary_menuJson_qss = temporary_menuJson_qss;
    old_temporary_codeEditorJson_qss = temporary_codeEditorJson_qss;
    old_temporary_framecompoJson_qss = temporary_framecompoJson_qss;
    old_temporary_splittercompoJson_qss = temporary_splittercompoJson_qss;

    this->old_temporaryLineNumberColor = temporaryLineNumberColor;
    this->old_temporaryKWHighlighterData = temporaryKWHighlighterData;
    /* */
}

// 创建用户的自定义主题目录
void MyCustomStyle::createUserThemeDir(const QString &projectDir, const QString &folderName)
{
    this->projectDir = projectDir;
    this->folderName = folderName;

    QDir dir(projectDir + "/" + folderName);

    // 如果文件夹不存在，则创建
    if (!dir.exists())
        dir.mkpath(".");

    this->currentPreViewFileDir = dir.absolutePath();
}

// 保存用户主题文件
bool MyCustomStyle::saveUserThemeFile(const QStringList &fileNameDesc, CurrentThemeConfig &temConfig)
{
    static const QRegularExpression reg("[<>:\"/\\|?*]");
    int i=0;
    foreach (QString key, this->keyOrder){
        key.replace(reg, "_");
        filePath = this->currentPreViewFileDir + "/" + themeName + "/" + key;

        QFile file(filePath);
        // 打开文件，如果文件不存在则创建文件
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            // qWarning() << "无法打开文件进行写入：" << file.errorString();
            return false;
        }
        file.write(this->saveJsonData[key].toUtf8()); // 写入文件
        file.close();
        temConfig.curThemePath[fileNameDesc[i]] = filePath;
        i++;
    }
    // qDebug() << "文件已创建或覆盖：" << themeName;
    return true;
}

void MyCustomStyle::onIntoEdit(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(bottomRight);
    if (roles.contains(Qt::EditRole)) {
        this->newThemeName = listModel.data(topLeft, Qt::EditRole).toString();
        // 查找并重命名目录
        QString directoryPath = this->currentPreViewFileDir;
        QDir dir(directoryPath);
        if (dir.exists(selectedThemeName)) {
            if (dir.rename(selectedThemeName, newThemeName)) {
                this->renameThemeToUpdateIniFile();
                // qDebug() << "目录重命名成功：" << selectedThemeName << " -> " << newThemeName;
            } else {
                // qWarning() << "目录重命名失败：" << selectedThemeName << " -> " << newThemeName;
            }
        } else {
            qWarning() << "目录不存在：" << selectedThemeName;
        }
    }
}

void MyCustomStyle::updateListView()
{
    QStringList initListData;
    QDir dir_useTheme(currentPreViewFileDir);
    if (!dir_useTheme.exists()) {
        qWarning() << "目录不存在：" << currentPreViewFileDir;
    } else {
        // 获取目录中的所有子目录名称
        initListData = dir_useTheme.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    }

    listModel.setStringList(initListData);
    ui->listView->setModel(&listModel);
}

MyCustomStyle::~MyCustomStyle()
{
    delete ui;
}

// 列表右击菜单逻辑
void MyCustomStyle::onListViewCustomContextMenu(const QPoint &pos)
{
    // 获取鼠标右键点击位置对应的全局坐标
    QPoint globalPos = ui->listView->viewport()->mapToGlobal(pos);
    QModelIndex index = ui->listView->indexAt(pos);

    // 获取当前选中项的字符串（主题名称）
    QString currentStr = ui->listView->currentIndex().data().toString();

    // 创建菜单
    QMenu menu;
    menu.setAttribute(Qt::WA_TranslucentBackground);
    menu.setWindowFlags(Qt::Popup|Qt::NoDropShadowWindowHint|Qt::FramelessWindowHint);
    menu.setStyleSheet(temporary_menuJson_qss.themeQssData);
    QAction *deleteAct = menu.addAction("删除");
    QAction *renameAct = menu.addAction("重命名");
    QAction *copyAct   = menu.addAction("复制");
    QAction *pasteAct  = menu.addAction("粘贴");
    QAction *preview  = menu.addAction("在预览区预览");
    QAction *applyTheme  = menu.addAction("应用主题");

    // 如果没有选中项，则禁用删除、重命名和复制操作
    if (!index.isValid()) {
        deleteAct->setEnabled(false);
        renameAct->setEnabled(false);
        copyAct->setEnabled(false);
    }

    QAction *selectedAction = menu.exec(globalPos);
    if (!selectedAction)
        return;

    // 删除操作：删除选中的主题目录，并更新列表
    if (selectedAction == deleteAct) {
        QString themeNameStr = listModel.data(index).toString();
        QString dirPath = currentPreViewFileDir + "/" + themeNameStr;
        if(Config::config()->curThemeConfig.currentThemeName == themeNameStr){
            QMessageBox::warning(this, tr("删除失败"), tr("主题应用中，请取消应用后再删除！"));
            return;
        }
        QDir dir(dirPath);
        if (dir.exists()) {
            if (dir.removeRecursively()) {
                qDebug() << "删除成功：" << themeNameStr;
                updateListView();
            } else {
                QMessageBox::warning(this, tr("删除失败"), tr("删除主题失败：%1").arg(themeNameStr));
            }
        } else {
            QMessageBox::warning(this, tr("目录不存在"), tr("主题目录不存在：%1").arg(themeNameStr));
        }
    }
    // 重命名操作：弹出输入对话框修改主题名称，并重命名目录
    else if (selectedAction == renameAct) {
        selectedThemeName = listModel.data(index).toString();
        bool ok = false;
        newThemeName = QInputDialog::getText(this, tr("重命名"),
                                                tr("请输入新的主题名称:"), QLineEdit::Normal,
                                                selectedThemeName, &ok);
        if (ok && !newThemeName.isEmpty() && selectedThemeName != newThemeName) {
            QDir dir(currentPreViewFileDir);
            if (dir.exists(newThemeName)) {
                QMessageBox::warning(this, tr("重命名"), tr("主题名称已存在！"));
            } else {
                if (dir.rename(selectedThemeName, newThemeName)) {
                    // 如果重命名的主题为应用且保存的，则相应更新主题名称
                    this->renameThemeToUpdateIniFile();
                    qDebug() << "重命名成功：" << selectedThemeName << "->" << newThemeName;
                    updateListView();
                } else {
                    QMessageBox::warning(this, tr("重命名失败"), tr("无法重命名主题：%1").arg(selectedThemeName));
                }
            }
        }
    }
    // 复制操作：将选中主题名称复制到剪贴板
    else if (selectedAction == copyAct) {
        QString themeName = listModel.data(index).toString();
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(themeName);
        qDebug() << "已复制主题名称：" << themeName;
    }
    // 粘贴操作：从剪贴板中获取主题名称，并复制该主题目录到一个新的目录
    else if (selectedAction == pasteAct) {
        QClipboard *clipboard = QGuiApplication::clipboard();
        QString sourceTheme = clipboard->text();
        if (sourceTheme.isEmpty()) {
            QMessageBox::information(this, tr("粘贴"), tr("剪贴板中没有内容"));
            return;
        }
        QDir dir(currentPreViewFileDir);
        if (!dir.exists(sourceTheme)) {
            QMessageBox::warning(this, tr("粘贴"), tr("主题 '%1' 不存在，无法复制").arg(sourceTheme));
            return;
        }
        // 生成新的主题名称
        QString newTheme = sourceTheme + "_copy";
        if (dir.exists(newTheme)) {
            newTheme += QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        }
        QString srcPath = currentPreViewFileDir + "/" + sourceTheme;
        QString dstPath = currentPreViewFileDir + "/" + newTheme;
        if (copyDirectoryRecursively(srcPath, dstPath)) {
            qDebug() << "复制成功，新主题：" << newTheme;
            updateListView();
        } else {
            QMessageBox::warning(this, tr("粘贴"), tr("复制主题失败"));
        }
    }
    else if(selectedAction == preview){
        if(currentStr != tempCurrentThemeConfig.currentThemeName){
            this->preview_theme(currentStr); // 先获取这个主题的数据
        }
        // qDebug() << "selectedAction == preview" << Config::config()->currentThemeName;
        this->setCurrentTheme();  // 显示预览
        this->isPreviewOnMainWidget = true;
    }
    else if(selectedAction == applyTheme){
        if(currentStr != tempCurrentThemeConfig.currentThemeName && currentStr != Config::config()->currentThemeName){
            this->preview_theme(currentStr); // 先获取这个主题的数据
        }
        /* 应用该主题，将这个主题设为旧主题，主题名单独赋值 */
        this->old_themeName = tempCurrentThemeConfig.currentThemeName;
        this->upDateOldTheme();

        // 应用主题并修改ini的主题配置文件，以便下一次打开后主题为此时应用的这个主题
        Config::config()->curThemeConfig = tempCurrentThemeConfig;
        this->on_btn_preview_on_mainWid_clicked();
    }
}

// 菜单栏背景颜色
void MyCustomStyle::on_btn_tit_bgdcol_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_bgdcol->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = temporary_topBarJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QWidget", "background-color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    ui->wid_pre_titleBar->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_bgdcol->setStyleSheet(btn_style);

    temporary_topBarJson_qss.themeQssData = style;
    temporary_topBarJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();

}

// 菜单栏按钮悬浮背景颜色
void MyCustomStyle::on_btn_tit_hoverbgcol_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_hoverbgcol->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = temporary_topBarJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QPushButton:hover", "background-color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    ui->wid_pre_titleBar->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_hoverbgcol->setStyleSheet(btn_style);

    temporary_topBarJson_qss.themeQssData = style;
    temporary_topBarJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
}

// 菜单栏字体颜色
void MyCustomStyle::on_btn_tit_fontcol_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_fontcol->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = temporary_topBarJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QPushButton", "color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    ui->wid_pre_titleBar->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_fontcol->setStyleSheet(btn_style);

    temporary_topBarJson_qss.themeQssData = style;
    temporary_topBarJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
}

// 菜单栏字体样式
void MyCustomStyle::on_btn_tit_fontStyle_clicked()
{
    QFontDialog *fontDialog = Config::config()->openFontDialog(this);

    if (fontDialog->exec() == QDialog::Accepted){
        QFont font = fontDialog->selectedFont();
        QStringList fontPart = font.toString().split(',');

        QString style = temporary_topBarJson_qss.themeJsonData;
        this->modifyJsonValue(style, "QPushButton", "font-family", fontPart[0]);
        this->modifyJsonValue(style, "QPushButton", "font-size", fontPart[1]+"pt");
        this->modifyJsonValue(style, "QPushButton", "font-weight", fontPart[fontPart.size()-1]);
        style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
        ui->wid_pre_titleBar->setStyleSheet(style);

        QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_fontStyle->styleSheet());
        this->modifyJsonValue(btn_style, "QPushButton", "font-family", fontPart[0]);
        this->modifyJsonValue(btn_style, "QPushButton", "font-size", fontPart[1]+"pt");
        this->modifyJsonValue(btn_style, "QPushButton", "font-weight", fontPart[fontPart.size()-1]);

        btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
        ui->btn_tit_fontStyle->setStyleSheet(btn_style);

        temporary_topBarJson_qss.themeQssData = style;
        temporary_topBarJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
    }
}

// 菜单基础背景
void MyCustomStyle::on_btn_tit_menu_bg_col_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_bg_col->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = this->temporary_menuJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QMenu", "background-color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    this->menu->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_bg_col->setStyleSheet(btn_style);

    temporary_menuJson_qss.themeQssData = style;
    temporary_menuJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
}

// 菜单悬浮背景
void MyCustomStyle::on_btn_tit_menu_hoverbgcol_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_hoverbgcol->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = this->temporary_menuJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QMenu::item:selected", "background-color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    this->menu->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_hoverbgcol->setStyleSheet(btn_style);

    temporary_menuJson_qss.themeQssData = style;
    temporary_menuJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
}

// 菜单基础字体颜色
void MyCustomStyle::on_btn_tit_menu_fontcol_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_fontcol->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = this->temporary_menuJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QMenu::item", "color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    this->menu->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_fontcol->setStyleSheet(btn_style);

    temporary_menuJson_qss.themeQssData = style;
    temporary_menuJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
}

// 菜单悬浮字体颜色
void MyCustomStyle::on_btn_tit_menu_fonthovercol_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_fonthovercol->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = this->temporary_menuJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QMenu::item:selected", "color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    this->menu->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_menu_fonthovercol->setStyleSheet(btn_style);

    temporary_menuJson_qss.themeQssData = style;
    temporary_menuJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
}

// 菜单字体样式
void MyCustomStyle::on_btn_tit_menu_fontStyle_clicked()
{
    QFontDialog *fontDialog = Config::config()->openFontDialog(this);

    if (fontDialog->exec() == QDialog::Accepted) {
        QFont font = fontDialog->selectedFont();
        QStringList fontPart = font.toString().split(',');
        QString style = this->temporary_menuJson_qss.themeJsonData;
        this->modifyJsonValue(style, "QMenu", "font-family", fontPart[0]);
        this->modifyJsonValue(style, "QMenu", "font-size", fontPart[1] + "pt");
        this->modifyJsonValue(style, "QMenu", "font-weight", fontPart[fontPart.size() - 1]);
        style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
        this->menu->setStyleSheet(style);

        QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_menu_fontStyle->styleSheet());
        this->modifyJsonValue(btn_style, "QPushButton", "font-family", fontPart[0]);
        this->modifyJsonValue(btn_style, "QPushButton", "font-size", fontPart[1] + "pt");
        this->modifyJsonValue(btn_style, "QPushButton", "font-weight", fontPart[fontPart.size() - 1]);
        btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
        ui->btn_tit_menu_fontStyle->setStyleSheet(btn_style);

        temporary_menuJson_qss.themeQssData = style;
        temporary_menuJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
    }
}

// 编辑器的基础背景颜色
void MyCustomStyle::on_btn_edit_bgdcol_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_bgdcol->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = this->temporary_codeEditorJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QWidget", "background-color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    this->codeEditor->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_edit_bgdcol->setStyleSheet(btn_style);

    temporary_codeEditorJson_qss.themeQssData = style;
    temporary_codeEditorJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
}

// 编辑器的字体颜色
void MyCustomStyle::on_btn_edit_fontcol_clicked()
{
    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_fontcol->styleSheet());
    QColor color = this->getJsonValue(btn_style, "QPushButton", "background-color");
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    QString style = this->temporary_codeEditorJson_qss.themeJsonData;
    this->modifyJsonValue(style, "QWidget", "color", color.name(QColor::HexArgb));
    style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
    this->codeEditor->setStyleSheet(style);

    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_edit_fontcol->setStyleSheet(btn_style);

    temporary_codeEditorJson_qss.themeQssData = style;
    temporary_codeEditorJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
}

// 编辑器的字体样式
void MyCustomStyle::on_btn_edit_fontstyle_clicked()
{
    QFontDialog *fontDialog = Config::config()->openFontDialog(this);
    if (fontDialog->exec() == QDialog::Accepted) {
        QFont font = fontDialog->selectedFont();
        QStringList fontPart = font.toString().split(',');
        QString style = this->temporary_codeEditorJson_qss.themeJsonData;
        this->modifyJsonValue(style, "QWidget", "font-family", fontPart[0]);
        this->modifyJsonValue(style, "QWidget", "font-size", fontPart[1] + "pt");
        this->modifyJsonValue(style, "QWidget", "font-weight", fontPart[fontPart.size() - 1]);
        style = JsonParse::getJsonParse()->parseThemeJsontoQss(style);
        this->codeEditor->setStyleSheet(style);

        QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_fontstyle->styleSheet());
        this->modifyJsonValue(btn_style, "QPushButton", "font-family", fontPart[0]);
        this->modifyJsonValue(btn_style, "QPushButton", "font-size", fontPart[1] + "pt");
        this->modifyJsonValue(btn_style, "QPushButton", "font-weight", fontPart[fontPart.size() - 1]);
        btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
        ui->btn_edit_fontstyle->setStyleSheet(btn_style);

        temporary_codeEditorJson_qss.themeQssData = style;
        temporary_codeEditorJson_qss.themeJsonData = JsonParse::getJsonParse()->qssToJson(style).toUtf8();
    }
}

// 行号区域背景颜色
void MyCustomStyle::on_btn_line_num_bgdcol_clicked()
{
    QColor color = temporaryLineNumberColor.backgroundColor;
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryLineNumberColor.backgroundColor = color;
    this->codeEditor->setLineNumberBackgroundColor(color);

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_fontstyle->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_edit_fontstyle->setStyleSheet(btn_style);
}

// 行号的基础字体颜色
void MyCustomStyle::on_btn_line_fontcol_clicked()
{
    QColor color = temporaryLineNumberColor.noSelectedColor;
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryLineNumberColor.noSelectedColor = color;
    this->codeEditor->setLineNumberNoSelectedColor(color);

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_line_fontcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_line_fontcol->setStyleSheet(btn_style);
}

// 行号被选中颜色
void MyCustomStyle::on_btn_line_sel_fontcol_clicked()
{
    QColor color = temporaryLineNumberColor.selectedColor;
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryLineNumberColor.selectedColor = color;
    this->codeEditor->setLineNumberSelectedColor(color);

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_line_sel_fontcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_line_sel_fontcol->setStyleSheet(btn_style);
}

// 关键字高亮--主控件
void MyCustomStyle::on_btn_kw_main_control_clicked()
{
    QColor color = temporaryKWHighlighterData.highlighterColors["widgetFormat_color"];
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryKWHighlighterData.highlighterColors["widgetFormat_color"] = color;
    QString str = temporaryKWHighlighterData.jsonData;
    this->modifyJsonValue(str, "commentFormat_color", "r", QString::number(color.red()));
    this->modifyJsonValue(str, "commentFormat_color", "g", QString::number(color.red()));
    this->modifyJsonValue(str, "commentFormat_color", "b", QString::number(color.red()));

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_main_control->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_main_control->setStyleSheet(btn_style);

    this->highlighter->setWidgetFormat_color(color);
}

// 关键字高亮--伪控件
void MyCustomStyle::on_btn_kw_fake_states_clicked()
{
    QColor color = temporaryKWHighlighterData.highlighterColors["pseudoStateFormat_color"];
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryKWHighlighterData.highlighterColors["pseudoStateFormat_color"] = color;
    QString str = temporaryKWHighlighterData.jsonData;
    this->modifyJsonValue(str, "pseudoStateFormat_color", "r", QString::number(color.red()));
    this->modifyJsonValue(str, "pseudoStateFormat_color", "g", QString::number(color.green()));
    this->modifyJsonValue(str, "pseudoStateFormat_color", "b", QString::number(color.blue()));
    this->highlighter->setPseudoStateFormat_color(color);

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_fake_states->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_fake_states->setStyleSheet(btn_style);
}

// 关键字高亮--子控件
void MyCustomStyle::on_btn_kw_chidren_control_clicked()
{
    QColor color = temporaryKWHighlighterData.highlighterColors["subControlFormat_color"];
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryKWHighlighterData.highlighterColors["subControlFormat_color"] = color;
    QString str = temporaryKWHighlighterData.jsonData;
    this->modifyJsonValue(str, "subControlFormat_color", "r", QString::number(color.red()));
    this->modifyJsonValue(str, "subControlFormat_color", "g", QString::number(color.green()));
    this->modifyJsonValue(str, "subControlFormat_color", "b", QString::number(color.blue()));
    this->highlighter->setSubControlFormat_color(color);

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_chidren_control->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_chidren_control->setStyleSheet(btn_style);
}

// 关键字高亮--样式属性
void MyCustomStyle::on_btn_kw_style_attri_clicked()
{
    QColor color = temporaryKWHighlighterData.highlighterColors["propertyFormat_color"];
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryKWHighlighterData.highlighterColors["propertyFormat_color"] = color;
    QString str = temporaryKWHighlighterData.jsonData;
    this->modifyJsonValue(str, "propertyFormat_color", "r", QString::number(color.red()));
    this->modifyJsonValue(str, "propertyFormat_color", "g", QString::number(color.green()));
    this->modifyJsonValue(str, "propertyFormat_color", "b", QString::number(color.blue()));
    this->highlighter->setPropertyFormat_color(color);

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_style_attri->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_style_attri->setStyleSheet(btn_style);
}

// 关键字高亮--布局属性
void MyCustomStyle::on_btn_kw_lay_attri_clicked()
{
    QColor color = temporaryKWHighlighterData.highlighterColors["layoutFormat_color"];
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryKWHighlighterData.highlighterColors["layoutFormat_color"] = color;
    QString str = temporaryKWHighlighterData.jsonData;
    this->modifyJsonValue(str, "layoutFormat_color", "r", QString::number(color.red()));
    this->modifyJsonValue(str, "layoutFormat_color", "g", QString::number(color.green()));
    this->modifyJsonValue(str, "layoutFormat_color", "b", QString::number(color.blue()));
    this->highlighter->setLayoutFormat_color(color);

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_lay_attri->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_lay_attri->setStyleSheet(btn_style);
}

// 关键字高亮--渐变属性
void MyCustomStyle::on_btn_kw_opacity_attri_clicked()
{
    QColor color = temporaryKWHighlighterData.highlighterColors["gradientFormat_color"];
    color = MyColorDialog::getCurrentColor(color, this);
    if (!color.isValid()) {
        return;
    }

    temporaryKWHighlighterData.highlighterColors["gradientFormat_color"] = color;
    QString str = temporaryKWHighlighterData.jsonData;
    this->modifyJsonValue(str, "gradientFormat_color", "r", QString::number(color.red()));
    this->modifyJsonValue(str, "gradientFormat_color", "g", QString::number(color.green()));
    this->modifyJsonValue(str, "gradientFormat_color", "b", QString::number(color.blue()));
    this->highlighter->setGradientFormat_color(color);

    QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_kw_opacity_attri->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "color", color.name(QColor::HexArgb));
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_kw_opacity_attri->setStyleSheet(btn_style);
}

void MyCustomStyle::updateThemeValues()
{
    Config::config()->topBarJson_qss = temporary_topBarJson_qss;
    Config::config()->menuJson_qss = temporary_menuJson_qss,
    Config::config()->codeEditorJson_qss = temporary_codeEditorJson_qss,
    Config::config()->framecompoJson_qss = temporary_framecompoJson_qss,
    Config::config()->splittercompoJson_qss = temporary_splittercompoJson_qss;

    Config::config()->KWHighlighterData = temporaryKWHighlighterData;
    Config::config()->lineNumberColor = temporaryLineNumberColor;
}

// 辅助函数，递归复制目录
bool MyCustomStyle::copyDirectoryRecursively(const QString &srcPath, const QString &dstPath)
{
    QDir sourceDir(srcPath);
    if (!sourceDir.exists())
        return false;

    QDir targetDir(dstPath);
    if (!targetDir.exists()) {
        if (!targetDir.mkpath("."))
            return false;
    }

    // 复制文件
    foreach (const QString &fileName, sourceDir.entryList(QDir::Files)) {
        QString srcFilePath = srcPath + "/" + fileName;
        QString dstFilePath = dstPath + "/" + fileName;
        if (!QFile::copy(srcFilePath, dstFilePath)) {
            return false;
        }
    }

    // 复制子目录
    foreach (const QString &dirName, sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString srcDirPath = srcPath + "/" + dirName;
        QString dstDirPath = dstPath + "/" + dirName;
        if (!copyDirectoryRecursively(srcDirPath, dstDirPath))
            return false;
    }
    return true;
}

void MyCustomStyle::renameThemeToUpdateIniFile()
{
    if(Config::config()->curThemeConfig.currentThemeName == selectedThemeName){
        QSettings settings(Config::config()->iniPath, QSettings::IniFormat);
        settings.beginGroup("currentThemeStyle");
        settings.setValue("name", newThemeName);
        settings.setValue("currentTheme_codeeditor",currentPreViewFileDir+"/"+newThemeName+"/codeeditor.json");
        settings.setValue("currentTheme_framecompo",currentPreViewFileDir+"/"+newThemeName+"/framecompo.json");
        settings.setValue("currentTheme_menu",currentPreViewFileDir+"/"+newThemeName+"/menu.json");
        settings.setValue("currentTheme_splittercompo",currentPreViewFileDir+"/"+newThemeName+"/splittercompo.json");
        settings.setValue("currentTheme_topBar",currentPreViewFileDir+"/"+newThemeName+"/topBar.json");
        settings.setValue("highlightwk_color",currentPreViewFileDir+"/"+newThemeName+"/highlightwk.json");
        settings.setValue("lineNumber_style",currentPreViewFileDir+"/"+newThemeName+"/lineNumber.json");
        settings.endGroup();
    }
}

// 保存并应用
void MyCustomStyle::on_btn_save_apply_clicked()
{
    if(!this->setThemeName()){  // 设置主题名
        return;
    }
    /* 把对应的颜色写入isonData */
    this->modifyColorValue(temporaryLineNumberColor.jsonData, "selectedColor",
                           temporaryLineNumberColor.selectedColor);
    this->modifyColorValue(temporaryLineNumberColor.jsonData, "noSelectedColor",
                           temporaryLineNumberColor.noSelectedColor);
    this->modifyColorValue(temporaryLineNumberColor.jsonData, "backgroundColor",
                           temporaryLineNumberColor.backgroundColor);

    /*更新主配置变量的值 */
    this->updateThemeValues();

    // 构建保存数据
    saveJsonData["codeeditor.json"] =  temporary_codeEditorJson_qss.themeJsonData;
    saveJsonData["framecompo.json"] =  temporary_framecompoJson_qss.themeJsonData;
    saveJsonData["menu.json"] =  temporary_menuJson_qss.themeJsonData;
    saveJsonData["splittercompo.json"] =  temporary_splittercompoJson_qss.themeJsonData;
    saveJsonData["topBar.json"] =  temporary_topBarJson_qss.themeJsonData;
    saveJsonData["highlightwk.json"] =  temporaryKWHighlighterData.jsonData;
    saveJsonData["lineNumber.json"] =  temporaryLineNumberColor.jsonData;

    keyOrder << "codeeditor.json" << "framecompo.json" << "menu.json" << "splittercompo.json"
             << "topBar.json" << "highlightwk.json" << "lineNumber.json";

    CurrentThemeConfig myThemeCfg;
    myThemeCfg.currentThemeName = this->themeName;

    // 应用该主题，将这个主题设为旧主题，主题名单独赋值
    this->old_themeName = this->themeName;
    this->upDateOldTheme();

    QStringList fileNameDesc{"currentTheme_codeeditor", "currentTheme_framecompo",
                             "currentTheme_menu", "currentTheme_splittercompo",
                             "currentTheme_topBar", "highlightwk_color","lineNumber_style"};

    this->saveUserThemeFile(fileNameDesc, myThemeCfg);
    Config::config()->curThemeConfig = myThemeCfg;
    Config::config()->updateCurrentThemeStyle();
}

// 仅保存
void MyCustomStyle::on_btn_onlysave_clicked()
{
    if(!this->setThemeName()){  // 设置主题名
        return;
    }
    this->modifyColorValue(temporaryLineNumberColor.jsonData, "selectedColor",
                           temporaryLineNumberColor.selectedColor);
    this->modifyColorValue(temporaryLineNumberColor.jsonData, "noSelectedColor",
                           temporaryLineNumberColor.noSelectedColor);
    this->modifyColorValue(temporaryLineNumberColor.jsonData, "backgroundColor",
                           temporaryLineNumberColor.backgroundColor);

    this->updateThemeValues();

    saveJsonData["codeeditor.json"] =  temporary_codeEditorJson_qss.themeJsonData;
    saveJsonData["framecompo.json"] =  temporary_framecompoJson_qss.themeJsonData;
    saveJsonData["menu.json"] =  temporary_menuJson_qss.themeJsonData;
    saveJsonData["splittercompo.json"] =  temporary_splittercompoJson_qss.themeJsonData;
    saveJsonData["topBar.json"] =  temporary_topBarJson_qss.themeJsonData;
    saveJsonData["highlightwk.json"] =  temporaryKWHighlighterData.jsonData;
    saveJsonData["lineNumber.json"] =  temporaryLineNumberColor.jsonData;

    CurrentThemeConfig myThemeCfg;
    myThemeCfg.currentThemeName = this->themeName;

    QStringList fileNameDesc{"currentTheme_codeeditor", "currentTheme_framecompo",
                             "currentTheme_menu", "currentTheme_splittercompo",
                             "currentTheme_topBar", "highlightwk_color","lineNumber_style"};
    this->saveUserThemeFile(fileNameDesc, myThemeCfg);
}

// 预览样式
void MyCustomStyle::preview_theme(const QString &themeName)
{
    if(themeName.isEmpty())
        return;
    tempCurrentThemeConfig.currentThemeName = themeName;
    tempCurrentThemeConfig.curThemePath["currentTheme_codeeditor"] = currentPreViewFileDir+"/"+themeName+"/codeeditor.json";
    tempCurrentThemeConfig.curThemePath["currentTheme_framecompo"] = currentPreViewFileDir+"/"+themeName+"/framecompo.json";
    tempCurrentThemeConfig.curThemePath["currentTheme_menu"] = currentPreViewFileDir+"/"+themeName+"/menu.json";
    tempCurrentThemeConfig.curThemePath["currentTheme_splittercompo"] = currentPreViewFileDir+"/"+themeName+"/splittercompo.json";
    tempCurrentThemeConfig.curThemePath["currentTheme_topBar"] = currentPreViewFileDir+"/"+themeName+"/topBar.json";
    tempCurrentThemeConfig.curThemePath["highlightwk_color"] = currentPreViewFileDir+"/"+themeName+"/highlightwk.json";
    tempCurrentThemeConfig.curThemePath["lineNumber_style"] = currentPreViewFileDir+"/"+themeName+"/lineNumber.json";

    temporary_topBarJson_qss = JsonParse::getJsonParse()->
                               parseThemeJsonFromFile(tempCurrentThemeConfig.curThemePath["currentTheme_topBar"]);
    temporary_menuJson_qss = JsonParse::getJsonParse()->
                             parseThemeJsonFromFile(tempCurrentThemeConfig.curThemePath["currentTheme_menu"]);
    temporary_framecompoJson_qss = JsonParse::getJsonParse()->
                                   parseThemeJsonFromFile(tempCurrentThemeConfig.curThemePath["currentTheme_framecompo"]);
    temporary_codeEditorJson_qss = JsonParse::getJsonParse()->
                                   parseThemeJsonFromFile(tempCurrentThemeConfig.curThemePath["currentTheme_codeeditor"]);
    temporary_splittercompoJson_qss = JsonParse::getJsonParse()->
                                      parseThemeJsonFromFile(tempCurrentThemeConfig.curThemePath["currentTheme_splittercompo"]);

    temporaryLineNumberColor = JsonParse::getJsonParse()->parseLineNumberJsonGetAllColor(
                                tempCurrentThemeConfig.curThemePath["lineNumber_style"]);

    temporaryKWHighlighterData = JsonParse::getJsonParse()->
                                 parseHighlighterColorsFromFile(tempCurrentThemeConfig.curThemePath["highlightwk_color"]);
}

void MyCustomStyle::on_btn_preview_on_mainWid_clicked()
{
    this->updateThemeValues();
    Config::config()->updateCurrentThemeStyle();
    Config::config()->updateAllKWHighlight(this->highlighter);
}

void MyCustomStyle::on_btn_cancel_preview_on_mainWid_clicked()
{
    if(isPreviewOnMainWidget && Config::config()->curThemeConfig.currentThemeName == old_themeName){
        Config::config()->topBarJson_qss = old_temporary_topBarJson_qss;
        Config::config()->menuJson_qss = old_temporary_menuJson_qss;
        Config::config()->framecompoJson_qss = old_temporary_framecompoJson_qss;
        Config::config()->codeEditorJson_qss = old_temporary_codeEditorJson_qss;
        Config::config()->splittercompoJson_qss = old_temporary_splittercompoJson_qss;

        Config::config()->lineNumberColor = old_temporaryLineNumberColor;
        Config::config()->KWHighlighterData = old_temporaryKWHighlighterData;

        Config::config()->updateCurrentThemeStyle();
        Config::config()->updateAllKWHighlight(this->highlighter);

        isPreviewOnMainWidget = false;
    }
}

void MyCustomStyle::on_btn_cancel_clicked()
{
    this->close();
}

void MyCustomStyle::closeEvent(QCloseEvent *event)
{
    /* 取消在主界面预览主题，关闭主题窗口时恢复原来主题，自动取消主界面预览*/
    this->on_btn_cancel_preview_on_mainWid_clicked();
    return QWidget::closeEvent(event);
}

