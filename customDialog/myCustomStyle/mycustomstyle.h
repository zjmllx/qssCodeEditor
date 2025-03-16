#ifndef MYCUSTOMSTYLE_H
#define MYCUSTOMSTYLE_H

#include "ui_mycustomstyle.h"
#include "../../myEditor/codeeditor.h"
#include "../../myEditor/highlighter.h"
#include "../../myCustomControl/myCustomBtn/jsonparse.h"

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
    void preViewTheme();  // 预览当前用户选中的我的主题项
    void initAllControl();

    // 修改的主内容、对象、属性、待修改的值
    bool modifyJsonValue(QString &jsonString, const QString &object,
                            const QString &property, const QString &newValue);

    QString getJsonValue(const QString &jsonString, const QString &object, const QString &property);

private:
    QMap<QString, QString> saveJsonData;
    QMenu *menu;
    CodeEditor* codeEditor;
    QFrame* fra_right;
    QSplitter *splitter;
    Highlighter *highlighter;

    QString projectDir;
    QString folderName;
    QString fileName;
    QString currentPreViewFileDir;
    QStringListModel listModel;

    QFileSystemWatcher *watcher;
    QString oldThemeName;
    QString newThemeName;

    /* 保存旧的那个样式，点击应用后，点击取消恢复上一个主题样式---------------------- */
    ParseThemeData old_temporary_topBarJson_qss,
        old_temporary_menuJson_qss,
        old_temporary_codeEditorJson_qss,
        old_temporary_framecompoJson_qss,
        old_temporary_splittercompoJson_qss;

    HighlighterData old_temporaryKWHighlighterData;
    LineNumberColor old_temporaryLineNumberColor;
    /* --------------------------------------------------------------------- */

    // json and qss
    ParseThemeData temporary_topBarJson_qss,
        temporary_menuJson_qss,
        temporary_codeEditorJson_qss,
        temporary_framecompoJson_qss,
        temporary_splittercompoJson_qss;

    HighlighterData temporaryKWHighlighterData;
    LineNumberColor temporaryLineNumberColor;

    bool createUserThemeDir(const QString &projectDir, const QString &folderName);
    bool saveUserThemeFile(const QMap<QString, QString> &saveJsonData);
    void updateThemeValues();

private:
    ~MyCustomStyle();
    Ui::mycustomstyle *ui;

private slots:
    void btn_tit_bgdcol_clicked();
    void btn_tit_hoverbgcol_cilcked();
    void btn_tit_fontcol_clicked();
    void btn_tit_fontStyle_clicked();
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
    void on_btn_onlyapply_clicked();
    void on_btn_cancel_clicked();

    void onIntoEdit(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void updateListView();
    void on_btn_resume_clicked();
};

#endif // MYCUSTOMSTYLE_H
