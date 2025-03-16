#include "mycustomstyle.h"
#include "config.h"
#include "../myColorDialog/mycolordialog.h"

#include <QDir>
#include <QFile>
#include <QFontDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QListView>
#include <QMessageBox>

MyCustomStyle::MyCustomStyle(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mycustomstyle)
{
    ui->setupUi(this);
    this->setWindowTitle("自定义主题");
    this->setWindowIcon(QIcon(":/img/images/theme.png"));

    codeEditor= new CodeEditor;
    fra_right = new QFrame;
    menu = new QMenu;

    codeEditor->appendPlainText("QWidget, QPushButton");
    highlighter = new Highlighter(codeEditor->document());

    // 初始化文件系统监视器
    watcher = new QFileSystemWatcher(this);

    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cdUp();
    this->createUserThemeDir(dir.absolutePath(),"userTheme");

    QStringList initListData;
    QDir dir_useTheme(this->currentPreViewFileDir);
    if (!dir.exists()) {
        qWarning() << "目录不存在：" << this->currentPreViewFileDir;
    }else{
        // 获取目录中的所有子目录名称
        initListData = dir_useTheme.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    }
    watcher->addPath(this->currentPreViewFileDir);
    listModel.setStringList(initListData);
    ui->listView->setModel(&listModel);

    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &MyCustomStyle::updateListView);
    // 连接信号和槽
    connect(ui->listView, &QListView::doubleClicked, this, [&](const QModelIndex &index){
        this->oldThemeName = index.data().toString();
    });
    connect(&listModel, &QStringListModel::dataChanged, this, &MyCustomStyle::onIntoEdit);

    //左上
    connect(ui->btn_tit_bgdcol,&QPushButton::clicked,this,&MyCustomStyle::btn_tit_bgdcol_clicked);
    connect(ui->btn_tit_hoverbgcol,&QPushButton::clicked,this,&MyCustomStyle::btn_tit_hoverbgcol_cilcked);
    connect(ui->btn_tit_fontcol,&QPushButton::clicked,this,&MyCustomStyle::btn_tit_fontcol_clicked);
    connect(ui->btn_tit_fontstyle,&QPushButton::clicked,this,&MyCustomStyle::btn_tit_fontStyle_clicked);

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

    temporary_topBarJson_qss = Config::config()->topBarJson_qss;
    temporary_menuJson_qss = Config::config()->menuJson_qss;
    temporary_codeEditorJson_qss = Config::config()->codeEditorJson_qss;
    temporary_framecompoJson_qss = Config::config()->framecompoJson_qss;
    temporary_splittercompoJson_qss = Config::config()->splittercompoJson_qss;

    this->temporaryLineNumberColor = Config::config()->lineNumberColor;
    this->temporaryKWHighlighterData = Config::config()->KWHighlighterData;

    /**/
    old_temporary_topBarJson_qss = temporary_topBarJson_qss;
    old_temporary_menuJson_qss = temporary_menuJson_qss;
    old_temporary_codeEditorJson_qss = temporary_codeEditorJson_qss;
    old_temporary_framecompoJson_qss = temporary_framecompoJson_qss;
    old_temporary_splittercompoJson_qss = temporary_splittercompoJson_qss;

    this->old_temporaryLineNumberColor = temporaryLineNumberColor;
    this->old_temporaryKWHighlighterData = temporaryKWHighlighterData;
     /**/

    this->setCurrentTheme();
    this->initAllControl();
}

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
}

