#ifndef MYCUSTOMSTYLE_H
#define MYCUSTOMSTYLE_H

#include "ui_mycustomstyle.h"
#include "../../myEditor/codeeditor.h"
#include "../../myEditor/highlighter.h"
#include "../../myCustomControl/myCustomBtn/jsonparse.h"
#include "config.h"

#include <QBoxLayout>
#include <QWidget>
#include <QMenu>
#include <QSplitter>
#include <QStringListModel>
#include <QJsonObject>
#include <QFontDialog>
#include <QFileSystemWatcher>

namespace Ui {
class mycustomstyle;
}

class MyCustomStyle : public QWidget
{
    Q_OBJECT

public:
    explicit MyCustomStyle(QWidget *parent = nullptr);

    void setCurrentTheme();
    void initAllControl();

    // 修改的主内容、对象、属性、待修改的值
    bool modifyJsonValue(QString &jsonString, const QString &object,
                            const QString &property, const QString &newValue);


    /* 修改颜色值 -- 参数：jsondata， 修改的对象， 新的颜色*/
    bool modifyColorValue(QString &jsonString, const QString &object, const QColor &newColor);

    /* 修改颜色值 */
    QString getJsonValue(const QString &jsonString, const QString &object, const QString &property);

private:
    QMap<QString, QString> saveJsonData; // key：部件样式表文件名， value：样式表文件内容
    QStringList keyOrder; // 保持对QMap的key的插入顺序，避免qmap自动排序导致错位

    QMenu *menu;
    CodeEditor* codeEditor;
    QFrame* fra_right;
    QSplitter *splitter;
    Highlighter *highlighter;

    QString projectDir; // 项目目录
    QString folderName; // 存放用户自定义主题的文件夹名
    QString filePath; // 主题目录下的文件路径
    QString themeName; // 保存当前预览的主题名称

    CurrentThemeConfig tempCurrentThemeConfig; // 临时主题配置,用于我的主题列表预览、应用主题操作

    bool setThemeName();  // 设置主题名，用于保存主题时用

    QString currentPreViewFileDir; // 存放用户自定义主题的文件夹路径

    QStringListModel listModel;

    QFileSystemWatcher *watcher;
    QString selectedThemeName;
    QString newThemeName;

    /* 保存旧的那个样式，点击应用后，点击取消恢复上一个主题样式---------------------- */
    QString old_themeName;
    ParseThemeData old_temporary_topBarJson_qss,
        old_temporary_menuJson_qss,
        old_temporary_codeEditorJson_qss,
        old_temporary_framecompoJson_qss,
        old_temporary_splittercompoJson_qss;

    HighlighterData old_temporaryKWHighlighterData;
    LineNumberColor old_temporaryLineNumberColor;
    void upDateOldTheme();
    /* --------------------------------------------------------------------- */

    // json and qss
    ParseThemeData temporary_topBarJson_qss,
        temporary_menuJson_qss,
        temporary_codeEditorJson_qss,
        temporary_framecompoJson_qss,
        temporary_splittercompoJson_qss;

    HighlighterData temporaryKWHighlighterData;
    LineNumberColor temporaryLineNumberColor;

    void createUserThemeDir(const QString &projectDir, const QString &folderName);
    bool saveUserThemeFile(const QStringList &fileNameDesc, CurrentThemeConfig &temConfig);
    void updateThemeValues(); // 更新主题的数据

    bool copyDirectoryRecursively(const QString &srcPath, const QString &dstPath); // 辅助函数：递归复制目录
    /* 辅助函数，重命名主题名时如果该主题为应用且保存的主题，则相应更改配置文件*/
    void renameThemeToUpdateIniFile();

    // 预览我的主题
    bool isPreviewOnMainWidget; // 是否处于主界面预览状态
    void preview_theme(const QString &themeName);

private:
    ~MyCustomStyle();
    Ui::mycustomstyle *ui;

private slots:
    void onListViewCustomContextMenu(const QPoint &pos);

    void on_btn_tit_bgdcol_clicked();
    void on_btn_tit_hoverbgcol_clicked();

    void on_btn_tit_fontcol_clicked();
    void on_btn_tit_fontStyle_clicked();
    void on_btn_tit_menu_bg_col_clicked();
    void on_btn_tit_menu_hoverbgcol_clicked();
    void on_btn_tit_menu_fontcol_clicked();
    void on_btn_tit_menu_fontStyle_clicked();
    void on_btn_tit_menu_fonthovercol_clicked();

    void on_btn_edit_bgdcol_clicked();
    void on_btn_edit_fontcol_clicked();
    void on_btn_edit_fontstyle_clicked();
    void on_btn_line_num_bgdcol_clicked();
    void on_btn_line_fontcol_clicked();
    void on_btn_line_sel_fontcol_clicked();

    void on_btn_kw_main_control_clicked();
    void on_btn_kw_fake_states_clicked();
    void on_btn_kw_chidren_control_clicked();
    void on_btn_kw_style_attri_clicked();
    void on_btn_kw_lay_attri_clicked();
    void on_btn_kw_opacity_attri_clicked();
    void on_btn_save_apply_clicked();
    void on_btn_onlysave_clicked();
    void on_btn_cancel_clicked();

    void onIntoEdit(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void updateListView();

    void on_btn_preview_on_mainWid_clicked();
    void on_btn_cancel_preview_on_mainWid_clicked();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MYCUSTOMSTYLE_H