void MyCustomStyle::preViewTheme()
{
    qDebug() << "预览我的主题";
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
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_fontstyle->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "font-family", fontFamily);
    this->modifyJsonValue(btn_style, "QPushButton", "font-size", fontSize);
    this->modifyJsonValue(btn_style, "QPushButton", "font-weight", fontWeight);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_tit_fontstyle->setStyleSheet(btn_style);

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
    colorStr = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QPlainTextEdit", "background-color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_bgdcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_edit_bgdcol->setStyleSheet(btn_style);

    // 初始化编辑器的字体颜色按钮
    colorStr = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QPlainTextEdit", "color");
    btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_edit_fontcol->styleSheet());
    this->modifyJsonValue(btn_style, "QPushButton", "background-color", colorStr);
    btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
    ui->btn_edit_fontcol->setStyleSheet(btn_style);

    // 初始化编辑器的字体样式按钮
    fontFamily = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QPlainTextEdit", "font-family");
    fontSize = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QPlainTextEdit", "font-size");
    fontWeight = this->getJsonValue(this->temporary_codeEditorJson_qss.themeJsonData, "QPlainTextEdit", "font-weight");
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

    // 修改指定对象和属性的值
    if (jsonObj.contains(object) && jsonObj[object].isObject()) {
        QJsonObject obj = jsonObj[object].toObject();
        obj[property] = newValue;  // 属性不存在，自动添加
        jsonObj[object] = obj;
    } else {
        qWarning() << "Object or property not found in JSON.";
        return false;
    }

    // 将修改后的 JSON 数据转换回字符串
    jsonDoc.setObject(jsonObj);
    jsonString = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
    return true;
}

QString MyCustomStyle::getJsonValue(const QString &jsonString, const QString &object, const QString &property)
{
    // 将 JSON 字符串转换为 QByteArray
    QByteArray jsonData = jsonString.toUtf8();

    // 解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return QString();
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 获取指定对象和属性的值
    if (jsonObj.contains(object) && jsonObj[object].isObject()) {
        QJsonObject obj = jsonObj[object].toObject();
        if (obj.contains(property)) {
            return obj[property].toString();
        } else {
            qWarning() << "Property not found in JSON object.";
            return QString();
        }
    } else {
        qWarning() << "Object not found in JSON.";
        return QString();
    }
}

bool MyCustomStyle::createUserThemeDir(const QString &projectDir, const QString &folderName)
{
    this->projectDir = projectDir;
    this->folderName = folderName;

    QDir dir(projectDir + "/" + folderName);

    // 如果文件夹不存在，则创建
    if (!dir.exists()) {
        if (dir.mkpath(".")) { // 创建目录
            qDebug() << "文件夹创建成功：" << dir.absolutePath();
        } else {
            qDebug() << "文件夹创建失败：" << dir.absolutePath();
            return false;
        }
    } else {
        qDebug() << "文件夹已存在：" << dir.absolutePath();
    }
    this->currentPreViewFileDir = dir.absolutePath();
    return true;
}

bool MyCustomStyle::saveUserThemeFile(const QMap<QString, QString> &saveJsonData)
{
    // 获取 QListView 的模型
    QAbstractItemModel *model = ui->listView->model();
    QString themeNum = QString::number(model->rowCount() + 1);

    bool ok;
    QString themeName;
    QString filePath;
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

    static const QRegularExpression reg("[<>:\"/\\|?*]");
    // 遍历 saveJsonData 并保存文件
    for (auto it = saveJsonData.cbegin(); it != saveJsonData.cend(); ++it) {
        QString key = it.key();
        QString value = it.value();

        // 检查并处理文件名中的非法字符
        key.replace(reg, "_");

        filePath = this->currentPreViewFileDir + "/" + themeName + "/" + key;

        QFile file(filePath);
        // 打开文件，如果文件不存在则创建文件
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "无法打开文件进行写入：" << file.errorString();
            return false;
        }
        file.write(value.toUtf8()); // 写入文件
        file.close();
    }

    qDebug() << "文件已创建或覆盖：" << themeName;
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
        if (dir.exists(oldThemeName)) {
            if (dir.rename(oldThemeName, newThemeName)) {
                qDebug() << "目录重命名成功：" << oldThemeName << " -> " << newThemeName;
            } else {
                qWarning() << "目录重命名失败：" << oldThemeName << " -> " << newThemeName;
            }
        } else {
            qWarning() << "目录不存在：" << oldThemeName;
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

// 菜单栏背景颜色
void MyCustomStyle::btn_tit_bgdcol_clicked()
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
void MyCustomStyle::btn_tit_hoverbgcol_cilcked()
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
void MyCustomStyle::btn_tit_fontcol_clicked()
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
void MyCustomStyle::btn_tit_fontStyle_clicked()
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

        QString btn_style = JsonParse::getJsonParse()->qssToJson(ui->btn_tit_fontstyle->styleSheet());
        this->modifyJsonValue(btn_style, "QPushButton", "font-family", fontPart[0]);
        this->modifyJsonValue(btn_style, "QPushButton", "font-size", fontPart[1]+"pt");
        this->modifyJsonValue(btn_style, "QPushButton", "font-weight", fontPart[fontPart.size()-1]);

        btn_style = JsonParse::getJsonParse()->parseThemeJsontoQss(btn_style);
        ui->btn_tit_fontstyle->setStyleSheet(btn_style);

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
    this->modifyJsonValue(style, "QPlainTextEdit", "background-color", color.name(QColor::HexArgb));
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
    this->modifyJsonValue(style, "QPlainTextEdit", "color", color.name(QColor::HexArgb));
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
        this->modifyJsonValue(style, "QPlainTextEdit", "font-family", fontPart[0]);
        this->modifyJsonValue(style, "QPlainTextEdit", "font-size", fontPart[1] + "pt");
        this->modifyJsonValue(style, "QPlainTextEdit", "font-weight", fontPart[fontPart.size() - 1]);
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

void MyCustomStyle::on_btn_save_apply_clicked()
{
    this->updateThemeValues();

    saveJsonData["topBar.json"] =  temporary_topBarJson_qss.themeJsonData;
    saveJsonData["menu.json"] =  temporary_menuJson_qss.themeJsonData;
    saveJsonData["codeeditor.json"] =  temporary_codeEditorJson_qss.themeJsonData;
    saveJsonData["framecompo.json"] =  temporary_framecompoJson_qss.themeJsonData;
    saveJsonData["splittercompo.json"] =  temporary_splittercompoJson_qss.themeJsonData;

    saveJsonData["highlightwk.json"] =  temporaryKWHighlighterData.jsonData;
    saveJsonData["lineNumber.json"] =  temporaryLineNumberColor.jsonData;

    this->saveUserThemeFile(saveJsonData);
    Config::config()->updataCurrentThemeStyle();
}


void MyCustomStyle::on_btn_onlysave_clicked()
{
    this->updateThemeValues();

    saveJsonData["topBar.json"] =  temporary_topBarJson_qss.themeJsonData;
    saveJsonData["menu.json"] =  temporary_menuJson_qss.themeJsonData;
    saveJsonData["codeeditor.json"] =  temporary_codeEditorJson_qss.themeJsonData;
    saveJsonData["framecompo.json"] =  temporary_framecompoJson_qss.themeJsonData;
    saveJsonData["splittercompo.json"] =  temporary_splittercompoJson_qss.themeJsonData;

    saveJsonData["highlightwk.json"] =  temporaryKWHighlighterData.jsonData;
    saveJsonData["lineNumber.json"] =  temporaryLineNumberColor.jsonData;
    this->saveUserThemeFile(saveJsonData);
}

void MyCustomStyle::on_btn_onlyapply_clicked()
{
    this->updateThemeValues();
    Config::config()->updataCurrentThemeStyle();
    Config::config()->KWHighlighterData = temporaryKWHighlighterData;
    Config::config()->updataAllKWHighlight();
}

void MyCustomStyle::on_btn_cancel_clicked()
{
    this->close();
}

void MyCustomStyle::on_btn_resume_clicked()
{
    Config::config()->topBarJson_qss = old_temporary_topBarJson_qss;
    Config::config()->menuJson_qss = old_temporary_menuJson_qss,
    Config::config()->codeEditorJson_qss = old_temporary_codeEditorJson_qss,
    Config::config()->framecompoJson_qss = old_temporary_framecompoJson_qss,
    Config::config()->splittercompoJson_qss = old_temporary_splittercompoJson_qss;

    Config::config()->KWHighlighterData = old_temporaryKWHighlighterData;
    Config::config()->lineNumberColor = old_temporaryLineNumberColor;

    Config::config()->updataCurrentThemeStyle();
    Config::config()->updataAllKWHighlight();
}

